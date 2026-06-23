#pragma once

#include <vector>
#include "../Core/Ast.hpp"
#include "../Utils/DefaultVisitor.hpp"

// Pass 2: Declaration Type Resolver 
// For typealias, function decls, aggregate
// Since their symbols are available but their types aren't, pre-define the types in this pass
// For future me: Please be well aware what this pass does.
// Also, invariant is that all typesymbol's type are populated after this pass.

class DeclTypeResolver : public DefaultVisitor {
public:
    DeclTypeResolver(FunctionExprPtr program, Scope* globalScope);
    void resolve();

private:
    Scope* globalScope = nullptr;
    Scope* currScope = nullptr;
    const FunctionExprPtr program;

    VarSymbol* currentThis = nullptr;
    InstanceType* currentAggregate = nullptr;

    // Expressions
    void visit(ThisExpr& e) override;
    void visit(FunctionExpr& e) override;

    // Statements
    void visit(Block& s) override;
    void visit(Let& s) override;
    void visit(Aggregate& s) override;
    void visit(TypeAlias& s) override;
    void visit(Enum& s) override;
};
