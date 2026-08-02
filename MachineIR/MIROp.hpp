#pragma once

#include <ostream>

enum class MIROp {

    // No op
    NOP,

    // Constants
    CONST,

    // Arithmetic
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,

    // Comparison
    CMP_EQ,
    CMP_NE,
    CMP_LT,
    CMP_LE,
    CMP_GT,
    CMP_GE,

    // Bitwise
    AND,
    OR,
    XOR,
    SHL,
    SHR,

    // Unary
    NEG,
    NOT,

    // Memory
    LOAD,
    STORE,
    LEA,
    ALLOC, // Allocates a heap memory with a given size (e.g., v0 = ALLOC 8)

    // Stack / calling convention
    PARAM,
    ARG,
    CALL,
    RET,

    // Control flow
    JMP,
    BRANCH,

    // Function
    LABEL,

    // Runtime calls
    RUNTIME_CALL
};


inline const char* toString(MIROp op) {

    switch(op) {

        case MIROp::NOP: return "NOP";
        case MIROp::CONST: return "CONST";

        case MIROp::ADD: return "ADD";
        case MIROp::SUB: return "SUB";
        case MIROp::MUL: return "MUL";
        case MIROp::DIV: return "DIV";
        case MIROp::MOD: return "MOD";


        case MIROp::CMP_EQ: return "CMP_EQ";
        case MIROp::CMP_NE: return "CMP_NE";
        case MIROp::CMP_LT: return "CMP_LT";
        case MIROp::CMP_LE: return "CMP_LE";
        case MIROp::CMP_GT: return "CMP_GT";
        case MIROp::CMP_GE: return "CMP_GE";


        case MIROp::AND: return "AND";
        case MIROp::OR: return "OR";
        case MIROp::XOR: return "XOR";
        case MIROp::SHL: return "SHL";
        case MIROp::SHR: return "SHR";


        case MIROp::NEG: return "NEG";
        case MIROp::NOT: return "NOT";


        case MIROp::LOAD: return "LOAD";
        case MIROp::STORE: return "STORE";
        case MIROp::LEA: return "LEA";
        case MIROp::ALLOC: return "ALLOC";


        case MIROp::PARAM: return "PARAM";
        case MIROp::ARG: return "ARG";
        case MIROp::CALL: return "CALL";
        case MIROp::RET: return "RET";


        case MIROp::JMP: return "JMP";
        case MIROp::BRANCH: return "BRANCH";


        case MIROp::LABEL: return "LABEL";


        case MIROp::RUNTIME_CALL:
            return "RUNTIME_CALL";
    }

    return "UNKNOWN";
}


inline std::ostream& operator<<(std::ostream& os, MIROp op) {
    return os << toString(op);
}
