#pragma once

#include <vector>
#include "../Core/Ast.hpp"
#include "SymbolScopeBuilder.hpp" // hack?

// Pass 2: Name Resolution
// Resolves identifier usages to their declared symbols using the scope tree built in Pass 1.
// Binds variables/functions/classes to Symbol objects and reports undefined references.
// Also checks valid loop break/continue, return statements.
// Also does some type building.

class Resolver : public Visitor {
public:
    Resolver(FunctionExprPtr program, Scope* globalScope);
    void resolve();

private:
    const FunctionExprPtr program;
    Scope* globalScope;
    Scope* currScope;

    Type* expectedType = nullptr;

    int loopDepth = 0;

    Type* typeSigToType(const TypeNodePtr& type);
    TypeSymbol* resolveTypeSymbol(const std::string& name);
    SymbolPtr resolveSymbol(const std::string& name);

    AggregateType* currentAggregate = nullptr;
    SymbolPtr currentThis = nullptr; 

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
    void visit(Aggregate& s) override;
    void visit(TypeAlias& s) override;
    void visit(ExprStmt& s) override;
};
