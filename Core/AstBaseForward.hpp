#pragma once
#include <memory>
#include <variant>
#include <unordered_map>
#include "type.hpp"

struct ASTNode {
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

using ExprPtr = std::shared_ptr<struct BaseExpr>;
using StmtPtr = std::shared_ptr<struct BaseStmt>;

constexpr int INVALID_SLOT = -1;

// The semantic identity of a variable.
struct Symbol {
    std::string name;
    bool isMutable;

    Type* type = nullptr;

    // Below are for debug. Each pass (or phase) has its own storage of the same info.

    // Runtime storage info for locals and upvalues (if used)
    int slot = INVALID_SLOT;

    // THOSE INFO SHOULD GO IN FN SPECIFIC CONTEXTS
    // bool captured = false;
    // int upvalueIndex = INVALID_SLOT;

    // Aggregate Info (if used)
    int fieldOffset = INVALID_SLOT;
    int methodIdx = INVALID_SLOT; // For vtables too...?

    // Function Info (if used)
    int funcProtoIdx = INVALID_SLOT;

    Symbol(const std::string& name, bool isMutable)
        : name(name), isMutable(isMutable) {}
};

struct TypeSymbol {
    std::string name;
    Type* type = nullptr;
};

using SymbolPtr = std::shared_ptr<Symbol>;


struct Local {
    SymbolPtr sym = nullptr;
    int slot = INVALID_SLOT;  // Stack slot index. For future, it can also mean virtual reg index.
    int scopeDepth = 0; // This is relative scope depth from current fnCtx.
    bool captured = false;
};

struct UpvalueInfo {
    bool isLocal;
    int index; // Slot index in parent locals OR parent upvalues
};

struct ResolvedVar {
    enum Kind {
        LOCAL, UPVALUE, GLOBAL
    } kind;

    int index;
};

struct Scope {
    Scope* parent = nullptr;
    std::unordered_map<std::string, SymbolPtr> symbols; // Symbols in this scope.
    std::unordered_map<std::string, TypeSymbol*> types;
    int depth = 0; // TODO: unused?

    Scope() = default;

    Scope(Scope* parent, int depth)
        : parent(parent), depth(depth) {}
};


enum class Opcode {
    PUSH_UNINITIALISED,
    PUSH_CONST,   // constant slot
    POP,
    DUPLICATE, // Used only for constructors for now...
    SWAP,

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

    // Annonymous records, nominal records, and classes
    MAKE_RECORD, // field count
    INIT_RECORD, // fieldInit funcProto idx
    MAKE_INSTANCE, // aggregate id
    MAKE_INSTANCE_SHARED, // aggregate id
    GET_PROPERTY, // slot
    SET_PROPERTY, // slot

    // Program end
    HALT,

    // Print the top value by popping
    PRINT,
};

// No runtime heap-allocated value. (For compiler's constantMap)
// Bascially, PRIMITIVES.
struct GarbageValue{};

struct Value;
using NativeFnPtr = Value(*)(int argc, Value* args);

using ConstValue = std::variant<
    std::nullptr_t,
    int,
    double,
    bool,
    std::string
>;

constexpr int UNUSED_OPERAND = -1;
struct Instruction {
    Opcode opcode;
    int operand = UNUSED_OPERAND; // -1 means unused. if this is used somehow, that's bad.
};

struct Chunk {
    std::vector<Instruction> code;
    std::vector<ConstValue> constants;
    //std::vector<FunctionProto> functionProtos;

    Chunk() = default;

    Chunk(
        std::vector<Instruction> code,
        std::vector<ConstValue> constants
        //std::vector<FunctionProto> functionProtos
    ) : code(move(code)), constants(move(constants)) {}
};  

struct FunctionProto {
    Chunk chunk; // The code it will run when called.
    
    int requiredParams = 0;
    int totalParams = 0;
    bool isVariadic = false;

    int upValueCnt = 0;
    std::vector<UpvalueInfo> upvalues; // For closure. The actual upvalue objects are created at runtime by CAPTURE opcodes.
    int frameSize = 0; // For stack allocation. Equals to max slot index used + 1.
    
    std::vector<Chunk> defaultValues; // if ith default value is missing, run code in ith slot.
};

struct Parameter {
    TypeNodePtr type;
    std::string name;
    bool isVariadic = false;
    bool isMutable = true; // Mutable by default
    bool defaultExists = false;
    ExprPtr defaultValue;
    SymbolPtr symbol = nullptr;

    Parameter(TypeNodePtr type, std::string name, bool isVariadic, bool isMutable, bool defaultExists, ExprPtr defaultValue=nullptr)
        : 
        type(std::move(type)),
        name(move(name)),
        isVariadic(isVariadic),
        isMutable(isMutable), 
        defaultExists(defaultExists),
        defaultValue(move(defaultValue)) {}
};