// TODO: WILL BE COMBINED WITH COMPILER.

#pragma once

#include <vector>
#include "../Core/Symbol.hpp" 
#include "../Core/Scope.hpp" 
#include "../Core/Ast.hpp"
#include "../Core/FunctionContext.hpp"
#include "../Utils/DefaultVisitor.hpp"

// Pass 3: Closure Analysis + Slot Allocation
// Determines runtime storage for variables.
// Detects captured variables/upvalues and assigns local stack slots
// and closure indices required for code generation.


// Forward decls.
static int allocateLocal(FunctionContext* fnCtx, VarSymbol* sym);
static int resolveUpvalue(FunctionContext* fnCtx, VarSymbol* sym);
static bool isInsideFunction(VarSymbol* sym, Scope* functionScope);


class ClosureAnalyser : public DefaultVisitor {
public:
    ClosureAnalyser(FunctionExprPtr program);
    void analyse();

private:
    const FunctionExprPtr program;
    
    FunctionContext* currCtx;
    ResolvedVar resolveVariable(VarSymbol* sym);

    bool insideMethod = false;

    Aggregate* currentAggregate = nullptr;

    // ID given to each function expr.
    int functionId = 0;

    // Expressions
    void visit(Variable& e) override;
    void visit(FunctionExpr& e) override;
    void visit(ThisExpr& e) override;

    // Statements
    void visit(Block& s) override;
    void visit(Let& s) override;
    void visit(Aggregate& s) override;
    void visit(Enum& s) override;
};
