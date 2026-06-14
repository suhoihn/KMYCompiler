// TODO: WILL BE COMBINED WITH COMPILER.

#pragma once

#include <vector>
#include "../Core/Ast.hpp"
#include "SymbolScopeBuilder.hpp" // hack?

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

// bool captured = false;
    // int upvalueIndex = INVALID_SLOT;

class ClosureAnalyser : public Visitor {
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

    // Expressions
    void visit(Literal& e) override;
    void visit(ArrayLiteral& e) override;
    void visit(RecordLiteral& e) override;
    void visit(Variable& e) override;
    void visit(BinaryExpr& e) override;
    void visit(UnaryExpr& e) override;
    void visit(Assignment& e) override;
    void visit(Index& e) override;
    void visit(Call& e) override;
    void visit(Get& e) override;
    void visit(ScopeAccessExpr& e) override;
    void visit(FunctionExpr& e) override;
    void visit(ThisExpr& e) override;
    void visit(NewExpr& e) override;

    // Statements
    void visit(Print& s) override;
    void visit(If& s) override;
    void visit(While& s) override;
    void visit(Block& s) override;
    void visit(Break& s) override;
    void visit(Continue& s) override;
    void visit(Let& s) override;
    void visit(Return& s) override;
    void visit(Aggregate& s) override;
    void visit(TypeAlias& s) override;
    void visit(Enum& s) override;
    void visit(ExprStmt& s) override;
};
