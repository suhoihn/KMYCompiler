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
    std::unordered_map<SymbolPtr, int> localMap;
    
    // Constants are accessed via chunk.constants
    // This is handled in codegen part.
    // std::unordered_map<ConstValue, int> constantMap;

    int scopeDepth = 0;
    int nextSlot = 0;

    std::unordered_map<SymbolPtr, int> upvalueMap;
    std::vector<UpvalueInfo> upvalues;
};


class ClosureAnalyser : public Visitor {
public:
    ClosureAnalyser(std::vector<StmtPtr> program);
    void analyse();

private:
    const std::vector<StmtPtr>& program;
    
    FunctionContext* currCtx = nullptr; // global
    ResolvedVar resolveVariable(SymbolPtr sym);
    int allocateLocal(SymbolPtr sym);

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
    void visit(Class& s) override;
    void visit(ExprStmt& s) override;
};
