#include "SSABuilder.hpp"

#include "../Core/errorhandler.hpp"
#include <assert.h>

SSABuilder::SSABuilder(FunctionExprPtr program)
    : program(program) {}

std::vector<SSAInstr> SSABuilder::compile() {
    code.clear();
    nextId = 0;

    if (!program) {
        throw KMYCompileError("Program is null.");
    }

    program->accept(*this);

    return code;
}

inline SSAValue SSABuilder::getLastValue() { return lastValue; }
inline void SSABuilder::setLastValue(SSAValue value) { lastValue = value; }

SSAValue SSABuilder::makeValue() {
    return SSAValue{nextId++};
}

SSAValue SSABuilder::emit(SSAOp op, int constant) {
    SSAValue result = makeValue();

    SSAInstr instr;
    instr.op = op;
    instr.dst = result;
    instr.imm = constant;

    code.push_back(instr);

    return result;
}

// For print op
void SSABuilder::emitVoid(SSAOp op, SSAValue val) {
    SSAInstr instr;
    instr.op = op;
    // dst is empty.
    instr.src1 = val;
    // src2 is empty. contains garbage.

    code.push_back(instr);
}

SSAValue SSABuilder::emit(SSAOp op, SSAValue lhs, SSAValue rhs) {
    SSAValue result = makeValue();

    SSAInstr instr;
    instr.op = op;
    instr.dst = result;
    instr.src1 = lhs;
    instr.src2 = rhs;

    code.push_back(instr);

    return result;
}

// ======================================================
// Expressions
// ======================================================

void SSABuilder::visit(Literal& e) {
    ConstValue v = std::visit([](auto&& arg) -> ConstValue {
        return ConstValue(arg);
    }, e.value);

    if (!std::holds_alternative<int>(e.value)) {
        throw KMYCompileError("Only int yet sorry.");
    }

    auto val = std::get<int>(e.value);

    setLastValue( emit(SSAOp::CONST_INT, val) );
}

void SSABuilder::visit(ArrayLiteral& e) {
    throw KMYCompileError("arr Not yet");
}

void SSABuilder::visit(RecordLiteral& e) {
    throw KMYCompileError("rec Not yet");
}

void SSABuilder::visit(Variable& e) {
    // PRE: It is GUANTEED that the symbol exists.
    // This is already checked (should be...) in previous passes.
    
    auto it = locals.find(e.symbol);
    assert(it != locals.end());

    // Doesn't emit any code.
    setLastValue(it->second);
}

static SSAOp binaryOpToSSAOp(BinaryOp op) {
    switch (op) {
        case BinaryOp::Plus:          return SSAOp::ADD;
        case BinaryOp::Minus:         return SSAOp::SUB;
        case BinaryOp::Star:          return SSAOp::MUL;
        case BinaryOp::Slash:         return SSAOp::DIV;
        case BinaryOp::Percent:       return SSAOp::MOD;

        case BinaryOp::EqualEqual:    return SSAOp::EQUAL;
        case BinaryOp::NotEqual:      return SSAOp::NOT_EQUAL;
        case BinaryOp::Less:          return SSAOp::LESS;
        case BinaryOp::LessEqual:     return SSAOp::LESS_EQUAL;
        case BinaryOp::Greater:       return SSAOp::GREATER;
        case BinaryOp::GreaterEqual:  return SSAOp::GREATER_EQUAL;

        case BinaryOp::LogicalAnd:    return SSAOp::LOGICAL_AND;
        case BinaryOp::LogicalOr:     return SSAOp::LOGICAL_OR;

        case BinaryOp::BitAnd:        return SSAOp::BIT_AND;
        case BinaryOp::BitOr:         return SSAOp::BIT_OR;
        case BinaryOp::BitXor:        return SSAOp::BIT_XOR;
        case BinaryOp::LShift:        return SSAOp::LEFT_SHIFT;
        case BinaryOp::RShift:        return SSAOp::RIGHT_SHIFT;
    }

    throw KMYCompileError("Unknown BinaryOp");
}

void SSABuilder::visit(BinaryExpr& e) {
    e.left->accept(*this);
    SSAValue lhs = getLastValue();
    
    e.right->accept(*this);
    SSAValue rhs = getLastValue();

    SSAValue res = emit(binaryOpToSSAOp(e.op), lhs, rhs);
    setLastValue(res);
}

void SSABuilder::visit(UnaryExpr& e) {
    throw KMYCompileError("unary Not yet");
}

void SSABuilder::visit(Assignment& e) {
    // Case 1: variable assignment
    assert(e.left->isLValue());

    if (e.left->kind == ExprKind::Variable) {
        auto var = std::static_pointer_cast<Variable>(e.left);
        if (!var->symbol->isMutable) {
            throw KMYCompileError("Assignment to a constant variable \"" + var->name + "\"");
        }
        
        if (e.op != AssignmentOp::Assign) {
            e.left->accept(*this);
            SSAValue lhs = getLastValue();
            e.right->accept(*this);
            SSAValue rhs = getLastValue();
            
            SSAValue res = emit(binaryOpToSSAOp(compoundToBinaryOp(e.op)), lhs, rhs);
            setLastValue(res);
            return;
        } else {
            e.right->accept(*this);
            locals[var->symbol] = getLastValue();
            // last value isnt updated.
            // In a = 42, the last value is RHS.
        }
        return;
    }

    throw KMYCompileError("Invalid assignment target");
}

void SSABuilder::visit(Index& e) {
    throw KMYCompileError("index Not yet");
}

void SSABuilder::visit(Call& e) {
    throw KMYCompileError("call Not yet");
}

void SSABuilder::visit(Get& e) {
    throw KMYCompileError("get Not yet");
}

void SSABuilder::visit(ScopeAccessExpr& e) {
    throw KMYCompileError("scope acc Not yet");
}

void SSABuilder::visit(FunctionExpr& e) {
    // TODO
    e.body->accept(*this);
}

void SSABuilder::visit(ThisExpr& e) {
    throw KMYCompileError("this Not yet");
}

void SSABuilder::visit(NewExpr& e) {
    throw KMYCompileError("new Not yet");
}

// ======================================================
// Statements
// ======================================================

void SSABuilder::visit(Print& s) {
    s.expr->accept(*this);
    emitVoid(SSAOp::PRINT, getLastValue());
}

void SSABuilder::visit(If& s) {
    throw KMYCompileError("if Not yet");
}

void SSABuilder::visit(While& s) {
    throw KMYCompileError("while Not yet");
}

void SSABuilder::visit(Block& s) {
    // TODO
    for (auto& stmt : s.statements) {
        stmt->accept(*this);
    }
}

void SSABuilder::visit(Break& s) {
    throw KMYCompileError("break Not yet");
}

void SSABuilder::visit(Continue& s) {
    throw KMYCompileError("continue Not yet");
}

void SSABuilder::visit(Let& s) {
    if (s.expr) {
        s.expr->accept(*this);
    } else {
        throw KMYCompileError("Uninit expr not yet.");
    }

    locals[s.symbol] = getLastValue();
}

void SSABuilder::visit(Return& s) {
    throw KMYCompileError("return Not yet");
}

void SSABuilder::visit(Aggregate& s) {
    throw KMYCompileError("agg Not yet");
}

void SSABuilder::visit(TypeAlias& s) {}
void SSABuilder::visit(Enum& s) {}

void SSABuilder::visit(ExprStmt& s) {
    s.expr->accept(*this);
}