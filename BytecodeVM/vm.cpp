#include "vm.hpp"

#include <stdexcept>
#include "../Core/value.hpp"
#include <iostream>

void VM::load(const std::vector<FunctionProto>& functionProtos) {
    this->functionProtos = std::move(functionProtos);
    stack.clear();
    frames.clear();
    openUpvalues.clear();
    frames.push_back(CallFrame{
        &this->functionProtos[0].chunk, // global chunk
        0, // base
        0, // ip
        nullptr, // no function object for global scope
    });
    frames[0].base = stack.size();
    std::cout << "framesize: " << this->functionProtos[0].frameSize << std::endl;
    stack.resize(stack.size() + this->functionProtos[0].frameSize); // or frameSize
}

Instruction VM::fetchInstr() {
    auto& frame = frames.back();
    if (frame.ip >= frame.chunk->code.size()) {
        throw std::runtime_error("IP out of bounds (no HALT or bad jump)");
    }
    return frame.chunk->code[frame.ip++];
}

void VM::push(Value value) {
    stack.push_back(value);
}

Value VM::pop() {
    if (stack.empty())
        throw std::runtime_error("Stack underflow");

    Value v = stack.back();
    stack.pop_back();
    return v;
}

// Every expression must leave exactly ONE value on the stack.
// (which is then popped at ExprStmt since its a statement!)
// Every statement must leave NO value on the stack.

/*
| caller locals | caller temps | callee locals | callee temps |
                              ^
                         frame.base

*/
void VM::executeInstr(const Instruction& instr) {
    auto& frame = frames.back();
    switch (instr.opcode) {

        case Opcode::PUSH_CONST: {
            push(
                constValToVal(frame.chunk->constants[instr.operand])
            );
            break;
        }

        case Opcode::POP: {
            pop();
            break;
        }

        // ----- Binary Operators -----

        case Opcode::ADD: 
        case Opcode::SUB: 
        case Opcode::MUL: 
        case Opcode::DIV:
        case Opcode::MOD: 

        case Opcode::EQUAL: 
        case Opcode::NOT_EQUAL: 
        case Opcode::LESS: 
        case Opcode::LESS_EQUAL:
        case Opcode::GREATER: 
        case Opcode::GREATER_EQUAL:

        case Opcode::LOGICAL_AND:
        case Opcode::LOGICAL_OR: 

        case Opcode::BIT_AND: 
        case Opcode::BIT_OR:
        case Opcode::BIT_XOR: 
        case Opcode::LEFT_SHIFT:
        case Opcode::RIGHT_SHIFT: {
            Value b = pop();
            Value a = pop();
            push(applyBinary(
                opcodeToBinaryOp(instr.opcode), a, b
            ));
            break;
        }

        case Opcode::BIT_NOT:
        case Opcode::LOGICAL_NOT:
        case Opcode::NEG: {
            Value a = pop();
            push(applyUnary(
                opcodeToUnaryOp(instr.opcode), a
            ));
            break;
        }
        
        
        // ----- Variables -----
        case Opcode::LOAD_LOCAL: {
            push(stack[frame.base + instr.operand]);
            break;
        }

        case Opcode::STORE_LOCAL: {
            Value v = pop();
            stack[frame.base + instr.operand] = v;
            push(v); // Assignment produces a value. Including Let stmt.
            break;
        }
        case Opcode::LOAD_UPVALUE: {
            auto up = frame.function->upvalues[instr.operand];
            push(*(up->location));
            break;
        }
        
        case Opcode::STORE_UPVALUE: {
            Value v = pop();

            auto up = frame.function->upvalues[instr.operand];
            *(up->location) = v;

            push(v);
            break;
        }
        
        case Opcode::CAPTURE_LOCAL: {
            int slot = instr.operand;

            Value* ptr = &stack[frame.base + slot];

            UpvaluePtr uv = std::make_shared<Upvalue>(
                ptr,
                nullptr,
                false
            );

            openUpvalues.push_back(uv);

            // closure being built gets this upvalue
            frame.function->upvalues.push_back(uv);
            break;
        }

        case Opcode::CAPTURE_UPVALUE: {
            int index = instr.operand;

            // parent closure is on call stack / current function
            UpvaluePtr parentUV = frame.function->upvalues[index];

            // reuse SAME upvalue object (critical!)
            frame.function->upvalues.push_back(parentUV);
            break;
        }

        case Opcode::CLOSE_UPVALUE: {
            int slot = instr.operand;

            Value* target = &stack[frame.base + slot];

            // iterate manually (safe for modification)
            for (size_t i = 0; i < openUpvalues.size(); ) {
                UpvaluePtr uv = openUpvalues[i];

                if (uv->location == target) {
                    // close it
                    uv->closed = *uv->location;
                    uv->location = &uv->closed;
                    uv->isClosed = true;

                    // remove from open list (swap-remove)
                    openUpvalues[i] = openUpvalues.back();
                    openUpvalues.pop_back();
                } else {
                    i++;
                }
            }

            break;
        }


        // ----- Control flow  -----
        case Opcode::JUMP:
            frame.ip = instr.operand;
            break;

        case Opcode::JUMP_IF_FALSE: {
            Value v = pop();
            if (!isTruthy(v)) {
                frame.ip = instr.operand;
            }
            break;
        }

        // ----- Functions -----
        case Opcode::MAKE_CLOSURE: {
            FunctionPtr fn = std::static_pointer_cast<FunctionObj>(std::get<ObjectPtr>(pop().data));

            auto closure = std::make_shared<FunctionObj>(*fn);

            // allocate upvalue slots
            closure->upvalues.resize(fn->upvalues.size());

            push(Value(closure));
            break;
        }

        case Opcode::CALL: {
            // Stack
            // [ ...other stuff ] [ callee function object ] [ arg1 ] ... [ argN ] 
            //                   ^ frame.base

            // 1. Get function from stack
            Value calleeVal = stack[stack.size() - instr.operand - 1];

            if (!std::holds_alternative<ObjectPtr>(calleeVal.data)) {
                throw std::runtime_error("Attempt to call non-function");
            }

            auto obj = std::get<ObjectPtr>(calleeVal.data);

            if (obj->kind != ObjKind::Function) {
                throw std::runtime_error("Can only call functions");
            }

            auto fn = std::static_pointer_cast<FunctionObj>(obj);

            // 2. Create new call frame
            CallFrame frame;
            frame.chunk = &fn->chunk;
            frame.ip = 0;
            frame.base = stack.size() - instr.operand - 1; // arguments are already on stack
            frame.function = fn;

            frames.push_back(frame);

            break;
        }

        case Opcode::RETURN_VOID: {
            stack.resize(frame.base);
            frames.pop_back();
            push(Value(nullptr)); // return void produces null value
            if (frames.empty()) {
                running = false; // main function returned, stop execution
            }
            break;
        }  

        case Opcode::RETURN_VALUE: {
            Value retVal = pop();
            stack.resize(frame.base);
            frames.pop_back();
            push(retVal);
            if (frames.empty()) {
                running = false; // main function returned, stop execution
            }
            break;
        }

        // ----- Arrays -----
        
        case Opcode::NEW_ARRAY: {
            auto arr = std::make_shared<ArrayObj>();

            for (int i = 0; i < instr.operand; ++i) {
                arr->array.push_back(pop());    
            }

            push(Value(arr));
            break;
        }

        case Opcode::GET_INDEX: {
            Value idx = pop();
            Value arr = pop();

            if (!checkObjType(arr, ObjKind::Array)) {
                throw std::runtime_error("Illegal Operation: Indexing non-array.");
            }
            if (!isInt(idx)) {
                throw std::runtime_error("Illegal Operation: Indexing with non-int.");
            }

            auto realArr = getArray(arr);
            int intIdx = std::get<int>(idx.data);

            if (intIdx < 0 || intIdx >= realArr->array.size()) {
                throw std::runtime_error("Index out of bounds");
            }

            //std::cout << arr.toString() << std::endl;
            //std::cout << idx.toString() << std::endl;
        
            push( realArr->array[intIdx] );

            break;
        }

        case Opcode::SET_INDEX: {
            Value idx = pop();
            Value arr = pop();
            Value val = pop();

            if (!checkObjType(arr, ObjKind::Array)) {
                throw std::runtime_error("Illegal Operation: Indexing non-array.");
            }
            if (!isInt(idx)) {
                throw std::runtime_error("Illegal Operation: Indexing with non-int.");
            }

            auto realArr = getArray(arr);
            int intIdx = std::get<int>(idx.data);

            if (intIdx < 0 || intIdx >= realArr->array.size()) {
                throw std::runtime_error("Index out of bounds");
            }

            realArr->array[intIdx] = val;

            push(val);

            break;
        }

        // ----- IO -----

        case Opcode::PRINT: {
            Value v = pop();
            std::cout << v.toString() << "\n";
            break;
        }

        case Opcode::HALT: {
            running = false;
            //ip = code.size(); // stop execution
            break;
        }

        default:
            throw std::runtime_error("Unknown opcode");
    }
}

void VM::run(void) {
    running = true;
    while (running) {
        //std::cout << "STACK SIZE: " << stack.size() << "\n";
        Instruction instr = fetchInstr();
        executeInstr(instr);
    }
}

// DEBUG
static std::string opcodeToString(Opcode op) {
    switch (op) {
        case Opcode::PUSH_CONST: return "PUSH_CONST";
        case Opcode::POP: return "POP";

        case Opcode::ADD: return "ADD";
        case Opcode::SUB: return "SUB";
        case Opcode::MUL: return "MUL";
        case Opcode::DIV: return "DIV";
        case Opcode::MOD: return "MOD";
        case Opcode::NEG: return "NEG";

        case Opcode::EQUAL: return "EQUAL";
        case Opcode::NOT_EQUAL: return "NOT_EQUAL";
        case Opcode::LESS: return "LESS";
        case Opcode::LESS_EQUAL: return "LESS_EQUAL";
        case Opcode::GREATER: return "GREATER";
        case Opcode::GREATER_EQUAL: return "GREATER_EQUAL";

        case Opcode::LOGICAL_AND: return "LOGICAL_AND";
        case Opcode::LOGICAL_OR: return "LOGICAL_OR";
        case Opcode::LOGICAL_NOT: return "LOGICAL_NOT";

        case Opcode::BIT_AND: return "BIT_AND";
        case Opcode::BIT_OR: return "BIT_OR";
        case Opcode::BIT_XOR: return "BIT_XOR";
        case Opcode::LEFT_SHIFT: return "LEFT_SHIFT";
        case Opcode::RIGHT_SHIFT: return "RIGHT_SHIFT";
        case Opcode::BIT_NOT: return "BIT_NOT";

        case Opcode::LOAD_LOCAL: return "LOAD_LOCAL";
        case Opcode::LOAD_UPVALUE: return "LOAD_UPVALUE";
        case Opcode::STORE_LOCAL: return "STORE_LOCAL";
        case Opcode::STORE_UPVALUE: return "STORE_UPVALUE";
        case Opcode::CAPTURE_LOCAL: return "CAPTURE_LOCAL";
        case Opcode::CAPTURE_UPVALUE: return "CAPTURE_UPVALUE";
        case Opcode::CLOSE_UPVALUE: return "CLOSE_UPVALUE";
        
        case Opcode::JUMP: return "JUMP";
        case Opcode::JUMP_IF_FALSE: return "JUMP_IF_FALSE";

        case Opcode::CALL: return "CALL";
        case Opcode::RETURN_VALUE: return "RETURN_VALUE";
        case Opcode::RETURN_VOID: return "RETURN_VOID";

        case Opcode::NEW_ARRAY: return "NEW_ARRAY";
        case Opcode::GET_INDEX: return "GET_INDEX";
        case Opcode::SET_INDEX: return "SET_INDEX"; 

        case Opcode::HALT: return "HALT";
        case Opcode::PRINT: return "PRINT";

        default: return "UNKNOWN";
    }
}

static std::string constValueToString(const ConstValue& v) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return "null";
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        }
        else {
            return std::to_string(arg);
        }
    }, v);
}

std::string chunkToString(const Chunk& chunk) {
    std::string out;

    out += "=== CONSTANTS ===\n";
    for (size_t i = 0; i < chunk.constants.size(); i++) {
        out += std::to_string(i) + ": " + constValueToString(chunk.constants[i]) + "\n";
    }

    out += "\n=== CODE ===\n";

    for (size_t i = 0; i < chunk.code.size(); i++) {
        const Instruction& ins = chunk.code[i];

        out += std::to_string(i) + "  ";
        out += opcodeToString(ins.opcode);

        // Only print operand if meaningful
        if (ins.opcode == Opcode::PUSH_CONST ||
            ins.opcode == Opcode::LOAD_LOCAL ||
            ins.opcode == Opcode::LOAD_UPVALUE ||
            ins.opcode == Opcode::STORE_LOCAL ||
            ins.opcode == Opcode::STORE_UPVALUE ||
            ins.opcode == Opcode::CAPTURE_LOCAL ||
            ins.opcode == Opcode::CAPTURE_UPVALUE ||
            ins.opcode == Opcode::CLOSE_UPVALUE ||
            ins.opcode == Opcode::JUMP ||
            ins.opcode == Opcode::JUMP_IF_FALSE ||
            ins.opcode == Opcode::CALL ||
            ins.opcode == Opcode::NEW_ARRAY
        ) {

            out += " " + std::to_string(ins.operand);
        }

        out += "\n";
    }

    return out;
}