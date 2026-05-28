#pragma once

#include <vector>
#include "../Core/Ast.hpp"

// Pass 1: Scope & Symbol Construction
// Builds scope hierarchy and registers all declarations (variables, functions, classes).
// Creates symbols and stores them in the correct scope.
// Symbols are semantic identities of a variable, function (which are variables here)

// A Symbol represents a semantic binding/declaration in the program.
//
// It is NOT just a name string.
//
// A symbol uniquely identifies a declared entity such as:
// - variable
// - parameter
// - function
// - field
// - method
// - class/record
//
// Multiple declarations with the same textual name
// produce different symbols due to lexical scoping.
//
// Example:
//
//     let x = 1;
//     {
//         let x = 2;
//     }
//
// The two `x` declarations are different symbols.
//
// Symbols store semantic/compiler information such as:
// - declared name
// - mutability
// - resolved type
// - scope ownership
// - stack/local slot
// - closure capture info
// - runtime metadata
//
// AST identifier nodes resolve to symbols during semantic analysis.

class SymbolScopeBuilder : public Visitor {
public:
    SymbolScopeBuilder(FunctionExprPtr program);
    Scope* analyse();

private:
    FunctionExprPtr program;

    Scope* globalScope;
    Scope* currScope;

    void enterScope();
    void exitScope();

    SymbolPtr declare(const std::string& name, bool isMutable);
    TypeSymbol* declareType(const std::string& name, bool isMutable);

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
    void visit(Aggregate& s) override;
    void visit(TypeAlias& s) override;
    void visit(ExprStmt& s) override;
};
