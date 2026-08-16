#include "IRBuilder.hpp"

#include <assert.h>
#include "../Core/type.hpp"
#include "../Core/errorhandler.hpp"
#include "IRFunction.hpp"
#include <iostream>
#include "../Semantics/TypeInterner.hpp"


static inline bool hasTerminator(HIRBlock* bb) {
    return bb->term.has_value();
}

IRBuilder::IRBuilder(FunctionExprPtr program)
    : program(program) 
{}

void IRBuilder::emit(const IRInstr& instr) {
    assert(currCtx && "No current function context");
    assert(currCtx->currBlock && "No current block");

    currCtx->currBlock->code.push_back(instr);
}


static IRLocalInfo* getValueFromLocal(IRCodegenFnCtx* fnCtx, VarSymbol* sym) {
    auto& localMap = fnCtx->locals;

    auto it = localMap.find(sym);
    if (it != localMap.end()) {
        return &it->second;
    }
    std::cout << ("SERIOUS ERROR?. UNDEFINED cell VAR " + sym->name) << "\n";
    return nullptr;
}

std::vector<HIRFunction*> IRBuilder::compile() {
    assert(program);

    program->accept(*this);

    if (!hasTerminator(currCtx->currBlock)) {
        currCtx->currBlock->term = HaltTerm{};
    }

    return functions;
}

inline HIROperand IRBuilder::getLastValue() { return currCtx->lastValue; }
inline void IRBuilder::setLastValue(HIROperand value) { currCtx->lastValue = value; }

IRValue IRBuilder::makeValue(Type* type) {
    return IRValue{currCtx->nextId++, type};
}

HIRBlock* IRBuilder::makeBlock() {
    auto* newBlock = new HIRBlock{currCtx->nextBlockId++};
    currFunc->blocks.push_back(newBlock);
    return newBlock;
}

void IRBuilder::connectBlock(HIRBlock* from, HIRBlock* to) {
    from->succs.push_back(to);
    to->preds.push_back(from);
}

// ======================================================
// Expressions
// ======================================================

void IRBuilder::visit(Literal& e) {
    ConstValue v = std::visit([](auto&& arg) -> ConstValue {
        return ConstValue(arg);
    }, e.value);

    if (
        !std::holds_alternative<int>(e.value) &&
        !std::holds_alternative<bool>(e.value)
    ) {
        throw KMYCompileError("Only bool and int yet sorry.");
    }

    int val;
    if (std::holds_alternative<int>(e.value)) {
        val = std::get<int>(e.value);
    } else {
        val = (int)std::get<bool>(e.value);
    }

    IRValue dst = makeValue(e.type);
    IRInstr instr{
        .op = IROp::CONST_INT, 
        .dst = dst,
        .imm = val
    };
    emit(instr);

    setLastValue(dst);
}

void IRBuilder::visit(ArrayLiteral& e) {
    throw KMYCompileError("arr Not yet");
}

void IRBuilder::visit(RecordLiteral& e) {
    throw KMYCompileError("rec Not yet");
}

void IRBuilder::visit(Variable& e) {
    auto upvalueIt = currCtx->upvalues.find(e.symbol);
    if (upvalueIt != currCtx->upvalues.end()) {
        IRValue cell = upvalueIt->second;

        IRValue loaded = makeValue(e.type);

        emit({
            .op = IROp::LOAD_CELL,
            .dst = loaded,
            .args = {cell}
        });

        setLastValue(loaded);
        return;
    }

    // Get from local
    IRLocalInfo* localInfo = getValueFromLocal(currCtx, e.symbol);

    if (localInfo && localInfo->isCell) {

        IRValue loaded = makeValue(e.type);

        emit({
            .op = IROp::LOAD_CELL,
            .dst = loaded,
            .args = {localInfo->value}
        });

        setLastValue(loaded);

    } else {
        setLastValue(VarRef{e.symbol});
    }

    // assert(false && "Variable not found in locals or upvalues");
}

static IROp binaryOpToIROp(BinaryOp op) {
    switch (op) {
        case BinaryOp::Plus:          return IROp::ADD;
        case BinaryOp::Minus:         return IROp::SUB;
        case BinaryOp::Star:          return IROp::MUL;
        case BinaryOp::Slash:         return IROp::DIV;
        case BinaryOp::Percent:       return IROp::MOD;

        case BinaryOp::EqualEqual:    return IROp::EQUAL;
        case BinaryOp::NotEqual:      return IROp::NOT_EQUAL;
        case BinaryOp::Less:          return IROp::LESS;
        case BinaryOp::LessEqual:     return IROp::LESS_EQUAL;
        case BinaryOp::Greater:       return IROp::GREATER;
        case BinaryOp::GreaterEqual:  return IROp::GREATER_EQUAL;

        case BinaryOp::LogicalAnd:    return IROp::LOGICAL_AND;
        case BinaryOp::LogicalOr:     return IROp::LOGICAL_OR;

        case BinaryOp::BitAnd:        return IROp::BIT_AND;
        case BinaryOp::BitOr:         return IROp::BIT_OR;
        case BinaryOp::BitXor:        return IROp::BIT_XOR;
        case BinaryOp::LShift:        return IROp::LEFT_SHIFT;
        case BinaryOp::RShift:        return IROp::RIGHT_SHIFT;
    }

    throw KMYCompileError("Unknown BinaryOp");
}

static IROp unaryOpToIROp(UnaryOp op) {
    switch (op) {
        case UnaryOp::Minus:      return IROp::NEG;
        case UnaryOp::BitNot:     return IROp::BIT_NOT;
        case UnaryOp::LogicalNot: return IROp::LOGICAL_NOT;
    }

    throw KMYCompileError("Unknown UnaryOp");
}

void IRBuilder::visit(BinaryExpr& e) {
    if (e.op != BinaryOp::LogicalAnd && e.op != BinaryOp::LogicalOr) {
        // Simple case
        e.left->accept(*this);
        HIROperand lhs = getLastValue();
        
        e.right->accept(*this);
        HIROperand rhs = getLastValue();
    
        IRValue dst = makeValue(e.type);
        IRInstr instr{
            .op = binaryOpToIROp(e.op),
            .dst = dst,
            .args = { lhs, rhs }
        };
        emit(instr);
    
        setLastValue(dst);
        return;
    }

    // e.op is && or ||
    if (e.op == BinaryOp::LogicalAnd) {
        /*
        Format for &&
        -----------
        B0(currBlock):
            ...
            cond = <lhs eval>
            branch cond B1 B2
        
        B1(true):
            ; Using property that true && A == A
            result_B1 = <rhs eval>
            jump B3
        
        B2(false):
            ; Using property that false && A == false
            result_B2 = <false const> 
            jump B3
    
        B3(merge):
            result = phi(result_B1, result_B2)
            ...
        */

        // && short-circuiting
        e.left->accept(*this);
        HIROperand lhs = getLastValue();

        HIRBlock* trueBlock = makeBlock();
        HIRBlock* falseBlock = makeBlock();
        HIRBlock* mergeBlock = makeBlock();

        currCtx->currBlock->term = BranchTerm<IRInstr>{
            .cond = lhs,
            .trueTarget = trueBlock,
            .falseTarget = falseBlock
        };
        connectBlock(currCtx->currBlock, trueBlock);
        connectBlock(currCtx->currBlock, falseBlock);
        
        // True block
        currCtx->currBlock = trueBlock;
        e.right->accept(*this);
        HIROperand rhs = getLastValue();
        
        currCtx->currBlock->term = JumpTerm<IRInstr>{mergeBlock};
        connectBlock(trueBlock, mergeBlock);
        
        // False block
        currCtx->currBlock = falseBlock;
        IRValue falseConst = makeValue(&Types::BOOL_TYPE);
        emit({
            .op = IROp::CONST_INT,
            .dst = falseConst,
            .imm = 0
        });
        
        currCtx->currBlock->term = JumpTerm<IRInstr>{mergeBlock};
        connectBlock(falseBlock, mergeBlock);

        // Merge block
        currCtx->currBlock = mergeBlock;

        IRValue result = makeValue(e.type);
        emit({
            .op = IROp::PHI,
            .dst = result,
            .phis = {
                {trueBlock, rhs},
                {falseBlock, falseConst}
            }
        });

        setLastValue(result);
    } else if (e.op == BinaryOp::LogicalOr) {
        /*
        Format for ||
        -----------
        B0(currBlock):
            ...
            cond = <lhs eval>
            branch cond B1 B2
        
        B1(true):
            ; Using property that true || A == true
            result_B1 = <true const>
            jump B3
        
        B2(false):
            ; Using property that false || A == A
            result_B2 = <rhs eval>
            jump B3

        B3(merge):
            result = phi(result_B1, result_B2)
            ...
        */

        // || short-circuiting
        e.left->accept(*this);
        HIROperand lhs = getLastValue();

        HIRBlock* trueBlock = makeBlock();
        HIRBlock* falseBlock = makeBlock();
        HIRBlock* mergeBlock = makeBlock();

        currCtx->currBlock->term = BranchTerm<IRInstr>{
            .cond = lhs,
            .trueTarget = trueBlock,
            .falseTarget = falseBlock
        };
        connectBlock(currCtx->currBlock, trueBlock);
        connectBlock(currCtx->currBlock, falseBlock);

        // True block
        currCtx->currBlock = trueBlock;
        IRValue trueConst = makeValue(&Types::BOOL_TYPE);
        emit({
            .op = IROp::CONST_INT,
            .dst = trueConst,
            .imm = 1
        });

        // Since the block is just a statement, there is only this terminator.
        currCtx->currBlock->term = JumpTerm<IRInstr>{mergeBlock};
        connectBlock(trueBlock, mergeBlock);

        // False block
        currCtx->currBlock = falseBlock;
        e.right->accept(*this);
        HIROperand rhs = getLastValue();

        // Since the block is just a statement, there is only this terminator.
        currCtx->currBlock->term = JumpTerm<IRInstr>{mergeBlock};
        connectBlock(falseBlock, mergeBlock);

        // Merge block
        currCtx->currBlock = mergeBlock;
        IRValue result = makeValue(e.type);
        emit({
            .op = IROp::PHI,
            .dst = result,
            .phis = {
                {trueBlock, trueConst},
                {falseBlock, rhs}
            }
        });

        setLastValue(result);
    }
}

void IRBuilder::visit(UnaryExpr& e) {
    e.operand->accept(*this);
    HIROperand v = getLastValue();

    IRValue dst = makeValue(e.type);

    IRInstr instr{
        .op = unaryOpToIROp(e.op),
        .dst = dst,
        .args = { v }
    };
    emit(instr);

    setLastValue(dst);
}

void IRBuilder::visit(Assignment& e) {
    // Case 1: variable assignment
    assert(e.left->isLValue());

    if (e.left->kind == ExprKind::Variable) {
        auto var = std::static_pointer_cast<Variable>(e.left);
        if (!var->symbol->isMutable) {
            throw KMYCompileError("Assignment to a constant variable \"" + var->name + "\"");
        }
        
        if (e.op != AssignmentOp::Assign) {
            e.left->accept(*this);
            HIROperand lhs = getLastValue();
            e.right->accept(*this);
            HIROperand rhs = getLastValue();
            
            IRValue dst = makeValue(e.type);
            IRInstr instr{
                .op = binaryOpToIROp(compoundToBinaryOp(e.op)),
                .dst = dst,
                .args = { lhs, rhs }
            };
            emit(instr);

            setLastValue(dst);
        } else {
            e.right->accept(*this);
            // last value isnt updated.
            // In a = 42, the last value is RHS.
        }
        // The locals must already contain the symbol.
        // Though this may never happen.

        // UNLESS it is an upvalue!
        // Emit SET_ENV for mutable upvalues (like x = 3 where x is not local.)
        std::cout << "[assignment] check: " << var->symbol << std::endl;
        if (currCtx->locals.count(var->symbol)) {
            std::cout << "[assignment] check: " << var->symbol << " exists in locals. this is an local cell mutation.\n";
            IRLocalInfo* varInfo = getValueFromLocal(currCtx, var->symbol);
            if (varInfo && varInfo->isCell) {
                emit({
                    .op = IROp::STORE_CELL,
                    .args = {
                        varInfo->value,
                        getLastValue()
                    }
                });
                return;
            }
        } else if (currCtx->upvalues.count(var->symbol) == 0) {
            // non cell mutation.
            HIROperand v = getLastValue();
            //currCtx->locals[var->symbol].value = v;
            currCtx->currBlock->defs.push_back({var->symbol, v});
            return;
        }
        
        // Upvalue mutation
        assert(currCtx->incomingEnv.has_value());
        
        // Should have a parent that provides addr to upvalue.
        std::cout << var->name << std::endl;
        std::cout << "= " << getLastValue() << std::endl;
        assert(currCtx->parent);

        IRValue cell = currCtx->upvalues[var->symbol];

        emit({
            .op = IROp::STORE_CELL,
            .args = {
                cell,
                getLastValue()
            }
        });

        return;
    }

    throw KMYCompileError("Invalid assignment target");
}

void IRBuilder::visit(Index& e) {
    throw KMYCompileError("index Not yet");
}

void IRBuilder::visit(Call& e) {
    std::optional<IRValue> v = std::nullopt;
    e.func->accept(*this);

    std::vector<HIROperand> args = { getLastValue() }; // First is always the function value.
    for (const auto& arg : e.args) {
        arg->accept(*this);
        args.push_back( getLastValue() );
    }

    if (e.type != &Types::VOID_TYPE) {
        v = makeValue(e.type);
    }

    IRInstr instr{
        .op = IROp::CALL,
        .dst = v,
        .args = std::move(args)
    };
    emit(instr);
    
    // NOTE: In prev semantic passes, it is guaranteed that void isn't stored anywhere.
    if (e.type != &Types::VOID_TYPE) {
        setLastValue(v.value());
    }
}

void IRBuilder::visit(Get& e) {
    throw KMYCompileError("get Not yet");
}

void IRBuilder::visit(ScopeAccessExpr& e) {
    throw KMYCompileError("scope acc Not yet");
}

static void printFunctionContext(FunctionContext* fnCtx) {
    if (!fnCtx) {
        std::cout << "<null function context>\n";
        return;
    }

    std::cout << "========================================\n";
    std::cout << "FunctionContext @" << fnCtx << "\n";

    //
    // Locals
    //
    std::cout << "\nLocals (" << fnCtx->locals.size() << ")\n";
    std::cout << "----------------------------------------\n";

    for (const auto& local : fnCtx->locals) {
        std::cout
            << "  "
            << local.sym->name
            << "  slot=" << local.slot
            << "  depth=" << local.scopeDepth
            << "  captured=" << std::boolalpha << local.captured
            << "\n";
    }

    //
    // Upvalues
    //
    std::cout << "\nUpvalues (" << fnCtx->upvalues.size() << ")\n";
    std::cout << "----------------------------------------\n";

    for (int i = 0; i < fnCtx->upvalues.size(); ++i) {
        const auto& up = fnCtx->upvalues[i];

        std::cout
            << "  ["
            << i
            << "] "
            << up.symbol->name
            << "  parentSlot=" << up.index
            << "  isLocal=" << std::boolalpha << up.isLocal
            << "  capturedByChildren="
            << up.capturedByChildren
            << "\n";
    }

    //
    // Environment layout
    //
    std::cout << "\nEnvironment (" << fnCtx->envMap.size()
              << " slots, size=" << fnCtx->envSize << ")\n";
    std::cout << "----------------------------------------\n";

    for (const auto& [sym, slot] : fnCtx->envMap) {
        std::cout
            << "  slot " << slot
            << " -> " << sym->name
            << "\n";
    }

    //
    // Local map
    //
    std::cout << "\nLocalMap\n";
    std::cout << "----------------------------------------\n";

    for (const auto& [sym, slot] : fnCtx->localMap) {
        std::cout
            << "  "
            << sym->name
            << " -> " << slot
            << "\n";
    }

    //
    // Upvalue map
    //
    std::cout << "\nUpvalueMap\n";
    std::cout << "----------------------------------------\n";

    for (const auto& [sym, slot] : fnCtx->upvalueMap) {
        std::cout
            << "  "
            << sym->name
            << " -> " << slot
            << "\n";
    }

    std::cout << "========================================\n";
}

void IRBuilder::visit(FunctionExpr& e) {
    
    IRValue fnValue;
    
    std::cout << "NEW FUNC " << e.functionId << "\n";
    printFunctionContext(e.functionContext);

    // Context switch to a new function.
    auto oldCtx = currCtx;
    HIRFunction* oldFn = currFunc;

    currCtx = new IRCodegenFnCtx{};
    currCtx->parent = oldCtx;   
    currCtx->fnCtx = e.functionContext;

    currFunc = new HIRFunction{ 
        .functionId = e.functionId, 
        .funcType = static_cast<FunctionType*>(e.type),
        .isEntryFunc = e.isEntry,
    };

    HIRBlock* entry = makeBlock();
    // entry has no preds!

    currFunc->entry = entry;
    currCtx->currBlock = entry;

    // Incoming env from parent
    // Only introduce env if there is a free variable in the function
    // i.e., if upvalues are present.
    std::optional<IRValue> incomingEnv;
    if (!e.functionContext->upvalues.empty()) {
        incomingEnv = makeValue(new PointerType(&Types::VOID_TYPE));
        currCtx->incomingEnv = incomingEnv; // Unused?

        IRInstr instr{
            .op  = IROp::INTRODUCE_ENV,
            .dst = incomingEnv.value()
        };

        emit(instr);
    }


    // Introduce params
    // NOTE: This must happen at the very beginning of the function.

    struct CapturedParamInfo {
        VarSymbol* sym;
        IRValue v;
        int envSlot;
    };

    std::vector<CapturedParamInfo> capturedParams;

    for (int i = 0; i < e.params.size(); i++) {
        IRValue v = makeValue(e.params[i].symbol->type);
        // Allow locals (function body) to see this param.
        currCtx->locals[e.params[i].symbol] = IRLocalInfo{ .value = v, .isCell = false };


        // Check whether the parameter is captured.
        // (Same logic as Let stmt visit)
        auto it = currCtx->fnCtx->envMap.find(e.params[i].symbol);
        if (it != currCtx->fnCtx->envMap.end()) {
            // This param needs to be SET_ENV'ed!
            // Since currCtx->env is not present here, we delegate it.
            capturedParams.push_back({e.params[i].symbol, v, it->second});
        }

        // Instructing the function to use the param
        /*
        main
        -------
        B0:
            v0 = 1
            v1 = 2
            v2 = CALL v0, v1 

        add
        -------
        B0:
            v0 = PARAM 0
            v1 = PARAM 1
            v2 = ADD v0, v1
            PRINT v2
        */
        IRInstr instr {
            .op = IROp::PARAM,
            .dst = v,
            .imm = i
        };
        emit(instr);
    }

    // Allocate my env (for children) if children capture my locals or upvalues
    if (e.functionContext->envSize > 0) {
        IRValue env = makeValue(new PointerType(&Types::VOID_TYPE));

        currCtx->env = env;

        IRInstr instr{
            .op  = IROp::ALLOC_ENV,
            .dst = env,
            .imm = e.functionContext->envSize
        };

        emit(instr);
    }

    // Now SET_ENV the captured params
    if (currCtx->env) {
        for (auto [sym, v, envSlot] : capturedParams) {
    
            IRValue cell = makeValue(
                new CellType(sym->type)
            );
    
            emit({
                .op = IROp::ALLOC_CELL_INIT,
                .dst = cell,
                .args = { v }
            });
    
            emit({
                .op = IROp::SET_ENV,
                .args = { currCtx->env.value(), cell },
                .imm = envSlot
            });
    
            // IMPORTANT! Overwrite local(param) reads with cell reads.
            currCtx->locals[sym] = IRLocalInfo{ .value = cell, .isCell = true };
        }            
    }

    // At this point, all params must have been assigned as locals
    // either as a cell or just a normal local.
    for (const auto& param: e.params) {
        IRLocalInfo& paramInfo = currCtx->locals[param.symbol];
        if (!paramInfo.isCell) {
            currCtx->currBlock->defs.push_back({param.symbol, paramInfo.value});
        }
    }


    // Introduce upvalues (They are accessed via incoming env with given env slot.)
    // They are assigned on function entry from its env.
    for (int i = 0; i < e.functionContext->upvalues.size(); i++) {
        auto& up = e.functionContext->upvalues[i];
        IRValue v = makeValue(new CellType(up.symbol->type));

        currCtx->upvalues[up.symbol] = v; // Needed for varaible upvalue lookup.

        // Having upvalue means incoming env and parent exists!
        assert(e.functionContext->parent != nullptr); 
        assert(e.functionContext->parent->envMap.count(up.symbol) > 0);
        IRInstr instr {
            .op = IROp::GET_ENV,    
            .dst = v,
            .args = { incomingEnv.value() },
            .imm = e.functionContext->parent->envMap.at(up.symbol) 
        };

        emit(instr);

        if (up.capturedByChildren) {
            std::cout << "Hey! it's a me, UPVALUECAPTUREDBYCHILDREN!\n";
            // Children captures MY upvalue
            // We need to put it in env so that the children can see my upvalues.
            
            assert(e.functionContext->envMap.count(up.symbol) > 0);
            
            // Here, v is already a pointer from parent's env. so a copy should occur.
            // This is essentially flattening environment.
            IRInstr instr {
                .op = IROp::SET_ENV,    
                .args = { currCtx->env.value(), v },
                .imm = e.functionContext->envMap.at(up.symbol)
            };

            emit(instr);
        }
    }

    // Hoist function declarations (in opposite order?)
    for (auto& [funcDeclSym, funcDeclExpr] : e.functionContext->funcDecls) {
        fnValue = makeValue(e.type);
        //assert(currCtx->env.has_value());

        std::vector<HIROperand> arg = {};
        if (currCtx->env.has_value()) { arg.push_back(currCtx->env.value()); }
        else { 
            IRValue nullValue = makeValue(new PointerType(&Types::VOID_TYPE));
            emit({
                .op = IROp::CONST_INT,
                .dst = nullValue,
                .imm = 0
            });
            arg.push_back(nullValue); 
        }

        IRInstr instr {
            .op = IROp::FUNC_LABEL,
            .dst = fnValue, 
            .args = arg, // Env to use
            .imm = funcDeclExpr->functionId // TODO: Don't store the whole expr...
        };
        emit(instr);
        
        //funcDeclExpr->accept(*this);

        // Quirky! TODO!!!
        bool uncapturedLocal = false;
        if (currCtx->locals.find(funcDeclSym) == currCtx->locals.end()) {
            currCtx->locals[funcDeclSym] = IRLocalInfo{ .value = fnValue, .isCell = false };
            uncapturedLocal = true;
        } else {
            std::cout << "Duplicate write prevented which is not very desired.\n";
        }

        auto it = currCtx->fnCtx->envMap.find(funcDeclSym);
        
        // If the hoisted function (closure) itself is needed from children closure,
        // Set it on env (func/closure is also a pointer, so copy occurs.)
        if (it != currCtx->fnCtx->envMap.end()) {
            assert(currCtx->env.has_value());
            std::cout << "Hoisted function " << funcDeclSym->name << " is captured by children. Setting it on env.\n";
            
            IRValue cell = makeValue(
                new CellType(e.type)
            );
    
            emit({
                .op = IROp::ALLOC_CELL_INIT,
                .dst = cell,
                .args = { fnValue }
            });
    
            emit({
                .op = IROp::SET_ENV,
                .args = { currCtx->env.value(), cell },
                .imm = it->second
            });
            uncapturedLocal = false;
        }

        if (uncapturedLocal) {
            currCtx->currBlock->defs.push_back({funcDeclSym, fnValue});
        }
    }

    std::cout << "env\n";
    for (auto& [sym, slot] : e.functionContext->envMap) {
        std::cout << sym->name << " -> " << slot << "\n";
    }
    std::cout << "locals so far\n";
    for (auto& [sym, v] : currCtx->locals) {
        std::cout << sym->name << " -> " << v.value << " (isCell: " << v.isCell << ")\n";
    }

    
    // body
    e.body->accept(*this);    

    // implicit return
    if (!hasTerminator(currCtx->currBlock)) {
        currCtx->currBlock->term = ReturnTerm{};
    }

    functions.push_back(currFunc);
    currFunc->lastValueId = currCtx->nextId;

    // Prevent currCtx from being nullptr (top level entry func)
    if (oldCtx) {
        currFunc = oldFn;
        currCtx = oldCtx;
        setLastValue(fnValue);
    }
}

void IRBuilder::visit(ThisExpr& e) {
    throw KMYCompileError("this Not yet");
}

void IRBuilder::visit(NewExpr& e) {
    throw KMYCompileError("new Not yet");
}

// ======================================================
// Statements
// ======================================================

void IRBuilder::visit(Print& s) {
    s.expr->accept(*this);

    IRInstr instr{
        .op = IROp::PRINT,
        .args = {getLastValue()}
    };
    emit(instr);
}

void IRBuilder::visit(If& s) {
    /*
    Format
    -----------
    B0(currBlock):
        <condition>
        branch cond B1 B2
    
    B1(then):
        <thenBranch>
        jump B3
    
    B2(else): ; This may not exist!
        <elseBranch>
        jump B3
    
    B3(merge):
        ...
    */
    
    // Note that order matters!!
    HIRBlock* thenBB  = makeBlock();
    HIRBlock* mergeBB = makeBlock();

    // Note that else block is just final block if else branch doesn't exist.
    HIRBlock* elseBB = mergeBB;

    if (s.elsebranch) {
        elseBB = makeBlock();
    }

    // Condition lives in current block.
    s.condition->accept(*this);
    HIROperand cond = getLastValue();

    currCtx->currBlock->term = BranchTerm<IRInstr>{
        cond,
        thenBB,
        elseBB
    };

    connectBlock(currCtx->currBlock, thenBB);
    connectBlock(currCtx->currBlock, elseBB);

    //
    // THEN
    //
    currCtx->currBlock = thenBB;
    s.thenbranch->accept(*this);
    // After thenBranch, the terminator MAY exist (e.g. return)
    // so we should be careful not to overwrite it.
    if (!hasTerminator(currCtx->currBlock)) {
        currCtx->currBlock->term = JumpTerm<IRInstr>{mergeBB};
        connectBlock(currCtx->currBlock, mergeBB);
    }

    //
    // ELSE
    //
    if (s.elsebranch) {
        currCtx->currBlock = elseBB;
        s.elsebranch->accept(*this);

        // Similar logic to thenBranch.
        if (!hasTerminator(currCtx->currBlock)) {
            currCtx->currBlock->term = JumpTerm<IRInstr>{mergeBB};
            connectBlock(currCtx->currBlock, mergeBB);
        }
    }

    currCtx->currBlock = mergeBB;
}

void IRBuilder::visit(While& s) {
    /*
    Format
    -----------
    B0(currBlock):
        <condition>
        jump B1
    
    B1(cond):
        <condition>
        branch cond B2 B3

    B2(body):
        ...
        <condition>
        jump B1
    
    B3(exit):
        ...
    */
    HIRBlock* condBB = makeBlock();
    HIRBlock* bodyBB = makeBlock();
    HIRBlock* exitBB = makeBlock();

    currCtx->currBlock->term = JumpTerm<IRInstr>{condBB};

    connectBlock(currCtx->currBlock, condBB);

    // Condition lives in condBB
    currCtx->currBlock = condBB;

    s.condition->accept(*this);
    HIROperand cond = getLastValue();

    currCtx->currBlock->term =
        BranchTerm<IRInstr>{cond, bodyBB, exitBB};

    connectBlock(condBB, bodyBB);
    connectBlock(condBB, exitBB);

    currCtx->loopStack.push_back({condBB, exitBB});

    currCtx->currBlock = bodyBB;
    s.body->accept(*this);

    if (!hasTerminator(currCtx->currBlock)) {
        currCtx->currBlock->term = JumpTerm<IRInstr>{condBB};
        connectBlock(currCtx->currBlock, condBB);
    }

    currCtx->loopStack.pop_back();

    currCtx->currBlock = exitBB;
}

void IRBuilder::visit(Block& s) {
    for (auto& stmt : s.statements) {
        if (hasTerminator(currCtx->currBlock)) {
            // Dead code after terminator
            break;
        }
        stmt->accept(*this);
    }
}

void IRBuilder::visit(Break& s) {
    if (currCtx->loopStack.empty()) {
        throw KMYCompileError("break outside loop");
    }

    auto& loop = currCtx->loopStack.back();

    currCtx->currBlock->term = JumpTerm<IRInstr>{
        loop.breakTarget
    };

    connectBlock(currCtx->currBlock, loop.breakTarget);

    // Return to the upper block
    //currCtx->currBlock = loop.breakTarget;

    // This(^) is not needed because the break target will be the next block to execute after the loop, 
    // and the current block is effectively terminated by the jump instruction.

    // Simply, currBlock is done after break/continue,
    // And the parent AST will contigure where to emit code afterwards.
    // Since break/continue only happens in loops, While& will handle that.
}

void IRBuilder::visit(Continue& s) {
    if (currCtx->loopStack.empty()) {
        throw KMYCompileError("continue outside loop");
    }

    auto& loop = currCtx->loopStack.back();

    currCtx->currBlock->term = JumpTerm<IRInstr>{
        loop.continueTarget
    };

    connectBlock(currCtx->currBlock, loop.continueTarget);

    // current block is dead after continue
    //currCtx->currBlock = loop.continueTarget;
}

void IRBuilder::visit(Let& s) {
    if (s.isFunctionDecl) {
        // Do not traverse when it is hoisted.
        // It is traversed in the function expr.
        //return;   
    }

    if (s.expr) {
        s.expr->accept(*this);
    } else {
        throw KMYCompileError("Uninit expr not yet.");
    }

    // Don't allocate function decl again (already hoisted in func expr visit)
    if (s.isFunctionDecl) { return; }

    HIROperand value = getLastValue();
    
    bool uncapturedLocal = true;
    if (currCtx->locals.find(s.symbol) == currCtx->locals.end()) {
        std::cout << "[let] wtf why are you here: " << s.symbol->name << "\n";
        uncapturedLocal = true;
       // currCtx->locals[s.symbol] = IRLocalInfo{ .value = value, .isCell = false };
    } else {
        std::cout << "Duplicate write prevented which is not very desired.\n";
    }

    // Set env right away.
    // This decl is captured from children closure.
    // This time, we need to copy the POINTER of this value.
    auto it = currCtx->fnCtx->envMap.find(s.symbol);

    IRValue cell;
    std::cout << "[let] currctx->env: " << currCtx->env.has_value() << "\n";
    
    if (currCtx->env && it != currCtx->fnCtx->envMap.end()) {
        uncapturedLocal = false;
        std::cout << "[let] " << s.symbol->name << " exists in env\n";

        Type* type = nullptr;
        if (std::holds_alternative<IRValue>(value)) {
            type = std::get<IRValue>(value).type;
        } else if (std::holds_alternative<VarRef>(value)) {
            type = std::get<VarRef>(value).sym->type;
        } 

        cell = makeValue(
            new CellType(type)
        );

        emit({
            .op = IROp::ALLOC_CELL_INIT,
            .dst = cell,
            .args = { value }
        });

        emit({
            .op = IROp::SET_ENV,
            .args = { currCtx->env.value(), cell },
            .imm = it->second
        });

        // IMPORTANT! Overwrite local reads with cell reads.
        currCtx->locals[s.symbol] = IRLocalInfo{ .value = cell, .isCell = true };
    }

    if (uncapturedLocal) {
        currCtx->currBlock->defs.push_back({s.symbol, value});
    } else {
        //currCtx->currBlock->defs.push_back({s.symbol, cell});
    }
}

void IRBuilder::visit(Return& s) {
    s.expr->accept(*this);

    currCtx->currBlock->term = ReturnTerm{ getLastValue() };
}

void IRBuilder::visit(Aggregate& s) {
    throw KMYCompileError("agg Not yet");
}

void IRBuilder::visit(TypeAlias& s) {}
void IRBuilder::visit(Enum& s) {}

void IRBuilder::visit(ExprStmt& s) {
    s.expr->accept(*this);
}