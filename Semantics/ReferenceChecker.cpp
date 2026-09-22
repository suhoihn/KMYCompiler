#include "ReferenceChecker.hpp"

#include "../Core/errorhandler.hpp"
#include "../Core/newParser.hpp"

ReferenceChecker::ReferenceChecker(Module& module)
    : module(module) {}

void ReferenceChecker::check() {
    // Visit real module statements, not the legacy synthetic program wrapper.
    for (const StmtPtr& statement : module.topLevelStatements) {
        statement->accept(*this);
    }
}

void ReferenceChecker::requireMutableLValue(
    const ExprPtr& expression,
    const char* operation
) {
    if (expression->kind == ExprKind::Variable) {
        const auto variable = std::static_pointer_cast<Variable>(expression);
        if (!variable->symbol->isMutable) {
            throw KMYCompileError(
                std::string("Cannot ") + operation + " immutable binding \"" +
                variable->name + "\"; declare it with `var` instead of `let`."
            );
        }
        return;
    }

    if (expression->kind == ExprKind::Get) {
        const auto get = std::static_pointer_cast<Get>(expression);
        if (get->resolvedMethod || !get->obj->type ||
            get->obj->type->kind != TypeKind::INSTANCE) {
            return;
        }

        auto* instance = static_cast<InstanceType*>(get->obj->type);
        auto field = instance->fieldMap.find(get->name);
        if (field != instance->fieldMap.end() && !field->second->isMutable) {
            throw KMYCompileError(
                std::string("Cannot ") + operation + " immutable field \"" +
                get->name + "\"; declare it with `var` instead of `let`."
            );
        }
    }

    // Index and pointer-dereference writes are deliberately not treated as
    // rebinding their base variable. Raw-pointer pointee mutability and deep
    // collection immutability require future reference/const types.
}

void ReferenceChecker::visit(UnaryExpr& e) {
    e.operand->accept(*this);

    if (e.op == UnaryOp::PreIncrement || e.op == UnaryOp::PreDecrement ||
        e.op == UnaryOp::PostIncrement || e.op == UnaryOp::PostDecrement) {
        requireMutableLValue(e.operand, "modify");
    }

    if (e.op == UnaryOp::AddressOf) {
        // Future T& work: classify the operand's storage as local, parameter,
        // global, field, index, dereference, or captured/upvalue storage.
        // Resolver already performs today's pointer addressability check.
    }
}

void ReferenceChecker::visit(Assignment& e) {
    e.left->accept(*this);
    e.right->accept(*this);

    // Declaration initialization is not reassignment. This flag is currently
    // used by the compiler-generated `this.field = initializer` stores.
    if (!e.isInitialisation) {
        requireMutableLValue(e.left, "assign to");
    }

    // Future T& work: reject storing a short-lived reference into storage that
    // can outlive its source (for example a global, field, or captured cell).
}

void ReferenceChecker::visit(FunctionExpr& e) {
    FunctionExpr* enclosingFunction = currentFunction;
    currentFunction = &e;

    for (const Parameter& parameter : e.params) {
        if (parameter.defaultValue) {
            parameter.defaultValue->accept(*this);
        }
    }
    e.body->accept(*this);

    currentFunction = enclosingFunction;
}

void ReferenceChecker::visit(Let& s) {
    // An immutable binding could never receive a value after its declaration,
    // so require its one legal write to occur in the declaration itself.
    if (!s.isMutable && !s.expr) {
        throw KMYCompileError(
            "Immutable binding \"" + s.name +
            "\" requires an initializer; use `var` for deferred assignment."
        );
    }

    if (s.expr) {
        s.expr->accept(*this);
    }
}

void ReferenceChecker::visit(Return& s) {
    if (s.expr) {
        s.expr->accept(*this);
    }

    // Future T& work: trace a returned reference back to its origin.  The
    // initial rule should allow globals and reference parameters, but reject
    // locals and upvalues whose storage may die after this call.
}
