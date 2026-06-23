#pragma once

#include <vector>
#include "BasicBlock.hpp"
#include "IRInstr.hpp"
#include "IRValue.hpp"
#include "IRFunction.hpp"
#include "../Core/visitor.hpp"
#include "../Core/Ast.hpp"

struct LoopContext {
    BasicBlock* continueTarget;
    BasicBlock* breakTarget;
};

// This is temporary! Only lived in this file.
struct IRCodegenFnCtx {
    IRCodegenFnCtx* parent = nullptr;
    int nextId = 0;

    // Locals
    IRValue lastValue;
    std::unordered_map<VarSymbol*, IRValue> locals;
    
    // Upvalues
    std::unordered_map<VarSymbol*, IRValue> upvalues;
    
    // Loops
    std::vector<LoopContext> loopStack;
    
    // Blocks
    int nextBlockId = 0;
    BasicBlock* currBlock = nullptr;

    // Env
    std::optional<IRValue> env; // The env this function is making for children (if any)
    std::optional<IRValue> incomingEnv; // The env this function is taking from parent (if any)


    FunctionContext* fnCtx;
};

class IRBuilder : public Visitor {    
public:
    IRBuilder(FunctionExprPtr program);

    std::vector<IRFunction*> compile();

private:
    // Readonly AST
    const FunctionExprPtr program;


    // Current function context
    IRCodegenFnCtx* currCtx = nullptr;

    // Blocks
    BasicBlock* makeBlock();
    void connectBlock(BasicBlock* from, BasicBlock* to);

    // Code
    IRValue makeValue(Type* type);

    inline IRValue getLastValue();
    inline void setLastValue(IRValue value);

    // Functions
    IRFunction* currFunc = nullptr;
    std::vector<IRFunction*> functions;

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
    void visit(ExprStmt& s) override;
    void visit(Enum& s) override;    
};
