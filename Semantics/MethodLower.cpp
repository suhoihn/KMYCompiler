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
    for (auto& arg : e.args)
    arg->accept(*this);
    
    // Lowering obj.f(...) form to f(obj, ...)
    if (e.func->kind == ExprKind::Get) {
        auto callee = std::static_pointer_cast<Get>(e.func);
        // change maybe. hard to track.
        if (callee->resolvedMethod) {
            // Instance object.
            auto obj = callee->obj;
            e.args.insert(e.args.begin(), obj);
        }
    }
}

void MethodLower::visit(Get& e) {
    e.obj->accept(*this);
}

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
        Parameter thisParam(nullptr, "this", false, false, false);
        // Create "this" symbol
        SymbolPtr thisSym = std::make_shared<Symbol>("implicit_this", false);

        // Store in scope
        s.scope->symbols["implicit_this"] = thisSym;

        thisParam.symbol = thisSym;

        auto& paramVec = member.methodExpr->params;
        paramVec.insert(paramVec.begin(), thisParam);
        member.methodExpr->accept(*this);
    }
}

void MethodLower::visit(TypeAlias& s) {}
void MethodLower::visit(ExprStmt& s) {
    s.expr->accept(*this);
}