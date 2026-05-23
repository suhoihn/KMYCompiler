#pragma once

#include "../Core/Ast.hpp"
#include "../Core/visitor.hpp"

struct LValue {
    SymbolPtr symbol; // For variables.
};

class LValueResolver : public Visitor {
public:
    LValue result;
    LValue resolve(ExprPtr expr) {
        expr->accept(*this);
        return result;
    }

private:
    void visit(Variable& e) override {}
    void visit(Index& e) override {}
    void visit(Get& e) override {}
};