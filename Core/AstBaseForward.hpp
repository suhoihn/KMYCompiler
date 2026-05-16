#pragma once
#include <memory>
#include "type.hpp"

struct ASTNode {
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

using ExprPtr = std::shared_ptr<struct BaseExpr>;
using StmtPtr = std::shared_ptr<struct BaseStmt>;

constexpr int INVALID_SLOT = -1;

// The semantic identity of a variable.
struct Symbol {
    std::string name;
    bool isMutable;
    Type* type = nullptr;

    // Runtime storage info.
    int slot = INVALID_SLOT;
    bool captured = false;
    int upvalueIndex = INVALID_SLOT;
};

using SymbolPtr = std::shared_ptr<Symbol>;


struct Local {
    SymbolPtr sym = nullptr;
    int slot = INVALID_SLOT;  // Stack slot index. For future, it can also mean virtual reg index.
    int scopeDepth = 0; // This is relative scope depth from current fnCtx.
    bool captured = false;
};

struct UpvalueInfo {
    bool isLocal;
    int index; // Slot index in parent locals OR parent upvalues
};

struct ResolvedVar {
    enum Kind {
        LOCAL, UPVALUE, GLOBAL
    } kind;

    int index;
};

struct Parameter {
    TypeNodePtr type;
    std::string name;
    bool isVariadic = false;
    bool isMutable = true; // Mutable by default
    bool defaultExists = false;
    ExprPtr defaultValue;
    SymbolPtr symbol = nullptr;

    Parameter(TypeNodePtr type, std::string name, bool isVariadic, bool isMutable, bool defaultExists, ExprPtr defaultValue=nullptr)
        : 
        type(std::move(type)),
        name(move(name)),
        isVariadic(isVariadic),
        isMutable(isMutable), 
        defaultExists(defaultExists),
        defaultValue(move(defaultValue)) {}
};