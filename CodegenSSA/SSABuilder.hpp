#pragma once

#include <vector>
#include "SSAInstr.hpp"
#include "SSAValue.hpp"
#include "../Core/visitor.hpp"
#include "../Core/Ast.hpp"

class SSABuilder : public Visitor {    
public:
    SSABuilder(FunctionExprPtr program);

    std::vector<SSAInstr> compile();

private:
    int nextId = 0;
    std::vector<SSAInstr> code;

    const FunctionExprPtr program;

    SSAValue makeValue();

    SSAValue lastValue;
    inline SSAValue getLastValue();
    inline void setLastValue(SSAValue value);

    SSAValue emit(SSAOp op, int constant);
    void emitVoid(SSAOp op, SSAValue val);
    SSAValue emit(SSAOp op, SSAValue lhs, SSAValue rhs);

    // Locals
    std::unordered_map<VarSymbol*, SSAValue> locals;

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
