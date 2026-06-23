#include "IRBuilder.hpp"

#include <assert.h>
#include "../Core/type.hpp"
#include "../Core/errorhandler.hpp"
#include "IRFunction.hpp"
#include <iostream>

static bool hasTerminator(BasicBlock* bb) {
    return bb->term.has_value();
}

IRBuilder::IRBuilder(FunctionExprPtr program)
    : program(program) 
{
}


static IRValue getValueFromLocal(IRCodegenFnCtx* fnCtx, VarSymbol* sym) {
    auto& localMap = fnCtx->locals;

    auto it = localMap.find(sym);
    if (it != localMap.end()) {
        return it->second;
    }

    throw KMYCompileError("SERIOUS ERROR. UNDEFINED VAR");
}

std::vector<IRFunction*> IRBuilder::compile() {
    assert(program);

    program->accept(*this);

    if (!hasTerminator(currCtx->currBlock)) {
        currCtx->currBlock->term = HaltTerm{};
    }

    return functions;
}

inline IRValue IRBuilder::getLastValue() { return currCtx->lastValue; }
inline void IRBuilder::setLastValue(IRValue value) { currCtx->lastValue = value; }

IRValue IRBuilder::makeValue(Type* type) {
    return IRValue{currCtx->nextId++, type};
}

BasicBlock* IRBuilder::makeBlock() {
    auto* newBlock = new BasicBlock{currCtx->nextBlockId++};
    currFunc->blocks.push_back(newBlock);
    return newBlock;
}

void IRBuilder::connectBlock(BasicBlock* from, BasicBlock* to) {
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
    currCtx->currBlock->code.push_back(instr);

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
        setLastValue(upvalueIt->second);
        return;
    }

    // Get from local
    setLastValue( getValueFromLocal(currCtx, e.symbol) );

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

void IRBuilder::visit(BinaryExpr& e) {
    e.left->accept(*this);
    IRValue lhs = getLastValue();
    
    e.right->accept(*this);
    IRValue rhs = getLastValue();

    IRValue dst = makeValue(e.type);
    IRInstr instr{
        .op = binaryOpToIROp(e.op),
        .dst = dst,
        .args = { lhs, rhs }
    };
    currCtx->currBlock->code.push_back(instr);

    setLastValue(dst);
}

void IRBuilder::visit(UnaryExpr& e) {
    throw KMYCompileError("unary Not yet");
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
            IRValue lhs = getLastValue();
            e.right->accept(*this);
            IRValue rhs = getLastValue();
            
            IRValue dst = makeValue(e.type);
            IRInstr instr{
                .op = binaryOpToIROp(compoundToBinaryOp(e.op)),
                .dst = dst,
                .args = { lhs, rhs }
            };
            currCtx->currBlock->code.push_back(instr);

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
        if (currCtx->locals.count(var->symbol)) {
            currCtx->locals[var->symbol] = getLastValue();
            return;
        }
        
        // Upvalue mutation
        assert(currCtx->incomingEnv.has_value());
        IRInstr instr{
            .op = IROp::SET_ENV,
            .args = { currCtx->incomingEnv.value(), getLastValue() }
        };
        currCtx->currBlock->code.push_back(instr);


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

    std::vector<IRValue> args = { getLastValue() }; // First is always the function value.
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
    currCtx->currBlock->code.push_back(instr);
    
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
    std::optional<IRValue> upperEnv;
    
    std::cout << "NEW FUNC " << e.functionId << "\n";
    printFunctionContext(e.functionContext);

    // Context switch
    auto oldCtx = currCtx;
    IRFunction* oldFn = currFunc;

    currCtx = new IRCodegenFnCtx{};
    currCtx->fnCtx = e.functionContext;

    currFunc = new IRFunction{ e.functionId, static_cast<FunctionType*>(e.type) };

    BasicBlock* entry = makeBlock();
    // entry has no preds!

    currFunc->entry = entry;
    currCtx->currBlock = entry;

    // Allocate my env if children capture my locals or upvalues
    if (e.functionContext->envSize > 0) {
        IRValue env = makeValue(nullptr);

        currCtx->env = env;

        IRInstr instr{
            .op  = IROp::ALLOC_ENV,
            .dst = env,
            .imm = e.functionContext->envSize
        };

        currCtx->currBlock->code.push_back(instr);
    }


    // Incoming env from parent
    std::optional<IRValue> incomingEnv;
    if (!e.functionContext->upvalues.empty()) {
        incomingEnv = makeValue(nullptr);
        currCtx->incomingEnv = incomingEnv;

        IRInstr instr{
            .op  = IROp::INTRODUCE_ENV,
            .dst = incomingEnv.value()
        };

        currCtx->currBlock->code.push_back(instr);
    }


    // Introduce params
    for (int i = 0; i < e.params.size(); i++) {
        IRValue v = makeValue(e.params[i].symbol->type);

        // Allow locals (function body) to see this param.
        currCtx->locals[e.params[i].symbol] = v;

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
        currCtx->currBlock->code.push_back(instr);
    }

    // Introduce upvalues
    // They are assigned on function entry from its env.
    for (int i = 0; i < e.functionContext->upvalues.size(); i++) {
        auto& up = e.functionContext->upvalues[i];
        IRValue v = makeValue(up.symbol->type);

        currCtx->upvalues[up.symbol] = v; // Needed? maybe

        IRInstr instr {
            .op = IROp::GET_ENV,    
            .dst = v,
            .args = { incomingEnv.value() },
            .imm = up.index // Upvalue slot allocated from pass 4.
        };

        currCtx->currBlock->code.push_back(instr);

        if (up.capturedByChildren) {
            std::cout << "Hey! it's a me, UPVALUECAPTUREDBYCHILDREN!\n";
            // Children captures MY upvalue
            // We need to put it in env so that the children can see my upvalues.
            
            assert(e.functionContext->envMap.count(up.symbol) > 0);
            IRInstr instr {
                .op = IROp::SET_ENV,    
                .dst = v,
                .args = { currCtx->env.value(), v },
                .imm = e.functionContext->envMap.at(up.symbol)
            };

            currCtx->currBlock->code.push_back(instr);
        }
    }

    // Hoist function declarations (in opposite order)
    for (auto& [funcDeclSym, funcDeclExpr] : e.functionContext->funcDecls) {
        fnValue = makeValue(e.type);
        if (currCtx) {
            assert(currCtx->env.has_value());

            IRInstr instr {
                .op = IROp::FUNC_LABEL,
                .dst = fnValue, 
                .args = { currCtx->env.value() }, // Env to use
                .imm = funcDeclExpr->functionId // TODO: Don't store the whole expr...
            };
            currCtx->currBlock->code.push_back(instr);
        }
        //funcDeclExpr->accept(*this);

        if (currCtx->locals.find(funcDeclSym) == currCtx->locals.end()) {
            currCtx->locals[funcDeclSym] = fnValue;
        } else {
            std::cout << "Duplicate write prevented which is not very desired.\n";
        }

        auto it = currCtx->fnCtx->envMap.find(funcDeclSym);
        assert(currCtx->env.has_value());
        
        if (it != currCtx->fnCtx->envMap.end()) {
            currCtx->currBlock->code.push_back({
                .op = IROp::SET_ENV,
                .args = { currCtx->env.value(), fnValue },
                .imm = it->second
            });
        }
    }

    std::cout << "env\n";
    for (auto& [sym, slot] : e.functionContext->envMap) {
        std::cout << sym->name << " -> " << slot << "\n";
    }
    std::cout << "locals so far\n";
    for (auto& [sym, v] : currCtx->locals) {
        std::cout << sym->name << " -> " << v << "\n";
    }

    
    // body
    e.body->accept(*this);
    
    /*
    // populate MY env
    if (currCtx->env) {
        // CONTINUE FROM HERE
        // outer middle inner capture fail.
        for (auto& local : e.functionContext->locals) {

            if (!local.captured)
                continue;
            std::cout << "searching " << local.sym->name << "\n";
            currCtx->currBlock->code.push_back({
                .op   = IROp::SET_ENV,
                .args = {
                    currCtx->env.value(),
                    // This causes error.
                    currCtx->locals.at(local.sym)
                },
                .imm = e.functionContext->envMap.at(local.sym)
            });
        }
    }
    */
    

    // implicit return
    if (!hasTerminator(currCtx->currBlock)) {
        currCtx->currBlock->term = ReturnTerm{};
    }

    functions.push_back(currFunc);


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
    currCtx->currBlock->code.push_back(instr);
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
    BasicBlock* thenBB  = makeBlock();
    BasicBlock* mergeBB = makeBlock();

    // Note that else block is just final block if else branch doesn't exist.
    BasicBlock* elseBB = mergeBB;

    if (s.elsebranch) {
        elseBB = makeBlock();
    }

    // Condition lives in current block.
    s.condition->accept(*this);
    IRValue cond = getLastValue();

    currCtx->currBlock->term = BranchTerm{
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
        currCtx->currBlock->term = JumpTerm{mergeBB};
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
            currCtx->currBlock->term = JumpTerm{mergeBB};
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
    BasicBlock* condBB = makeBlock();
    BasicBlock* bodyBB = makeBlock();
    BasicBlock* exitBB = makeBlock();

    currCtx->currBlock->term = JumpTerm{condBB};

    connectBlock(currCtx->currBlock, condBB);

    // Condition lives in condBB
    currCtx->currBlock = condBB;

    s.condition->accept(*this);
    IRValue cond = getLastValue();

    currCtx->currBlock->term =
        BranchTerm{cond, bodyBB, exitBB};

    connectBlock(condBB, bodyBB);
    connectBlock(condBB, exitBB);

    currCtx->loopStack.push_back({condBB, exitBB});

    currCtx->currBlock = bodyBB;
    s.body->accept(*this);

    if (!hasTerminator(currCtx->currBlock)) {
        currCtx->currBlock->term = JumpTerm{condBB};
        connectBlock(currCtx->currBlock, condBB);
    }

    currCtx->loopStack.pop_back();
    connectBlock(currCtx->currBlock, condBB);

    currCtx->currBlock = exitBB;
}

void IRBuilder::visit(Block& s) {
    for (auto& stmt : s.statements) {
        stmt->accept(*this);
    }
}

void IRBuilder::visit(Break& s) {
    if (currCtx->loopStack.empty()) {
        throw KMYCompileError("break outside loop");
    }

    auto& loop = currCtx->loopStack.back();

    currCtx->currBlock->term = JumpTerm{
        loop.breakTarget
    };

    connectBlock(currCtx->currBlock, loop.breakTarget);

    currCtx->currBlock = makeBlock();
}

void IRBuilder::visit(Continue& s) {
    if (currCtx->loopStack.empty()) {
        throw KMYCompileError("continue outside loop");
    }

    auto& loop = currCtx->loopStack.back();

    currCtx->currBlock->term = JumpTerm{
        loop.continueTarget
    };

    connectBlock(currCtx->currBlock, loop.continueTarget);

    // current block is dead after continue
    currCtx->currBlock = makeBlock();
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

    IRValue value = getLastValue();
    if (currCtx->locals.find(s.symbol) == currCtx->locals.end()) {
        currCtx->locals[s.symbol] = value;
    } else {
        std::cout << "Duplicate write prevented which is not very desired.\n";
    }

    // Set env right away.
    auto it = currCtx->fnCtx->envMap.find(s.symbol);
    if (currCtx->env && it != currCtx->fnCtx->envMap.end()) {

        currCtx->currBlock->code.push_back({
            .op = IROp::SET_ENV,
            .args = { currCtx->env.value(), value },
            .imm = it->second
        });
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