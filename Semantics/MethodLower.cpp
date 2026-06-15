#include "MethodLower.hpp"

#include <vector>
#include "../Core/errorhandler.hpp"
#include "../Core/Ast.hpp"
#include <unordered_set>

MethodLower::MethodLower(
    FunctionExprPtr program
) : program(program) {}

void MethodLower::lower() {
    program->accept(*this);
}

void MethodLower::visit(Literal&) {}
void MethodLower::visit(Variable&) {}
void MethodLower::visit(ArrayLiteral& e) {
    for (auto& elem : e.elements)
        elem->accept(*this);
}
void MethodLower::visit(RecordLiteral& e) {
    for (auto& [_, value] : e.fields) {
        value->accept(*this);
    }
}
void MethodLower::visit(BinaryExpr& e) {
    e.left->accept(*this);
    e.right->accept(*this);
}
void MethodLower::visit(UnaryExpr& e) {
    e.operand->accept(*this);
}
void MethodLower::visit(Assignment& e) {
    e.left->accept(*this);
    e.right->accept(*this);
}
void MethodLower::visit(Index& e) {
    e.obj->accept(*this);
    e.index->accept(*this);
}

void MethodLower::visit(Call& e) {
    // Lower children first
    e.func->accept(*this);
    for (auto& arg : e.args) {
        arg->accept(*this);
    }
    
    // Lowering obj.f(...) form to f(obj, ...)
    // NOTE: f must be a method, not a function!
    if (e.func->kind == ExprKind::Get) {
        auto callee = std::static_pointer_cast<Get>(e.func);
        // change maybe. hard to track.
        if (callee->resolvedMethod) {
            // Instance object.
            auto obj = callee->obj;
            e.args.push_back(obj); // REALLY DONT KNOW WHY THIS WORKS LOL
            //e.args.insert(e.args.begin(), obj);
        }
        // HACK!!!!
    }
}

void MethodLower::visit(Get& e) {
    e.obj->accept(*this);
}
void MethodLower::visit(ScopeAccessExpr& e) {}
void MethodLower::visit(ThisExpr&) {}
void MethodLower::visit(NewExpr& e) {
    for (auto& arg : e.args)
        arg->accept(*this);
}

void MethodLower::visit(FunctionExpr& e) {
    for (auto& param : e.params) {
        if (param.defaultExists) {
            param.defaultValue->accept(*this);
        }
    }

    e.body->accept(*this);
}

// ======================================================
// Statements
// ======================================================

void MethodLower::visit(Block& s) {
    for (auto& stmt : s.statements)
        stmt->accept(*this);
}
void MethodLower::visit(Print& s) {
    s.expr->accept(*this);
}
void MethodLower::visit(If& s) {
    s.condition->accept(*this);

    s.thenbranch->accept(*this);

    if (s.elsebranch)
        s.elsebranch->accept(*this);
}
void MethodLower::visit(While& s) {
    s.condition->accept(*this);
    s.body->accept(*this);
}
void MethodLower::visit(Break&) {}
void MethodLower::visit(Continue&) {}
void MethodLower::visit(Return& s) {
    if (s.expr)
        s.expr->accept(*this);
}
void MethodLower::visit(Let& s) {
    if (s.expr)
        s.expr->accept(*this);
}

void MethodLower::visit(Aggregate& s) {
    // Fields
    for (auto& member : s.fieldMembers) {
        if (member.initialiser)
            member.initialiser->accept(*this);
    }

    // Methods
    for (auto& member : s.methodMembers) {
        member.methodExpr->accept(*this);
    }

    // Constructors
    for (auto& member : s.constructorMembers) {
        member.initFuncExpr->accept(*this);
    }

    // Field initialiser
    s.fieldInitFunc->accept(*this);
}

void MethodLower::visit(TypeAlias& s) {}
void MethodLower::visit(Enum& s) {}
void MethodLower::visit(ExprStmt& s) {
    s.expr->accept(*this);
}