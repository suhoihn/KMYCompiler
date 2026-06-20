#pragma once

#include "../Core/Symbol.hpp"
#include "../Core/Scope.hpp"

VarSymbol* declareVar(Scope* currScope, const std::string& name, bool isMutable) {
    // 1. Check current scope only (NOT parents)
    if (currScope->values.find(name) != currScope->values.end()) {
        // Redeclaration in same scope.
        return nullptr; 
    }

    // 2. Create symbol
    VarSymbol* sym = new VarSymbol(name, isMutable);

    // 3. Store in scope
    currScope->values[name] = sym;

    return sym;
}