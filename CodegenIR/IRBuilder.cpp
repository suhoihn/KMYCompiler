#include "IRBuilder.hpp"

#include <assert.h>
#include "../Core/type.hpp"
#include "../Core/errorhandler.hpp"
#include "IRFunction.hpp"

static bool hasTerminator(BasicBlock* bb) {
    return bb->term.has_value();
}

IRBuilder::IRBuilder(FunctionExprPtr program)
    : program(program) 
{
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

    if (!std::holds_alternative<int>(e.value)) {
        throw KMYCompileError("Only int yet sorry.");
    }

    auto val = std::get<int>(e.value);

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
    auto localIt = currCtx->locals.find(e.symbol);
    if (localIt != currCtx->locals.end()) {
        setLastValue(localIt->second);
        return;
    }

    auto upvalueIt = currCtx->upvalues.find(e.symbol);
    if (upvalueIt != currCtx->upvalues.end()) {
        setLastValue(upvalueIt->second);
        return;
    }

    assert(false && "Variable not found in locals or upvalues");
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
            return;
        } else {
            e.right->accept(*this);
            currCtx->locals[var->symbol] = getLastValue();
            // last value isnt updated.
            // In a = 42, the last value is RHS.
        }
        return;
    }

    throw KMYCompileError("Invalid assignment target");
}

void IRBuilder::visit(Index& e) {
    throw KMYCompileError("index Not yet");
}

void IRBuilder::visit(Call& e) {
    std::optional<IRValue> v = std::nullopt;
    if (e.type != &Types::VOID_TYPE) {
        v = makeValue(e.type);
    }
    e.func->accept(*this);

    std::vector<IRValue> args = { getLastValue() }; // First is always the function value.
    for (const auto& arg : e.args) {
        arg->accept(*this);
        args.push_back( getLastValue() );
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

void IRBuilder::visit(FunctionExpr& e) {
    // TODO: I definitiely need functioncontext struct.
    // CONTINUE FROM HERE
    // Encode function in call instr as pointer.
    // find out why current version is broken
    // with the CURRENT VERSION of compiler, test SimpleFunctionTest.kmy and ChatGPT history.
    // GOOD LUCK.
    // Unfinished job by suho in the past. ^_^
    
    auto oldCtx = currCtx;

    IRValue fnValue;
    if (currCtx) {
        fnValue = makeValue(e.type);
        std::vector<IRCapture> capturedVals;
        for (auto& up : e.upvalues) {
            if (up.isLocal) {
                // Capturing in the closure's local
                capturedVals.push_back({ true, currCtx->locals.at(up.symbol) });
            } else {
                // Capturing the closure's upvalue
                capturedVals.push_back({ false, currCtx->upvalues.at(up.symbol) });
            }
        }

        IRInstr instr {
            .op = IROp::FUNC_LABEL,
            .dst = fnValue, 
            .captures = capturedVals,
            .imm = e.functionId
        };
        currCtx->currBlock->code.push_back(instr);
    }

    IRFunction* oldFn = currFunc;

    currCtx = new IRCodegenFnCtx{};
    currFunc = new IRFunction{ e.functionId, static_cast<FunctionType*>(e.type) };

    BasicBlock* entry = makeBlock();
    // entry has no preds!

    currFunc->entry = entry;
    currCtx->currBlock = entry;

    // params
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

    // upvalues
    for (int i = 0; i < e.upvalues.size(); i++) {
        IRValue v = makeValue(e.upvalues[i].symbol->type);

        currCtx->upvalues[e.upvalues[i].symbol] = v;

        IRInstr instr {
            .op = IROp::UPVALUE,
            .dst = v,
            .imm = i
        };

        currCtx->currBlock->code.push_back(instr);
    }

    // body
    e.body->accept(*this);

    // implicit return
    if (!hasTerminator(currCtx->currBlock)) {
        currCtx->currBlock->term = ReturnTerm{};
    }

    functions.push_back(currFunc);

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
    // TODO
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
    if (s.expr) {
        s.expr->accept(*this);
    } else {
        throw KMYCompileError("Uninit expr not yet.");
    }

    currCtx->locals[s.symbol] = getLastValue();
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