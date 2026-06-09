#include "ClosureAnalyser.hpp"
#include "../Core/errorhandler.hpp"
#include "../Utils/SymbolPrinter.hpp"

ClosureAnalyser::ClosureAnalyser(FunctionExprPtr program)
    : program(program)
{
    currCtx = new FunctionContext; // global context
}


void ClosureAnalyser::analyse() {
    program->accept(*this);
}

static int resolveUpvalue(FunctionContext* fnCtx, SymbolPtr sym) {
    if (!sym) {
        throw KMYCompileError("Symbol not resolved? this is stupid.");
    }
    // Returns the slot of upvalue where name belongs in fnCtx's context.
    // std::cout << "[DEBUG] Resolving upvalue for symbol: " << sym->name << std::endl;

    auto parentCtx = fnCtx->parent;
    if (!parentCtx) {
        // Parent doesn't exist (woah!)
        return -1;
    }

    // Check whether name is already resolved in current context.
    auto itMap = fnCtx->upvalueMap.find(sym);
    if (itMap != fnCtx->upvalueMap.end()) {
        return itMap->second;
    }
    
    // Firstly, is name in parent's local?
    auto it = parentCtx->localMap.find(sym);
    if (it != parentCtx->localMap.end()) {
        // Found in parent's local.
        // Slot of name in parent's locals.
        int parentLocalSlot = it->second;

        // Update current fnCtx's upvalue map.
        int slot = fnCtx->upvalues.size();
        fnCtx->upvalueMap[sym] = slot;
        
        // Also the upvalues vector.
        fnCtx->upvalues.push_back(
            UpvalueInfo {
                true,               // it is in parent's local
                parentLocalSlot,    // return slot of parent's local.
            }
        );

        // Also we tell the local in locals that it is captured.
        // So that VM knows to keep it on heap when destroyed.
        parentCtx->locals[it->second].captured = true;
        return slot;
    }

    // Not found in parent's local.
    // Let's go up and add name in ancestor's upvalues vector.
    // Slot in parent's upvalue vector.
    int parentUpvalueSlot = resolveUpvalue(parentCtx, sym);
    if (parentUpvalueSlot != -1) {
        // parentCtx->upvalues[parentSlot] stores where name is stored.

        int slot = fnCtx->upvalues.size();
        fnCtx->upvalueMap[sym] = slot;
        fnCtx->upvalues.push_back(
            UpvalueInfo {
                false,              // it is not in parent's local.
                parentUpvalueSlot,  // It is a slot in parent's upvalue (so fnCtx->upvalue[slot] = parentUpvalueSlot) 
                                    // You need to recursively walk in parent's upvalues, not locals.
            }
        );
        return slot;
    }

    // Cannot find name anywhere. Allocation failed.
    return -1;
}


ResolvedVar ClosureAnalyser::resolveVariable(SymbolPtr sym) {
    if (!sym) {
        throw KMYCompileError("Symbol not resolved? this is stupid.");
    }

    // 1. Local
    auto& localMap = currCtx->localMap;
    auto it = localMap.find(sym);
    if (it != localMap.end()) {
        // Found in local. Easy.
        std::cout << "Easy!\n";
        return ResolvedVar {
            ResolvedVar::Kind::LOCAL,
            it->second,
        };
    }
    
    // 2. Upvalue
    int up = resolveUpvalue(currCtx, sym);
    if (up != -1) {
        std::cout << "Captured upvalue! " << sym->name << "\n";
        std::cout << "Upvalue slot: " << up << "\n";

        return ResolvedVar {
            ResolvedVar::Kind::UPVALUE,
            up
        };
    }

    throw KMYCompileError("Undefined variable.");
}

int ClosureAnalyser::allocateLocal(SymbolPtr sym) {
    if (!sym) {
        throw KMYCompileError("Symbol not resolved? this is stupid.");
    }

    int slot = currCtx->nextSlot++;
    currCtx->localMap[sym] = slot;

    // For debug.
    sym->slot = slot;

    std::cout << sym << std::endl;
    std::cout << "Allocating local variable: " << sym->name << " at slot " << slot << std::endl;
    currCtx->locals.push_back(Local{
        sym,
        slot,
        currCtx->scopeDepth,
        false, // initially not captured.
    });

    return slot;
}


void ClosureAnalyser::visit(Literal&) {}
void ClosureAnalyser::visit(ArrayLiteral& e) {
    for (auto& elem : e.elements)
        elem->accept(*this);
}
void ClosureAnalyser::visit(RecordLiteral& e) {
    for (auto& [_, value] : e.fields)
        value->accept(*this);
}

void ClosureAnalyser::visit(Variable& e) {
    std::cout << "haha i got ya\n";
    std::cout << "var node=" << &e << '\n';
    std::cout << "symbol=" << e.symbol.get() << '\n';
    std::cout << "resolved= " << e.resolved << "\n";
    std::cout << "resolution=" << (int)e.resolution.kind << "\n";
    std::cout << typeToString(e.type) << "\n";
    std::cout << "name=" << e.name << "\n"; 

    if (e.symbol)
        std::cout << "symbol name=" << e.symbol->name << '\n';

    if (e.symbol->nativeFnPtr) {
        std::cout << "Hi native!\n";
        e.resolved = true;
        // TODO: Really feels like a hack...
        e.resolution = ResolvedVar{ResolvedVar::Kind::GLOBAL, e.symbol->globalSlot};
        return;
    }
        
    if (!e.resolved) {
        e.resolved = true;
        std::cout << "Lets resolve symbol\n";
        e.resolution = resolveVariable(e.symbol);
    }
}

void ClosureAnalyser::visit(BinaryExpr& e) {
    e.left->accept(*this);
    e.right->accept(*this);
}
void ClosureAnalyser::visit(UnaryExpr& e) {
    e.operand->accept(*this);
}
void ClosureAnalyser::visit(Assignment& e) {
    std::cout << "crash now.\n";
    e.left->accept(*this);
    e.right->accept(*this);
}
void ClosureAnalyser::visit(Index& e) {
    e.obj->accept(*this);
    e.index->accept(*this);
}
void ClosureAnalyser::visit(Call& e) {
    std::cout << "Is call the culprit?\n";
    e.func->accept(*this);

    std::cout << "not the func\n";
    for (auto& arg : e.args) {
        std::cout << "then which arg?\n";
        arg->accept(*this);
    }
    
    std::cout << "no\n";
}
void ClosureAnalyser::visit(Get& e) {
    e.obj->accept(*this);
}

void ClosureAnalyser::visit(FunctionExpr& e) {
    std::cout << "Entering function: " << e.params.size() << " params\n";
    
    // 1. Create new context
    FunctionContext* fnCtx = new FunctionContext;
    fnCtx->parent = currCtx;
    currCtx = fnCtx;

    // 2. Parameters
    for (auto& param : e.params) {
        if (!param.symbol) {
            throw KMYCompileError("Symbol not resolved in param? this is stupid.");
        }
        allocateLocal(param.symbol);
    }

    std::cout << "Local alloc done. body check." << std::endl;
    // 3. Body
    e.body->accept(*this);
    std::cout << "body check done" << std::endl;

    currCtx = fnCtx->parent;
    
    std::cout << "framesize: " << fnCtx->nextSlot << std::endl;
    std::cout << "upvalue cnt: " << fnCtx->upvalues.size() << std::endl;

    e.upvalues = fnCtx->upvalues;
    e.frameSize = fnCtx->nextSlot;

    delete fnCtx;
}

void ClosureAnalyser::visit(ThisExpr&) {}
void ClosureAnalyser::visit(NewExpr& e) {
    for (auto& arg : e.args)
        arg->accept(*this);
}


// Statements
void ClosureAnalyser::visit(Print& s) {
    std::cout << "print? ru culprit\n";
    s.expr->accept(*this);
    std::cout << "no haha\n";
}
void ClosureAnalyser::visit(If& s) {
    s.condition->accept(*this);

    s.thenbranch->accept(*this);

    if (s.elsebranch)
        s.elsebranch->accept(*this);
}
void ClosureAnalyser::visit(While& s) {
    s.condition->accept(*this);
    s.body->accept(*this);
}
void ClosureAnalyser::visit(Block& s) {
    currCtx->scopeDepth++;   // ENTER scope

    for (auto& stmt : s.statements) {
        stmt->accept(*this);
    }

    currCtx->scopeDepth--;   // EXIT scope

    // remove locals declared in this scope
    while (!currCtx->locals.empty() && currCtx->locals.back().scopeDepth > currCtx->scopeDepth) {
        auto& local = currCtx->locals.back();

        if (local.captured) {
            //emit(Opcode::CLOSE_UPVALUE, local.slot);
        }

        currCtx->locals.pop_back();
    }
}

void ClosureAnalyser::visit(Break&) {}
void ClosureAnalyser::visit(Continue&) {}

void ClosureAnalyser::visit(Let& s) {
    if (s.expr)
        s.expr->accept(*this);
    
    int slot = allocateLocal(s.symbol);
    s.symbol->slot = slot;
}

void ClosureAnalyser::visit(Return& s) {
    if (s.expr)
        s.expr->accept(*this);
}

void ClosureAnalyser::visit(Aggregate& s) {
    std::cout << "Entering class: \n";

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
    std::cout << "member check done" << std::endl;
}

void ClosureAnalyser::visit(TypeAlias& s) {}
void ClosureAnalyser::visit(ExprStmt& s) {
    s.expr->accept(*this);
}