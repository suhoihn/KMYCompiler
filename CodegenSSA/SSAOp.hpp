#pragma once

#include <ostream>

enum class SSAOp {
    CONST_INT,

    ADD,
    SUB,
    MUL,
    DIV,
    MOD,

    EQUAL,
    NOT_EQUAL,

    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,

    LOGICAL_AND,
    LOGICAL_OR,

    BIT_AND,
    BIT_OR,
    BIT_XOR,

    LEFT_SHIFT,
    RIGHT_SHIFT,

    PRINT,
    RETURN
};

inline const char* toString(SSAOp op) {
    switch (op) {
        case SSAOp::CONST_INT:      return "CONST_INT";

        case SSAOp::ADD:            return "ADD";
        case SSAOp::SUB:            return "SUB";
        case SSAOp::MUL:            return "MUL";
        case SSAOp::DIV:            return "DIV";
        case SSAOp::MOD:            return "MOD";

        case SSAOp::EQUAL:          return "EQUAL";
        case SSAOp::NOT_EQUAL:      return "NOT_EQUAL";

        case SSAOp::LESS:           return "LESS";
        case SSAOp::LESS_EQUAL:     return "LESS_EQUAL";
        case SSAOp::GREATER:        return "GREATER";
        case SSAOp::GREATER_EQUAL:  return "GREATER_EQUAL";

        case SSAOp::LOGICAL_AND:    return "LOGICAL_AND";
        case SSAOp::LOGICAL_OR:     return "LOGICAL_OR";

        case SSAOp::BIT_AND:        return "BIT_AND";
        case SSAOp::BIT_OR:         return "BIT_OR";
        case SSAOp::BIT_XOR:        return "BIT_XOR";

        case SSAOp::LEFT_SHIFT:     return "LEFT_SHIFT";
        case SSAOp::RIGHT_SHIFT:    return "RIGHT_SHIFT";

        case SSAOp::PRINT:         return "PRINT";
        case SSAOp::RETURN:         return "RETURN";
    }

    return "UNKNOWN";
}

inline std::ostream& operator<<(std::ostream& os, SSAOp op) {
    return os << toString(op);
}