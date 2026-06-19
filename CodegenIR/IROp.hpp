#pragma once

#include <ostream>

enum class IROp {
    GARBAGE,
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

    FUNC_LABEL, // Takes args (which are stored as upvalues) and imm (function id)
    PARAM, // Takes imm which is param_id (1-1 match from CALL's args)
    UPVALUE, // Takes imm which is upvalue_idx for the current closure (1-1 match from FUNC_LABEL's captured vars).
    CALL,
    RETURN
};

inline const char* toString(IROp op) {
    switch (op) {
        case IROp::CONST_INT:      return "CONST_INT";

        case IROp::ADD:            return "ADD";
        case IROp::SUB:            return "SUB";
        case IROp::MUL:            return "MUL";
        case IROp::DIV:            return "DIV";
        case IROp::MOD:            return "MOD";

        case IROp::EQUAL:          return "EQUAL";
        case IROp::NOT_EQUAL:      return "NOT_EQUAL";

        case IROp::LESS:           return "LESS";
        case IROp::LESS_EQUAL:     return "LESS_EQUAL";
        case IROp::GREATER:        return "GREATER";
        case IROp::GREATER_EQUAL:  return "GREATER_EQUAL";

        case IROp::LOGICAL_AND:    return "LOGICAL_AND";
        case IROp::LOGICAL_OR:     return "LOGICAL_OR";

        case IROp::BIT_AND:        return "BIT_AND";
        case IROp::BIT_OR:         return "BIT_OR";
        case IROp::BIT_XOR:        return "BIT_XOR";

        case IROp::LEFT_SHIFT:     return "LEFT_SHIFT";
        case IROp::RIGHT_SHIFT:    return "RIGHT_SHIFT";

        case IROp::PRINT:          return "PRINT";

        case IROp::FUNC_LABEL:     return "FUNC_LABEL";
        case IROp::PARAM:          return "PARAM";
        case IROp::UPVALUE:        return "UPVALUE";
        case IROp::CALL:           return "CALL";
        case IROp::RETURN:         return "RETURN";
    }

    return "UNKNOWN";
}

inline std::ostream& operator<<(std::ostream& os, IROp op) {
    return os << toString(op);
}