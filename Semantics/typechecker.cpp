#include "typechecker.hpp"

#include "../Core/errorhandler.hpp"
#include <unordered_map>

static std::unordered_map<ASTNode*, Type*> typeInfo;
static std::unordered_map<ASTNode*, SymbolPtr> symbolInfo;

static Type* getType(ASTNode* n) {
    auto it = typeInfo.find(n);

    if (it == typeInfo.end())
        return nullptr;

    return it->second;
}

static void setType(ASTNode* n, Type* t) {
    typeInfo[n] = t;
}


void TypeChecker::visit(Literal& e) {
    e.type = std::visit([](auto&& value) -> Type* {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, int>)
            return &Types::INT_TYPE;
        else if constexpr (std::is_same_v<T, double>)
            return &Types::DOUBLE_TYPE;
        else if constexpr (std::is_same_v<T, bool>)
            return &Types::BOOL_TYPE;
        else
            return nullptr;

    }, e.value);
}
void TypeChecker::visit(ArrayLiteral& e) {
    bool uniformType = true;
    Type* baseType = nullptr;
    for (auto& elem : e.elements) {
        elem->accept(*this);
        if (baseType) {
            if (baseType != elem->type) {
                throw KMYCompileError("Unmatched type.");
            }
        } else {
            baseType = elem->type;
        }
        
    }
}
void TypeChecker::visit(RecordLiteral& e) {}
void TypeChecker::visit(Variable& e) {
    if (!e.symbol) {
        throw KMYCompileError("Unresolved variable.");
    }

    if (!e.symbol->type) {
        throw KMYCompileError("Variable has no type.");
    }

    e.type = e.symbol->type;
}

Type* testBinary(BinaryOp op, Type* leftType, Type* rightType) {
    switch (op) {
        case BinaryOp::Plus:
        case BinaryOp::Minus:
        case BinaryOp::Star:
        case BinaryOp::Slash: {
            if (leftType == &Types::INT_TYPE && rightType == &Types::INT_TYPE) {
                return &Types::INT_TYPE;
            } else if (leftType == &Types::INT_TYPE && rightType == &Types::DOUBLE_TYPE) {
                return &Types::DOUBLE_TYPE;
            } else if (leftType == &Types::DOUBLE_TYPE && rightType == &Types::INT_TYPE) {
                return &Types::DOUBLE_TYPE;
            } else if (leftType == &Types::DOUBLE_TYPE && rightType == &Types::DOUBLE_TYPE) {
                return &Types::DOUBLE_TYPE;
            }
        }



        case BinaryOp::Percent: 
        
        {
            if (toDouble(right) == 0.0)
                throw std::runtime_error("Modulo by zero");
            
            if (isDouble(left)) {
                throw std::runtime_error("Divident cannot be double.");
            }

            if (isDouble(right)) {
                throw std::runtime_error("Divisor cannot be double.");
            }

            // C-style: integer modulo only
            return Value(toInt(left) % toInt(right));
        }
            
        case BinaryOp::LShift: {
            if (isDouble(left) || isDouble(right)) {
                throw std::runtime_error("Invalid operand.");
            }
            return Value(toInt(left) << toInt(right));
        }

        case BinaryOp::RShift: {
            if (isDouble(left) || isDouble(right)) {
                throw std::runtime_error("Invalid operand.");
            }
            return Value(toInt(left) >> toInt(right));
        }

        case BinaryOp::BitAnd: {
            if (isDouble(left) || isDouble(right)) {
                throw std::runtime_error("Invalid operand.");
            }
            return Value(toInt(left) & toInt(right));
        }

        case BinaryOp::BitOr: {
            if (isDouble(left) || isDouble(right)) {
                throw std::runtime_error("Invalid operand.");
            }
            return Value(toInt(left) | toInt(right));
        }

        case BinaryOp::BitXor:{
            if (isDouble(left) || isDouble(right)) {
                throw std::runtime_error("Invalid operand.");
            }
            return Value(toInt(left) ^ toInt(right));
        }

        default:
            break;
    }
}

    // ========================
    // LOGICAL
    // ========================
    if (op == BinaryOp::LogicalAnd)
        return Value(isTruthy(left) && isTruthy(right));

    if (op == BinaryOp::LogicalOr)
        return Value(isTruthy(left) || isTruthy(right));

    // ========================
    // COMPARISON
    // ========================
    if (op == BinaryOp::EqualEqual)
        return Value(isEqual(left, right));

    if (op == BinaryOp::NotEqual)
        return Value(!isEqual(left, right));

    if (op == BinaryOp::Greater ||
        op == BinaryOp::GreaterEqual ||
        op == BinaryOp::Less ||
        op == BinaryOp::LessEqual) {

        // numeric comparison
        if (isNumber(left) && isNumber(right)) {
            double a = toDouble(left);
            double b = toDouble(right);

            switch (op) {
                case BinaryOp::Greater: return Value(a > b);
                case BinaryOp::GreaterEqual: return Value(a >= b);
                case BinaryOp::Less: return Value(a < b);
                case BinaryOp::LessEqual: return Value(a <= b);
                default: break;
            }
        }

        // string comparison
        if (isString(left) && isString(right)) {
            const auto& a = std::get<std::string>(left.data);
            const auto& b = std::get<std::string>(right.data);

            switch (op) {
                case BinaryOp::Greater: return Value(a > b);
                case BinaryOp::GreaterEqual: return Value(a >= b);
                case BinaryOp::Less: return Value(a < b);
                case BinaryOp::LessEqual: return Value(a <= b);
                default: break;
            }
        }

        throw std::runtime_error("Invalid operands for comparison");
    }

    throw std::runtime_error("Unsupported binary operator");
}

void TypeChecker::visit(BinaryExpr& e) {
    e.left->accept(*this);
    e.right->accept(*this);

    e.type = testBinary(e.op, e.left->type, e.right->type);
}
void TypeChecker::visit(UnaryExpr& e) {}
void TypeChecker::visit(Assignment& e) {}
void TypeChecker::visit(Index& e) {}
void TypeChecker::visit(Call& e) {
    if (e.func->type->kind != TypeKind::FUNCTION) {
        throw KMYCompileError("Uncallable object.");
    }
}
void TypeChecker::visit(Get& e) {
    e.obj->type;
}
void TypeChecker::visit(FunctionExpr& e) {
    e.params;
}
void TypeChecker::visit(ThisExpr& e) {}
void TypeChecker::visit(NewExpr& e) {}

void TypeChecker::visit(Print& s) {
    s.expr->accept(*this);
}
void TypeChecker::visit(If& s) {}
void TypeChecker::visit(While& s) {}
void TypeChecker::visit(Block& s) {

}
void TypeChecker::visit(Break& s) {}
void TypeChecker::visit(Continue& s) {}

Type* typeSigToType(const TypeNodePtr& type) {
    switch (type->kind) {
        case TypeNodeKind::NAMED: {
            const NamedTypeNode& named = static_cast<NamedTypeNode&>(*type);
            if (named.name == "int") {
                return &Types::INT_TYPE;
            }
        }
        case TypeNodeKind::FUNCTION: {
            const FunctionTypeNode& named = static_cast<FunctionTypeNode&>(*type);
            named.params;
        }
    }
}

void TypeChecker::visit(Let& s) {
    if (s.expr) {
        s.expr->accept(*this);
    }

    if (s.type) {
        // Type signature exists
        if (s.expr->type != typeSigToType(s.type)) {
            KMYCompileError("Different type detected from signature.");
        }
    }
    // No type signature: infer.
    s.symbol->type = s.expr ? s.expr->type : &Types::UNINITIALISED;
}
void TypeChecker::visit(Return& s) {}
void TypeChecker::visit(Class& s) {}
void TypeChecker::visit(ExprStmt& s) {}