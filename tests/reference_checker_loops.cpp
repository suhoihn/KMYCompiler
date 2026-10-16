// Resolved-AST loop/transfer tests: no KMY code generation or runtime execution.
#include "../Semantics/ReferenceChecker.hpp"
#include "../Core/newParser.hpp"
#include "../Core/errorhandler.hpp"
#include "../Core/value.hpp"
#include <stdexcept>
#include <string>

// Identity marker only: checker tests never invoke this runtime callback.
static Value dummyBuiltin(int, Value*) { return Value{}; }

struct Fixture {
    ArrayType arrayType{&Types::INT_TYPE};
    ArrayType literalType{&arrayType};
    Type boxType{TypeKind::INSTANCE};
    FunctionType predicateType{{&arrayType}, &Types::BOOL_TYPE};
    VarSymbol predicate{"consumeAsBool", false};
    VarSymbol x{"x", true};
    VarSymbol y{"y", false};
    Module module;

    explicit Fixture(Type* valueType = nullptr) {
        x.type = y.type = valueType ? valueType : &arrayType;
        literalType.elementType = x.type;
        predicate.type = &predicateType;
        predicateType.paramTypes = {x.type};
        auto declaration = std::make_shared<Let>(nullptr, "x", fresh(), true, false);
        declaration->symbol = &x;
        module.topLevelStatements.push_back(declaration);
    }

    ExprPtr fresh() {
        if (x.type == &Types::INT_TYPE) {
            auto value = std::make_shared<Literal>(42);
            value->type = x.type;
            return value;
        }
        if (x.type->kind == TypeKind::SHARED) {
            auto value = std::make_shared<NewExpr>("Foo", std::vector<ExprPtr>{});
            value->type = x.type;
            return value;
        }
        auto value = std::make_shared<ArrayLiteral>(std::vector<ExprPtr>{});
        value->type = x.type;
        return value;
    }

    ExprPtr array(std::vector<ExprPtr> elements) {
        auto value = std::make_shared<ArrayLiteral>(std::move(elements));
        value->type = &literalType;
        return value;
    }

    ExprPtr construct(std::vector<ExprPtr> arguments) {
        auto value = std::make_shared<NewExpr>("Box", std::move(arguments));
        value->type = &boxType;
        return value;
    }

    ExprPtr read() {
        auto value = std::make_shared<Variable>("x");
        value->symbol = &x;
        value->type = x.type;
        return value;
    }

    ExprPtr condition() {
        auto value = std::make_shared<Literal>(true);
        value->type = &Types::BOOL_TYPE;
        return value; // Checker conservatively allows both condition outcomes.
    }

    ExprPtr qualifiedRead() {
        auto value = std::make_shared<ScopeAccessExpr>(std::vector<std::string>{"math", "x"});
        value->symbol = &x;
        value->type = x.type;
        return value;
    }

    ExprPtr consumeAsBool() {
        // Model a resolved external by-value function returning bool.
        auto callee = std::make_shared<Variable>("consumeAsBool");
        callee->symbol = &predicate;
        callee->type = &predicateType;
        auto call = std::make_shared<Call>(callee, std::vector<ExprPtr>{read()});
        call->type = &Types::BOOL_TYPE;
        return call;
    }

    ExprPtr callableRead(bool qualified = false) {
        if (qualified) {
            auto value = std::make_shared<ScopeAccessExpr>(std::vector<std::string>{"native", "consumeAsBool"});
            value->symbol = &predicate;
            value->type = &predicateType;
            return value;
        }
        auto value = std::make_shared<Variable>("consumeAsBool");
        value->symbol = &predicate;
        value->type = &predicateType;
        return value;
    }

    StmtPtr moveX() {
        auto declaration = std::make_shared<Let>(nullptr, "y", read(), false, false);
        declaration->symbol = &y;
        return declaration;
    }

    StmtPtr restoreX() {
        auto assignment = std::make_shared<Assignment>(AssignmentOp::Assign, read(), fresh());
        assignment->type = &arrayType;
        return std::make_shared<ExprStmt>(assignment);
    }

    void loop(std::vector<StmtPtr> body) {
        module.topLevelStatements.push_back(std::make_shared<While>(
            condition(), std::make_shared<Block>(std::move(body))));
    }

    void expect(const std::string& error = "") {
        try {
            ReferenceChecker(module).check();
        } catch (const KMYCompileError& failure) {
            if (!error.empty() && std::string(failure.what()).find(error) != std::string::npos) return;
            throw;
        }
        if (!error.empty()) throw std::runtime_error("Expected ownership diagnostic: " + error);
    }
};

int main() {
    { // Stable fallthrough: analysis converges without executing the source loop.
        Fixture f;
        f.loop({std::make_shared<ExprStmt>(f.read())});
        f.expect();
    }
    { // Move on normal completion: the second iteration cannot read x.
        Fixture f;
        f.loop({f.moveX()});
        f.expect("not available on every control-flow path");
    }
    { // Continue carries a move back to the condition.
        Fixture f;
        f.loop({f.moveX(), std::make_shared<Continue>()});
        f.expect("not available on every control-flow path");
    }
    { // Restoring x before continue permits repeated iterations.
        Fixture f;
        f.loop({f.moveX(), f.restoreX(), std::make_shared<Continue>()});
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect();
    }
    { // Break snapshots survive an if with neither branch falling through.
        Fixture f;
        f.loop({std::make_shared<If>(f.condition(),
            std::make_shared<Block>(std::vector<StmtPtr>{f.moveX(), std::make_shared<Break>()}),
            std::make_shared<Break>())});
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect("not available on every control-flow path");
    }
    { // Return has no back edge; the zero-iteration path still preserves x.
        Fixture f;
        f.loop({std::make_shared<Return>(f.read())});
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect();
    }
    { // Nested break targets only the innermost loop.
        Fixture f;
        auto inner = std::make_shared<While>(f.condition(), std::make_shared<Break>());
        f.loop({inner, std::make_shared<Continue>()});
        f.expect();
    }
    { // Constructor consumes a Unique argument; subsequent reads fail.
        Fixture f;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.construct({f.read()})));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect("Use of moved value");
    }
    { // Constructor visits/consumes one argument before reading the next.
        Fixture f;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.construct({f.read(), f.read()})));
        f.expect("Use of moved value");
    }
    { // Unique array elements transfer ownership into their slots.
        Fixture f;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.array({f.read()})));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect("Use of moved value");
    }
    { // Duplicate Unique elements fail on the second read.
        Fixture f;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.array({f.read(), f.read()})));
        f.expect("Use of moved value");
    }
    { // Copy values can be repeated in arrays and constructor arguments.
        Fixture f(&Types::INT_TYPE);
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.array({f.read(), f.read()})));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.construct({f.read(), f.read()})));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect();
    }
    { // Shared copies keep their source available; no RC runtime is tested here.
        Type fooType{TypeKind::INSTANCE};
        SharedType sharedType{&fooType};
        Fixture f(&sharedType);
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.array({f.read(), f.read()})));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.construct({f.read(), f.read()})));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect();
    }
    { // Moving math::x invalidates reads through another spelling of its symbol.
        Fixture f;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.array({f.qualifiedRead()})));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect("Use of moved value");
    }
    { // A qualified overwrite restores the same whole binding.
        Fixture f;
        f.module.topLevelStatements.push_back(f.moveX());
        auto assignment = std::make_shared<Assignment>(AssignmentOp::Assign, f.qualifiedRead(), f.fresh());
        assignment->type = &f.arrayType;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(assignment));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.qualifiedRead()));
        f.expect();
    }
    for (auto op : {BinaryOp::LogicalAnd, BinaryOp::LogicalOr, BinaryOp::NullCoalesce}) {
        // Optional RHS consumption makes x MaybeUnavailable, not definitely Moved.
        Fixture f;
        ExprPtr left = f.condition();
        NullableType nullableBool{&Types::BOOL_TYPE};
        VarSymbol flag{"flag", false};
        if (op == BinaryOp::NullCoalesce) {
            flag.type = &nullableBool;
            auto value = std::make_shared<Literal>(nullptr);
            value->type = &Types::NULL_TYPE;
            auto declaration = std::make_shared<Let>(nullptr, "flag", value, false, false);
            declaration->symbol = &flag;
            f.module.topLevelStatements.push_back(declaration);
            auto variable = std::make_shared<Variable>("flag");
            variable->symbol = &flag;
            variable->type = &nullableBool;
            left = variable;
        }
        auto binary = std::make_shared<BinaryExpr>(op, left, f.consumeAsBool());
        binary->type = &Types::BOOL_TYPE;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(binary));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect("not available on every control-flow path");
    }
    for (auto op : {BinaryOp::LogicalAnd, BinaryOp::LogicalOr}) {
        // Copy argument use on an optional path does not invalidate its source.
        Fixture f(&Types::INT_TYPE);
        auto binary = std::make_shared<BinaryExpr>(op, f.condition(), f.consumeAsBool());
        binary->type = &Types::BOOL_TYPE;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(binary));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect();
    }
    { // Ordinary operators still evaluate RHS unconditionally.
        Fixture f;
        auto binary = std::make_shared<BinaryExpr>(BinaryOp::EqualEqual,
            f.condition(), f.consumeAsBool());
        binary->type = &Types::BOOL_TYPE;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(binary));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect("Use of moved value");
    }
    for (bool consumeResult : {false, true}) {
        // Reading ?? doesn't move its arms; transferring its Unique result does.
        Fixture f;
        NullableType nullableArray{&f.arrayType};
        VarSymbol optional{"optional", false};
        VarSymbol result{"result", false};
        optional.type = &nullableArray;
        result.type = &f.arrayType;
        auto declaration = std::make_shared<Let>(nullptr, "optional", f.fresh(), false, false);
        declaration->symbol = &optional;
        f.module.topLevelStatements.push_back(declaration);
        auto left = std::make_shared<Variable>("optional");
        left->symbol = &optional;
        left->type = &nullableArray;
        auto binary = std::make_shared<BinaryExpr>(BinaryOp::NullCoalesce, left, f.read());
        binary->type = &f.arrayType;
        if (consumeResult) {
            auto binding = std::make_shared<Let>(nullptr, "result", binary, false, false);
            binding->symbol = &result;
            f.module.topLevelStatements.push_back(binding);
        } else {
            f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(binary));
        }
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect(consumeResult ? "not available on every control-flow path" : "");
    }
    for (bool qualified : {false, true}) {
        // Builtins are shareable callables; repeated uses do not move their source.
        Fixture f;
        f.predicate.nativeFnPtr = dummyBuiltin;
        ArrayType callableArray{&f.predicateType};
        auto values = std::make_shared<ArrayLiteral>(std::vector<ExprPtr>{
            f.callableRead(qualified), f.callableRead(qualified)});
        values->type = &callableArray;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(values));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.callableRead(qualified)));
        f.expect();
    }
    { // Copying a builtin into a binding leaves the original builtin available.
        Fixture f;
        f.predicate.nativeFnPtr = dummyBuiltin;
        VarSymbol alias{"alias", false};
        alias.type = &f.predicateType;
        auto declaration = std::make_shared<Let>(nullptr, "alias", f.callableRead(), false, false);
        declaration->symbol = &alias;
        f.module.topLevelStatements.push_back(declaration);
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.callableRead()));
        auto aliasRead = [&]() -> ExprPtr {
            auto value = std::make_shared<Variable>("alias");
            value->symbol = &alias;
            value->type = alias.type;
            return value;
        };
        ArrayType callableArray{&f.predicateType};
        auto values = std::make_shared<ArrayLiteral>(std::vector<ExprPtr>{aliasRead(), aliasRead()});
        values->type = &callableArray;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(values));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(aliasRead()));
        f.expect();
    }
    { // User callable values are shareable without a builtin identity marker.
        Fixture f;
        ArrayType callableArray{&f.predicateType};
        auto values = std::make_shared<ArrayLiteral>(std::vector<ExprPtr>{
            f.callableRead(), f.callableRead()});
        values->type = &callableArray;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(values));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.callableRead()));
        f.expect();
    }
    { // Closure aliases share their value; capture transfer/RC is not tested here.
        Fixture f;
        FunctionType closureType{{}, &Types::VOID_TYPE};
        VarSymbol closure{"closure", false};
        closure.type = &closureType;
        auto expression = std::make_shared<FunctionExpr>(std::vector<Parameter>{},
            std::make_shared<Block>(std::vector<StmtPtr>{std::make_shared<ExprStmt>(f.read())}), nullptr);
        expression->type = &closureType;
        auto declaration = std::make_shared<Let>(nullptr, "closure", expression, false, false);
        declaration->symbol = &closure;
        f.module.topLevelStatements.push_back(declaration);
        auto closureRead = [&]() -> ExprPtr {
            auto value = std::make_shared<Variable>("closure");
            value->symbol = &closure;
            value->type = &closureType;
            return value;
        };
        ArrayType callableArray{&closureType};
        auto values = std::make_shared<ArrayLiteral>(std::vector<ExprPtr>{closureRead(), closureRead()});
        values->type = &callableArray;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(values));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(closureRead()));
        f.expect();
    }
    { // Fresh Unique temporaries have no source binding to invalidate.
        Fixture f;
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.array({f.fresh(), f.fresh()})));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.construct({f.fresh()})));
        f.module.topLevelStatements.push_back(std::make_shared<ExprStmt>(f.read()));
        f.expect();
    }
}
