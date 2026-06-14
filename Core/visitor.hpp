#pragma once

// All AST nodes forward declared.

class Visitor {
public:
    // Expressions
    virtual void visit(struct Literal& e) = 0;
    virtual void visit(struct ArrayLiteral& e) = 0;
    virtual void visit(struct RecordLiteral& e) = 0;
    virtual void visit(struct Variable& e) = 0;
    virtual void visit(struct BinaryExpr& e) = 0;
    virtual void visit(struct UnaryExpr& e) = 0;
    virtual void visit(struct Assignment& e) = 0;
    virtual void visit(struct Index& e) = 0;
    virtual void visit(struct Call& e) = 0;
    virtual void visit(struct Get& e) = 0;
    virtual void visit(struct ScopeAccessExpr& e) = 0;
    virtual void visit(struct FunctionExpr& e) = 0;
    virtual void visit(struct ThisExpr& e) = 0;
    virtual void visit(struct NewExpr& e) = 0;

    // Statements
    virtual void visit(struct Print& s) = 0;
    virtual void visit(struct If& s) = 0;
    virtual void visit(struct While& s) = 0;
    virtual void visit(struct Block& s) = 0;
    virtual void visit(struct Break& s) = 0;
    virtual void visit(struct Continue& s) = 0;
    virtual void visit(struct Let& s) = 0;
    virtual void visit(struct Return& s) = 0;
    virtual void visit(struct Aggregate& s) = 0;
    virtual void visit(struct TypeAlias& s) = 0;
    virtual void visit(struct Enum& s) = 0;
    virtual void visit(struct ExprStmt& s) = 0;

    virtual ~Visitor() = default;
};