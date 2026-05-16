#include "compiler.hpp"

#include <stdexcept>
#include <exception>
#include "../Core/Ast.hpp"
#include "../BytecodeVM/vm.hpp"
#include "../Core/errorhandler.hpp"

void Compiler::emit(Opcode op, int operand=-1) {
    currCtx->chunk.code.push_back(Instruction{op, operand});
}

Compiler::Compiler(const std::vector<StmtPtr>& program) 
    : program(std::move(program)) {}

Chunk Compiler::compile(void) {
    for (auto& stmt : program) {
        stmt->accept(*this);
    }

    emit(Opcode::HALT);
    
    return;
}


int Compiler::addConstant(const ConstValue& v) {
    // Returns the index of the constant v in the chunk.
    auto& map = currCtx->constantMap;
    auto it = map.find(v);
    // Constant exists.
    if (it != map.end()) {
        return it->second;
    }

    int index = currCtx->chunk.constants.size();
    currCtx->chunk.constants.push_back(v);
    currCtx->constantMap[v] = index;
    return index;
}

// Expressions
void Compiler::visit(Literal& e) {
    // Literal only contains ConstValue
    ConstValue v = std::visit([](auto&& arg) -> ConstValue {
        return ConstValue(arg);
    }, e.value);

    int index = addConstant(v);
    emit(Opcode::PUSH_CONST, index);
}


void Compiler::visit(ArrayLiteral& e) {
    for (auto it = e.elements.rbegin(); it != e.elements.rend(); ++it) {
        (*it)->accept(*this);
    }
    emit(Opcode::NEW_ARRAY, e.elements.size());
}


void Compiler::visit(RecordLiteral& e) { throw std::runtime_error("NOT IMPLEMENTED"); }



void Compiler::visit(Variable& e) {
    if (!e.resolved) {
        e.resolved = true;   
        e.resolution = resolveVariable(e.name);
    }

    switch (e.resolution.kind) {
        case ResolvedVar::Kind::LOCAL:
            emit(Opcode::LOAD_LOCAL, e.resolution.index);
            break;

        case ResolvedVar::Kind::UPVALUE:
            emit(Opcode::LOAD_UPVALUE, e.resolution.index);
            break;

        // TODO(): glboals actually DE.
        case ResolvedVar::Kind::GLOBAL:
            emit(Opcode::LOAD_GLOBAL, e.resolution.index);
            break;
    }
}


static Opcode binaryOpToOpcode(BinaryOp op) {
    switch (op) {
        case BinaryOp::Plus:          return Opcode::ADD;
        case BinaryOp::Minus:         return Opcode::SUB;
        case BinaryOp::Star:          return Opcode::MUL;
        case BinaryOp::Slash:         return Opcode::DIV;
        case BinaryOp::Percent:       return Opcode::MOD;

        case BinaryOp::EqualEqual:    return Opcode::EQUAL;
        case BinaryOp::NotEqual:      return Opcode::NOT_EQUAL;
        case BinaryOp::Less:          return Opcode::LESS;
        case BinaryOp::LessEqual:     return Opcode::LESS_EQUAL;
        case BinaryOp::Greater:       return Opcode::GREATER;
        case BinaryOp::GreaterEqual:  return Opcode::GREATER_EQUAL;

        case BinaryOp::LogicalAnd:    return Opcode::LOGICAL_AND;
        case BinaryOp::LogicalOr:     return Opcode::LOGICAL_OR;

        case BinaryOp::BitAnd:        return Opcode::BIT_AND;
        case BinaryOp::BitOr:         return Opcode::BIT_OR;
        case BinaryOp::BitXor:        return Opcode::BIT_XOR;
        case BinaryOp::LShift:        return Opcode::LEFT_SHIFT;
        case BinaryOp::RShift:        return Opcode::RIGHT_SHIFT;

        default:
            throw std::runtime_error("Unknown BinaryOp");
    }
}

void Compiler::visit(BinaryExpr& e) { 
    e.left->accept(*this);
    e.right->accept(*this);
    emit( binaryOpToOpcode(e.op) );
}


static Opcode unaryOpToOpcode(UnaryOp op) {
    switch (op) {
        case UnaryOp::Plus:
            // Unary plus usually does nothing
            // Parser will ignore UnaryOp::Plus, but if it somehow came here...?
            throw std::runtime_error("Unary plus does not require an opcode");

        case UnaryOp::Minus:
            return Opcode::NEG;

        case UnaryOp::LogicalNot:
            return Opcode::LOGICAL_NOT;

        case UnaryOp::BitNot:
            return Opcode::BIT_NOT;

        default:
            throw std::runtime_error("Unknown UnaryOp");
    }
}

void Compiler::visit(UnaryExpr& e) { 
    e.operand->accept(*this);
    emit( unaryOpToOpcode(e.op) );
}


void Compiler::visit(Assignment& e) {
    // 1. Evaluate RHS ONCE
    e.right->accept(*this);
    
    // Case 1: variable assignment
    if (e.left->kind == ExprKind::Variable) {
        auto var = std::static_pointer_cast<Variable>(e.left);

        if (!var->symbol->isMutable) {
            throw KMYCompileError("Immuable modified.");
        }

        // compound assignment
        if (e.op != AssignmentOp::Assign) {
            // emit load_local / load_upvalue etc. according to its resolved type.
            var->accept(*this);
            emit(binaryOpToOpcode(compoundToBinaryOp(e.op)));
        }

        // TODO. STORE_LOCAL or STORE_UPVALUE?
        var->accept(*this);
        return;
    }

    // Case 2: array index assignment
    if (e.left->kind == ExprKind::Index) {
        auto index = std::static_pointer_cast<Index>(e.left);
        // stack: [ ... value, array, index ]
        index->obj->accept(*this);
        index->index->accept(*this);
        emit(Opcode::SET_INDEX);
        return;
    }

    // 2. Resolve target

    /*
    std::visit([&](auto& t) {
        using T = std::decay_t<decltype(t)>;

        if constexpr (std::is_same_v<T, Variable>) {
            int index = resolveLocalIndex(t.name);

            if (!locals[index].isMutable) {
                throw KMYCompileError("Immuable modified.");
            }

            // compound assignment
            if (e.op != AssignmentOp::Assign) {

                // load old value
                emit(Opcode::LOAD_LOCAL locals[index].slot);

                emit(binaryOpToOpcode(compoundToBinaryOp(e.op)));
            }

            emit(Opcode::STORE, locals[index].slot);
            return;
        }
        
        else if constexpr (std::is_same_v<T, Index>) {
            // stack: [ ... value, array, index ]
            t.obj->accept(*this);
            t.index->accept(*this);
            emit(Opcode::SET_INDEX);
        }
        
        
        else if constexpr (std::is_same_v<T, Get>) {
            // stack: [ ... object, value ]
            int nameConst = addConstant(t.name);
            emit(Opcode::SET_PROPERTY, nameConst);
        }
        

    }, e.left);
    */

    throw KMYCompileError("Invalid assignment target");
}

void Compiler::visit(Index& e) { 
    // Push array
    // TODO: no type check here?
    e.obj->accept(*this);

    // Push index
    e.index->accept(*this);

    emit(Opcode::GET_INDEX);
}

void Compiler::visit(Call& e) { 
    // Push function
    e.func->accept(*this);

    // Push args
    for(auto it = e.args.rbegin(); it >= e.args.rend(); ++it) {
        (*it)->accept(*this);
    }

    emit(Opcode::CALL, e.args.size());
}

void Compiler::visit(Get& e) { throw std::runtime_error("NOT IMPLEMENTED"); }


int Compiler::allocateFuncProto(const FunctionProto& fnProto) {
    int idx = funcProtoCnt++;
    funcProtos[idx] = fnProto;
    return idx;
}


void Compiler::visit(FunctionExpr& e) {
    // 1. Create new context
    // Manual memory management required.
    FunctionContext* fnCtx = new FunctionContext;
    fnCtx->parent = currCtx;
    currCtx = fnCtx;

    FunctionProto fnProto;
    fnProto.totalParams = e.params.size();

    // 2. Parameters
    for (auto& param : e.params) {
        allocateLocal(param.symbol);
    }

    // 3. Body
    e.body->accept(*this);

    // 4. Implicit return
    emit(Opcode::RETURN_VOID);

    // 5. Extract compiled context
    fnProto.chunk = std::move(fnCtx->chunk);
    fnProto.upValueCnt = fnCtx->upvalues.size();

    // 6. Register proto
    int fnIndex = allocateFuncProto(fnProto);

    // 7. Emit closure (NOT MAKE_FUNCTION)
    emit(Opcode::MAKE_CLOSURE, fnIndex);

    // 8. Capture variables
    for (auto up : fnCtx->upvalues) {
        // CAPTURE opcode creates a runtime heap object UpValueObj
        if (up.isLocal) {
            // From immediate parent,
            // take x directly from parent stack frame
            emit(Opcode::CAPTURE_LOCAL, up.index);
        } else {
            // take variable from my parent closure’s upvalue list
            emit(Opcode::CAPTURE_UPVALUE, up.index);
        }
    }

    currCtx = fnCtx->parent;
    delete fnCtx;
}


void Compiler::visit(ThisExpr& e) { throw std::runtime_error("NOT IMPLEMENTED"); }
void Compiler::visit(NewExpr& e) { throw std::runtime_error("NOT IMPLEMENTED"); }


// Statements
void Compiler::visit(Print& s) {
    // Compile the expression to leave its value on the stack
    s.expr->accept(*this);

    // Emit print instruction
    emit(Opcode::PRINT);
}


int Compiler::emitJump(Opcode op) {
    emit(op, -1);
    // Returns the pos of the "op" instruction.
    return currCtx->chunk.code.size() - 1;
}

void Compiler::patchJump(int pos) {
    currCtx->chunk.code[pos].operand = currCtx->chunk.code.size();
}

void Compiler::visit(If& s) { 
    s.condition->accept(*this);
    // Now the condition evaluation result is on top of stack.
    int falseJumpPos = emitJump(Opcode::JUMP_IF_FALSE);
    s.thenbranch->accept(*this);

    // Jump over ELSE
    int endJump = emitJump(Opcode::JUMP);

    patchJump(falseJumpPos);
    if (s.elsebranch) {
        s.elsebranch->accept(*this);
    }
    patchJump(endJump);
}


void Compiler::visit(While& s) {
    // mark loop start
    int loopStartPos = currCtx->chunk.code.size();

    // compile condition
    s.condition->accept(*this);

    // jump out if false
    int exitJumpPos = emitJump(Opcode::JUMP_IF_FALSE);

    // loop body
    s.body->accept(*this);

    // jump back to loop start
    emit(Opcode::JUMP, loopStartPos);

    // patch exit jump
    patchJump(exitJumpPos);
}


void Compiler::visit(Block& s) { 
    currCtx->scopeDepth++;   // ENTER scope

    for (auto& stmt : s.statements) {
        stmt->accept(*this);
    }

    currCtx->scopeDepth--;   // EXIT scope

    // remove locals declared in this scope
    while (!currCtx->locals.empty() && currCtx->locals.back().scopeDepth > currCtx->scopeDepth) {
        auto& local = currCtx->locals.back();

        if (local.captured) {
            emit(Opcode::CLOSE_UPVALUE, local.slot);
        }

        currCtx->locals.pop_back();
    }
}

void Compiler::visit(Break& s) { throw std::runtime_error("NOT IMPLEMENTED"); }
void Compiler::visit(Continue& s) { throw std::runtime_error("NOT IMPLEMENTED"); }


int Compiler::allocateLocal(SymbolPtr sym) {
    int slot = currCtx->nextSlot++;

    currCtx->locals.push_back(Local{
        sym,
        slot,
        currCtx->scopeDepth,
        false, // initially not captured.
    });

    return slot;
}

void Compiler::visit(Let& s) { 
    if (s.expr) {
        s.expr->accept(*this);
    } else {
        emit(Opcode::PUSH_CONST, addConstant(nullptr));
    }
    int slot = allocateLocal(s.symbol);
    emit(Opcode::STORE_LOCAL, slot);
    emit(Opcode::POP); // Important: Ensures stack size invariance after statement.
}


void Compiler::visit(Return& s) { throw std::runtime_error("NOT IMPLEMENTED"); }
void Compiler::visit(Class& s) { throw std::runtime_error("NOT IMPLEMENTED"); }

void Compiler::visit(ExprStmt& s) { 
    s.expr->accept(*this);

    // Always pop since Expr leaves one value on the stack
    // and Stmt should not leave no value on the stack.
    emit(Opcode::POP);
}