// TODO: WILL BE COMBINED WITH COMPILER.

#pragma once

#include <vector>
#include "../Core/Ast.hpp"
#include "../Utils/DefaultVisitor.hpp"

// Pass 3: Closure Analysis + Slot Allocation
// Determines runtime storage for variables.
// Detects captured variables/upvalues and assigns local stack slots
// and closure indices required for code generation.

struct FunctionContext {
    FunctionContext* parent;
    
    std::vector<Local> locals;
    std::unordered_map<VarSymbol*, int> localMap;
    
    // Constants are accessed via chunk.constants
    // This is handled in codegen part.
    // std::unordered_map<ConstValue, int> constantMap;

    int scopeDepth = 0;
    int nextSlot = 0;

    std::unordered_map<VarSymbol*, int> upvalueMap;
    std::vector<UpvalueInfo> upvalues;
};

class ClosureAnalyser : public DefaultVisitor {
public:
    ClosureAnalyser(FunctionExprPtr program);
    void analyse();

private:
    const FunctionExprPtr program;
    
    FunctionContext* currCtx;
    ResolvedVar resolveVariable(VarSymbol* sym);
    int allocateLocal(VarSymbol* sym);

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
