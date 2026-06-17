#pragma once

#include <cstdint>
#include "SSAOp.hpp"
#include "SSAValue.hpp"

struct SSAInstr {
    SSAOp op;

    SSAValue dst;

    SSAValue src1;
    SSAValue src2;

    int64_t imm;
};

inline std::ostream& operator<<(std::ostream& os, const SSAInstr& instr) {
    switch (instr.op) {
        case SSAOp::CONST_INT:
            os << instr.dst
               << " = const "
               << instr.imm;
            break;

        case SSAOp::RETURN:
            os << "return "
               << instr.src1;
            break;
        
        case SSAOp::PRINT:
            os << "print "
               << instr.src1;
            break;

        default:
            os << instr.dst
               << " = "
               << toString(instr.op)
               << " "
               << instr.src1
               << ", "
               << instr.src2;
            break;
    }

    return os;
}