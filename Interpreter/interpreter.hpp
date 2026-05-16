#pragma once

#include <vector>
#include "../Core/value.hpp"
#include "../Core/visitor.hpp"
#include "../Core/Ast.hpp"
#include "env.hpp"

class Interpreter : public Visitor {
public:
    Env* global;
    Env* env; // Current scope env.

    Value result;

    Interpreter();
    
    void initBuiltins();

    void interpret(const std::vector<StmtPtr>& stmts);

    void printEnv() const;

    Value Interpreter::getResult(ExprPtr expr);

    // Expressions
    virtual void visit(Literal& e) override;
    virtual void visit(ArrayLiteral& e) override;
    virtual void visit(RecordLiteral& e) override;
    virtual void visit(Variable& e) override;
    virtual void visit(BinaryExpr& e) override;
    virtual void visit(UnaryExpr& e) override;
    virtual void visit(Assignment& e) override;
    virtual void visit(Index& e) override;
    virtual void visit(Call& e) override;
    virtual void visit(Get& e) override;
    virtual void visit(FunctionExpr& e) override;
    virtual void visit(ThisExpr& e) override;
    virtual void visit(NewExpr& e) override;

    
    // Statements
    virtual void visit(Print& s) override;
    virtual void visit(If& s) override;
    virtual void visit(While& s) override;
    virtual void visit(Block& s) override;
    virtual void visit(Break& s) override;
    virtual void visit(Continue& s) override;
    virtual void visit(Let& s) override;
    virtual void visit(Return& s) override;
    virtual void visit(Class& s) override;
    virtual void visit(ExprStmt& s) override;
};