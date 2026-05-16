#include "SymbolScopeBuilder.hpp"

#include <vector>
#include "../Core/errorhandler.hpp"
#include "../Core/Ast.hpp"

SymbolScopeBuilder::SymbolScopeBuilder(
    std::vector<StmtPtr> program
) : program(move(program)) 
{
    // Global scope made
    currScope = std::make_unique<Scope>();
}

// Exports the scope tree.
ScopePtr SymbolScopeBuilder::analyse() {
    for (auto& stmt : program) {
        stmt->accept(*this);
    }
    return std::move(currScope);
}

void SymbolScopeBuilder::enterScope() {
    auto child = std::make_unique<Scope>();
    child->parent = std::move(currScope);
    child->depth = child->parent->depth;
    currScope = std::move(child);
}

void SymbolScopeBuilder::exitScope() {
    currScope = std::move(currScope->parent);
}

SymbolPtr SymbolScopeBuilder::declare(const std::string& name, bool isMutable) {
    // 1. Check current scope only (NOT parents)
    if (currScope->symbols.find(name) != currScope->symbols.end()) {
        return nullptr; // redeclaration in same scope.
    }

    // 2. Create symbol
    SymbolPtr sym = std::make_shared<Symbol>(
        name,
        nullptr, // Semantic type not enforced yet.
        isMutable
    );

    // 3. Store in scope
    currScope->symbols[name] = sym;

    return sym;
}
    
void SymbolScopeBuilder::visit(FunctionExpr& e) {
    enterScope();

    e.scope = currScope.get();

    for (auto& param : e.params) {
        declare(param.name, true);
    }

    e.body->accept(*this);

    exitScope();
}
    
void SymbolScopeBuilder::visit(Block& s) {
    enterScope();

    s.scope = currScope.get();
    
    for (auto& stmt : s.statements)
        stmt->accept(*this);
    
    exitScope();
}


void SymbolScopeBuilder::visit(Let& s) {
    SymbolPtr sym = declare(s.name, s.isMutable);
    if (!sym) {
        throw KMYCompileError("Redeclaration of variable \"" + s.name + "\"");
    }
    
    s.symbol = sym;

    if (s.expr) {
        s.expr->accept(*this);
    }

}

void SymbolScopeBuilder::visit(Class& s) {
    // Declare class name in outer scope
    SymbolPtr sym = declare(s.name, false);

    if (!sym) {
        throw KMYCompileError(
            "Redeclaration of class \"" + s.name + "\""
        );
    }

    s.symbol = sym;

    // Create class scope
    enterScope();

    s.scope = currScope.get();

    // Visit fields
    for (auto& member : s.fieldMembers) {
        if (member.initialiser)
            member.initialiser->accept(*this);
    }

    // Visit methods
    for (auto& member : s.methodMembers) {
        member.methodExpr->accept(*this);
    }

    exitScope();
}