#pragma once

#include "Symbol.hpp"

struct Local {
    VarSymbol* sym = nullptr;
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
