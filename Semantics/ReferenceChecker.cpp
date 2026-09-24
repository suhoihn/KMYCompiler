#include "ReferenceChecker.hpp"

#include "../Core/errorhandler.hpp"
#include "../Core/newParser.hpp"

ReferenceChecker::ReferenceChecker(Module& module)
    : module(module) {}

OwnershipKind ReferenceChecker::ownershipKind(const Type* type) const {
    if (!type) {
        throw KMYCompileError("Cannot classify ownership of an unresolved type.");
    }

    switch (type->kind) {
        // Plain machine values may be duplicated without ownership work.
        case TypeKind::INT:
        case TypeKind::DOUBLE:
        case TypeKind::BOOL:
        case TypeKind::POINTER:
        case TypeKind::CELL:
        case TypeKind::NULLTYPE:
        case TypeKind::VOID:
        case TypeKind::ENUM:
            return OwnershipKind::Copy;

        case TypeKind::SHARED:
            return OwnershipKind::Shared;

        // A nullable value keeps the ownership policy of the value it wraps.
        case TypeKind::NULLABLE:
            return ownershipKind(static_cast<const NullableType*>(type)->innerType);

        // KMY strings currently have no owned-string representation: literals
        // and runtime-produced char pointers share the same semantic type.
        // Preserve today's copy behaviour until that representation is split.
        case TypeKind::STRING:
            return OwnershipKind::Copy;

        // Heap aggregates, arrays, closures/functions, structural values, and
        // dynamically typed values are conservatively move-only by default.
        case TypeKind::ARRAY:
        case TypeKind::STRUCTUAL:
        case TypeKind::INSTANCE:
        case TypeKind::FUNCTION:
        case TypeKind::ANY:
            return OwnershipKind::Unique;

        case TypeKind::UNKNOWN:
        case TypeKind::UNINITIALISED:
            throw KMYCompileError("Cannot classify ownership before a value has a resolved type.");
    }

    throw KMYCompileError("Unknown type ownership classification.");
}

void ReferenceChecker::requireAvailable(
    const VarSymbol* symbol,
    const std::string& name
) const {
    auto found = currentMap.find(symbol);

    // Built-ins, imported values, and hoisted declarations may not have a
    // source-order entry in this function's map. Resolver already validated
    // that those names exist, so this pass only diagnoses states it tracks.
    if (found == currentMap.end()) return;

    switch (found->second) {
        case BindingAvailability::Available:
            return;
        case BindingAvailability::Uninitialized:
            throw KMYCompileError("Use of uninitialized value \"" + name + "\".");
        case BindingAvailability::Moved:
            throw KMYCompileError("Use of moved value \"" + name + "\".");
        case BindingAvailability::MaybeUnavailable:
            throw KMYCompileError(
                "Value \"" + name + "\" is not available on every control-flow path."
            );
    }
}

void ReferenceChecker::consumeValue(const ExprPtr& expression) {
    // First validate every ordinary read nested inside the expression.
    expression->accept(*this);

    // This first ownership step supports whole-variable moves only. Calls and
    // temporaries transfer fresh results, while field/index partial moves will
    // receive their own explicit rules later.
    if (expression->kind != ExprKind::Variable) return;

    const auto variable = std::static_pointer_cast<Variable>(expression);
    if (ownershipKind(variable->type) == OwnershipKind::Unique) {
        requireAvailable(variable->symbol, variable->name);
        currentMap[variable->symbol] = BindingAvailability::Moved;
    }
    // Copy values leave the source available. Shared values will eventually
    // cause IRBuilder to emit RETAIN and also leave the source available.
}

void ReferenceChecker::check() {
    // Visit real module statements, not the legacy synthetic program wrapper.
    for (const StmtPtr& statement : module.topLevelStatements) {
        statement->accept(*this);
    }
}

void ReferenceChecker::visit(Variable& e) {
    requireAvailable(e.symbol, e.name);
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

    AvailabilityMap oldMap = currentMap;

    for (const Parameter& parameter : e.params) {
        currentMap[parameter.symbol] = BindingAvailability::Available;
        if (parameter.defaultValue) {
            parameter.defaultValue->accept(*this);
        }
    }
    e.body->accept(*this);

    currentMap = oldMap;
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

    // The name exists after symbol building, but it does not own a usable
    // value until its initializer has been checked and transferred.
    currentMap[s.symbol] = BindingAvailability::Uninitialized;

    if (!s.expr) return;

    if (s.isFunctionDecl) {
        // Function declarations are hoisted and may recursively reference
        // their own binding while their function body is being checked.
        currentMap[s.symbol] = BindingAvailability::Available;
        s.expr->accept(*this);
        return;
    }

    consumeValue(s.expr);
    currentMap[s.symbol] = BindingAvailability::Available;
}

void ReferenceChecker::visit(Return& s) {
    if (s.expr) {
        s.expr->accept(*this);
    }

    // Future T& work: trace a returned reference back to its origin.  The
    // initial rule should allow globals and reference parameters, but reject
    // locals and upvalues whose storage may die after this call.
}
