#pragma once

#include <vector>
#include "../Core/Ast.hpp"
#include "../Utils/DefaultVisitor.hpp"

// Pass 3: Method lowerer
// Lowers all function calls in form of 
//  obj.f(...) -> f(obj, ...)
// NOTE: The method's parameters already have implicit_this inserted in pass 1. 


class MethodLower : public DefaultVisitor {
public:
    MethodLower(FunctionExprPtr program);
    void lower();

private:
    const FunctionExprPtr program;

    // Expressions
    void visit(Call& e) override;
};
