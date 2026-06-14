#pragma once

#include <string>
#include "AstBaseForward.hpp"

inline constexpr int INVALID_SLOT = -1;

// Forward decl.
struct Type;

enum struct SymbolKind {
    VARIABLE, // "let x", "fun f" build them (+ aggregate member decls).
    TYPE // "record/class X", "typealias IntArr", "enum E" build them.
};

// Symbol is the semantic identity of a variable/function/type.
struct Symbol {
    std::string name;
    SymbolKind kind;

    Symbol(const std::string& name, SymbolKind kind)
        : name(name), kind(kind) {}

    virtual ~Symbol() = default;
};


struct VarSymbol : Symbol {
    bool isMutable;
    Type* type = nullptr;

    // Fields below are ONLY for debug. Each pass (or phase) has its own storage of the same info.
    
    // Runtime storage info for locals and upvalues (if used)
    int slot = INVALID_SLOT;
    
    // Aggregate Info (if used)
    int fieldOffset = INVALID_SLOT;
    int methodIdx = INVALID_SLOT; // For vtables too...?
    
    // Function Info (if used)
    int funcProtoIdx = INVALID_SLOT;
    
    // Native function info (if used)
    NativeFnPtr nativeFnPtr = nullptr;
    int globalSlot = INVALID_SLOT;

    VarSymbol(const std::string& name, bool isMutable)
        : Symbol(name, SymbolKind::VARIABLE), isMutable(isMutable) {}
};


struct TypeSymbol : Symbol {
    bool isMutable;
    // TODO: Move this field to base symbol?
    Type* type = nullptr;

    TypeSymbol(const std::string& name, bool isMutable)
        : Symbol(name, SymbolKind::TYPE), isMutable(isMutable) {}
};