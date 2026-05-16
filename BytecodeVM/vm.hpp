#pragma once

#include "../Core/value.hpp"
#include <stdexcept>

enum class Opcode {
    PUSH_CONST,   // value
    POP,

    // Arithmetic
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    NEG,

    // Comparison
    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,

    // Logical
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,

    // Bitwise
    BIT_AND,
    BIT_OR,
    BIT_XOR,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    BIT_NOT,

    // Variables
    LOAD_LOCAL,     // slot in stack
    STORE_LOCAL,    // slot in stack
    LOAD_GLOBAL,  
    STORE_GLOBAL,   
    LOAD_UPVALUE,   // slot in function->upvalues
    STORE_UPVALUE,  // slot in function->upvalues
    CAPTURE_LOCAL,
    CAPTURE_UPVALUE,
    CLOSE_UPVALUE,

    // Control flow
    JUMP,         // address
    JUMP_IF_FALSE,// address

    // Functions
    MAKE_FUNCTION,
    MAKE_CLOSURE,
    CALL,         // function argc
    RETURN_VOID,
    RETURN_VALUE,

    // Arrays
    NEW_ARRAY, // size
    GET_INDEX,
    SET_INDEX, 

    // Program end
    HALT,

    // Print the top value by popping
    PRINT,
};

struct FunctionProto {
    Chunk chunk; // The code it will run when called.
    
    int requiredParams = 0;
    int totalParams = 0;
    bool isVariadic = false;

    int upValueCnt = 0;
    
    std::vector<Chunk> defaultValues; // if ith default value is missing, run code in ith slot.
};


inline static BinaryOp opcodeToBinaryOp(Opcode op) {
    switch (op) {

        // Arithmetic
        case Opcode::ADD:           return BinaryOp::Plus;
        case Opcode::SUB:           return BinaryOp::Minus;
        case Opcode::MUL:           return BinaryOp::Star;
        case Opcode::DIV:           return BinaryOp::Slash;
        case Opcode::MOD:           return BinaryOp::Percent;

        // Comparison
        case Opcode::EQUAL:         return BinaryOp::EqualEqual;
        case Opcode::NOT_EQUAL:     return BinaryOp::NotEqual;
        case Opcode::LESS:          return BinaryOp::Less;
        case Opcode::LESS_EQUAL:    return BinaryOp::LessEqual;
        case Opcode::GREATER:       return BinaryOp::Greater;
        case Opcode::GREATER_EQUAL: return BinaryOp::GreaterEqual;

        // Logical
        case Opcode::LOGICAL_AND:   return BinaryOp::LogicalAnd;
        case Opcode::LOGICAL_OR:    return BinaryOp::LogicalOr;

        // Bitwise
        case Opcode::BIT_AND:       return BinaryOp::BitAnd;
        case Opcode::BIT_OR:        return BinaryOp::BitOr;
        case Opcode::BIT_XOR:       return BinaryOp::BitXor;
        case Opcode::LEFT_SHIFT:    return BinaryOp::LShift;
        case Opcode::RIGHT_SHIFT:   return BinaryOp::RShift;

        default:
            throw std::runtime_error("Opcode is not a BinaryOp");
    }
}

inline static UnaryOp opcodeToUnaryOp(Opcode op) {
    switch (op) {

        case Opcode::NEG:           return UnaryOp::Minus;
        case Opcode::LOGICAL_NOT:   return UnaryOp::LogicalNot;
        case Opcode::BIT_NOT:       return UnaryOp::BitNot;

        default:
            throw std::runtime_error("Opcode is not a UnaryOp");
    }
}

constexpr int UNUSED_OPERAND = -1;
struct Instruction {
    Opcode opcode;
    int operand = UNUSED_OPERAND; // -1 means unused. if this is used somehow, that's bad.
};

struct Chunk {
    std::vector<Instruction> code;
    std::vector<ConstValue> constants;
    std::vector<FunctionProto> functionProtos;

    Chunk() = default;

    Chunk(
        std::vector<Instruction> code,
        std::vector<ConstValue> constants,
        std::vector<FunctionProto> functionProtos
    ) : code(move(code)), constants(move(constants)), functionProtos(move(functionProtos)) {}
};          

struct CallFrame {
    Chunk* chunk;           // Function code source.
    size_t ip;              // instruction pointer
    int base;               // stack base for this function
    FunctionPtr function;   // closure
};


class VM {
public:
    void load(const std::vector<FunctionProto>& functionProtos);
    void run(void);

    VM() = default;
private:
    bool running = false;

    std::vector<FunctionProto> functionProtos;

    std::vector<Value> stack; // Runtime stack (main memory; RAM)
    std::vector<CallFrame> frames; // Current frame running (has its own ip and chunk)
   
    std::vector<UpvaluePtr> openUpvalues;

    // Execution
    Instruction fetchInstr(void);
    void executeInstr(const Instruction& instr);
    void push(Value value);
    Value pop();
};

std::string chunkToString(const Chunk& chunk);
