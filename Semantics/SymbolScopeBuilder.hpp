#pragma once

#include <vector>
#include "../Core/Ast.hpp"

using ScopePtr = std::unique_ptr<struct Scope>;

struct Scope {
    ScopePtr parent = nullptr;
    std::unordered_map<std::string, SymbolPtr> symbols; // Symbols in this scope.
    int depth = 0;

    Scope() = default;

    Scope(ScopePtr parent, int depth)
        : parent(std::move(parent)), depth(depth) {}
};


// Pass 1: Scope & Symbol Construction
// Builds scope hierarchy and registers all declarations (variables, functions, classes).
// Creates symbols and stores them in the correct scope.
class SymbolScopeBuilder : public Visitor {
public:
    SymbolScopeBuilder(std::vector<StmtPtr> program);
    ScopePtr analyse();

private:
    const std::vector<StmtPtr>& program;

    ScopePtr currScope;

    void enterScope();
    void exitScope();

    SymbolPtr declare(const std::string& name, bool isMutable);

    // Expressions are skipped entirely since no new symbol is introduced here.
    // except functionExpr
    void visit(FunctionExpr& e) override;

    // Statements with only declaration stages are visited.
    void visit(Block& s) override;
    void visit(Let& s) override;
    void visit(Class& s) override;
};
