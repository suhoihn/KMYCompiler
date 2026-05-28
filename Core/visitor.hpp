#pragma once

// Forward declarations of all AST node types
struct Literal;
struct ArrayLiteral;
struct RecordLiteral;
struct Variable;
struct BinaryExpr;
struct UnaryExpr;
struct Assignment;
struct Index;
struct Call;
struct Get;
struct FunctionExpr;
struct ThisExpr;
struct NewExpr;

// Statements
struct Print;
struct If;
struct While;
struct Block;
struct Break;
struct Continue;
struct Let;
struct Return;
struct Aggregate;
struct TypeAlias;
struct ExprStmt;

// Visitor interface
class Visitor {
public:
    // Expressions
    virtual void visit(Literal& e) {}
    virtual void visit(ArrayLiteral& e) {}
    virtual void visit(RecordLiteral& e) {}
    virtual void visit(Variable& e) {}
    virtual void visit(BinaryExpr& e) {}
    virtual void visit(UnaryExpr& e) {}
    virtual void visit(Assignment& e) {}
    virtual void visit(Index& e) {}
    virtual void visit(Call& e) {}
    virtual void visit(Get& e) {}
    virtual void visit(FunctionExpr& e) {}
    virtual void visit(ThisExpr& e) {}
    virtual void visit(NewExpr& e) {}

    // Statements
    virtual void visit(Print& s) {}
    virtual void visit(If& s) {}
    virtual void visit(While& s) {}
    virtual void visit(Block& s) {}
    virtual void visit(Break& s) {}
    virtual void visit(Continue& s) {}
    virtual void visit(Let& s) {}
    virtual void visit(Return& s) {}
    virtual void visit(Aggregate& s) {}
    virtual void visit(TypeAlias& s) {}
    virtual void visit(ExprStmt& s) {}

    virtual ~Visitor() = default;
};