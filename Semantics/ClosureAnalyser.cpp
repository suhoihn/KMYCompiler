#include "ClosureAnalyser.hpp"

#include <iostream>
#include "../Core/errorhandler.hpp"
#include "../Utils/SymbolPrinter.hpp"
#include "../Utils/utils.hpp"
#include <assert.h>

ClosureAnalyser::ClosureAnalyser(FunctionExprPtr program)
    : program(program)
{}

void ClosureAnalyser::analyse() {
    program->accept(*this);
}

static int allocateLocal(FunctionContext* fnCtx, VarSymbol* sym) {
    assert(fnCtx);
    assert(sym);

    // allocateLocal should not be called for symbol already allocated.
    assert(fnCtx->localMap.find(sym) == fnCtx->localMap.end());

    int slot = fnCtx->nextSlot++;
    fnCtx->localMap[sym] = slot;

    // For debug.
    sym->slot = slot;

    fnCtx->locals.push_back(Local{
        sym,
        slot,
        fnCtx->scopeDepth,
        false, // initially not captured.
    });

    return slot;
}

static int resolveUpvalue(FunctionContext* fnCtx, VarSymbol* sym) {
    // Returns the slot of upvalue where name belongs in fnCtx's context.
    // TODO: Example will be very helpful.

    if (!sym) {
        throw KMYCompileError("Symbol not resolved? this is stupid.");
    }
    // Returns the slot of upvalue where name belongs in fnCtx's context.

    auto parentCtx = fnCtx->parent;
    if (!parentCtx) {
        // Parent doesn't exist (woah!)
        return INVALID_SLOT;
    }

    // Check whether name is already resolved in current context.
    auto itMap = fnCtx->upvalueMap.find(sym);
    if (itMap != fnCtx->upvalueMap.end()) {
        return itMap->second;
    }
    
    // Firstly, is name in parent's local?
    
    if (isInsideFunction(sym, parentCtx->fnScope)) {
        // Found in parent's local.
        // Check whether it has its slot allocated in parent's context.
        auto it = parentCtx->localMap.find(sym);
        int parentLocalSlot;
        if (it != parentCtx->localMap.end()) {
            // Sym slot already exists in parent's locals.
            parentLocalSlot = it->second;
        } else {
            // Sym hasn't been allocated a slot.
            // Thus allocate it in parent.
            parentLocalSlot = allocateLocal(parentCtx, sym);
        }
        

        // Update current fnCtx's upvalue map.
        int slot = fnCtx->upvalues.size();
        fnCtx->upvalueMap[sym] = slot;

        
        // Also the upvalues vector.
        fnCtx->upvalues.push_back(
            UpvalueInfo {
                true,               // it is in parent's local
                parentLocalSlot,    // return slot of parent's local.
                sym                 // Storing the symbol for the captured value (for IR codegen)
            }
        );

        // Also we tell the local in locals that it is captured.
        // So that VM knows to keep it on heap when destroyed.
        parentCtx->locals[parentLocalSlot].captured = true;
        return slot;
    }

    // Not found in parent's local.
    // Let's go up and add name in ancestor's upvalues vector.
    // Slot in parent's upvalue vector.
    int parentUpvalueSlot = resolveUpvalue(parentCtx, sym);
    if (parentUpvalueSlot != INVALID_SLOT) {
        // parentCtx->upvalues[parentSlot] stores where name is stored.

        parentCtx->upvalues[parentUpvalueSlot].capturedByChildren = true;
        
        int slot = fnCtx->upvalues.size();
        fnCtx->upvalueMap[sym] = slot;
        fnCtx->upvalues.push_back(
            UpvalueInfo {
                false,              // it is not in parent's local.
                parentUpvalueSlot,  // It is a slot in parent's upvalue (so fnCtx->upvalue[slot] = parentUpvalueSlot) 
                                    // You need to recursively walk in parent's upvalues, not locals.
                sym
            }
        );
        return slot;
    }

    // Cannot find name anywhere. Allocation failed.
    return INVALID_SLOT;
}


static bool isInsideFunction(VarSymbol* sym, Scope* functionScope) {
    Scope* s = sym->definingScope;

    while (s) {
        if (s == functionScope) return true;
        s = s->parent;
    }

    return false;
}

ResolvedVar ClosureAnalyser::resolveVariable(VarSymbol* sym) {
    if (!sym) {
        throw KMYCompileError("Symbol not resolved? this is stupid.");
    }

    // 1. Local
    if (isInsideFunction(sym, currCtx->fnScope)) {
        // Found in function's local.
        // Check whether it has its slot allocated.
        auto it = currCtx->localMap.find(sym);
        if (it != currCtx->localMap.end()) {
            // Sym slot already exists
            return ResolvedVar {
                ResolvedVar::Kind::LOCAL,
                it->second,
            };
        }
        
        // Sym hasn't been allocated a slot.
        int slot = allocateLocal(currCtx, sym);

        return ResolvedVar {
            ResolvedVar::Kind::LOCAL,
            slot,
        };

    }
    
    // 2. Upvalue
    int up = resolveUpvalue(currCtx, sym);
    if (up != INVALID_SLOT) {

        return ResolvedVar {
            ResolvedVar::Kind::UPVALUE,
            up
        };
    }

    throw KMYCompileError("Undefined variable.");
}

void ClosureAnalyser::visit(Variable& e) {
    if (e.symbol->nativeFnPtr) {
        e.resolved = true;
        // TODO: Really feels like a hack...
        e.resolution = ResolvedVar{ResolvedVar::Kind::GLOBAL, e.symbol->globalSlot};
        return;
    }
        
    if (!e.resolved) {
        e.resolved = true;
        e.resolution = resolveVariable(e.symbol);
    }
}

void ClosureAnalyser::visit(FunctionExpr& e) {
    e.functionId = functionId++;
    
    // 1. Create new context
    FunctionContext* fnCtx = new FunctionContext;
    fnCtx->parent = currCtx;
    fnCtx->fnScope = e.scope;
    currCtx = fnCtx;
    

    // 2. Parameters
    for (auto& param : e.params) {
        if (!param.symbol) {
            throw KMYCompileError("Symbol not resolved in param? this is stupid.");
        }
        // No dedup check since it is guaranteed to be new.
        // ... right?   
        allocateLocal(currCtx, param.symbol);
    }

    // 3. Body
    e.body->accept(*this);

    int envSlot = 0;

    // Env genreation.
    // Very simple principle: if children closures capture any of my locals or upvalues,
    // Give it an envSlot.
    // captured locals
    for (auto& local : currCtx->locals) {
        if (local.captured) {
            currCtx->envMap[local.sym] = envSlot++;
        }
    }

    // re-exported upvalues
    for (auto& up : currCtx->upvalues) {
        if (up.capturedByChildren) {
            currCtx->envMap[up.symbol] = envSlot++;
        }
    }

    currCtx->envSize = envSlot;

    currCtx = fnCtx->parent;
    

    e.functionContext = fnCtx;
    
}

void ClosureAnalyser::visit(ThisExpr& e) {
    // Note that "this" can be captured too!
    if (!e.resolved) {
        e.resolved = true;
        e.resolution = resolveVariable(e.symbol);
    }
}

// Statements
void ClosureAnalyser::visit(Block& s) {
    currCtx->scopeDepth++;   // ENTER scope

    for (auto& stmt : s.statements) {
        stmt->accept(*this);
    }

    currCtx->scopeDepth--;   // EXIT scope

    // remove locals declared in this scope
    return;
    // Why?
    while (!currCtx->locals.empty() && currCtx->locals.back().scopeDepth > currCtx->scopeDepth) {
        auto& local = currCtx->locals.back();

        if (local.captured) {
            //emit(Opcode::CLOSE_UPVALUE, local.slot);
        }

        currCtx->locals.pop_back();
    }
}

void ClosureAnalyser::visit(Let& s) {
    if (s.expr)
        s.expr->accept(*this);
    
    if (s.isFunctionDecl) {
        // H A C K?
        currCtx->funcDecls.push_back({
            s.symbol,
            std::static_pointer_cast<FunctionExpr>(s.expr)
        });
    }

    // Hmm, allocateLocal is used all over I think. Some places I think unnecessary.
    int slot;
    auto it = currCtx->localMap.find(s.symbol);
    if (it != currCtx->localMap.end()) {
        // Already allocated.
        slot = it->second;
    } else {
        slot = allocateLocal(currCtx, s.symbol);
    }
    s.symbol->slot = slot;
}

void ClosureAnalyser::visit(Aggregate& s) {

    int offset = 0;
    for (auto& field : s.fieldMembers) {
        offset++;
        if (field.initialiser) {
            field.initialiser->accept(*this);
        }
    }

    for (auto& method : s.methodMembers) {
        method.methodExpr->accept(*this);
    }

    for (auto& method : s.constructorMembers) {
        method.initFuncExpr->accept(*this);
    }

    s.fieldInitFunc->accept(*this);

    s.fieldCount = offset;
}

void ClosureAnalyser::visit(Enum& s) {
    EnumType* enumType = static_cast<EnumType*>(s.typeSymbol->type);

    int num = 0;
    for (auto& str : s.variants) {
        enumType->variantMap[str] = num;
    }
}
