#include "MIRBuilder.hpp"

#include <unordered_map>
#include "../Core/errorhandler.hpp"
#include "TypeLayout.hpp"

MIRBuilder::MIRBuilder(std::vector<HIRFunction*> irFunctions)
    : irFunctions(irFunctions) 
{}

IRValue MIRBuilder::makeValue(Type* type) {
    return IRValue{.id = lastValueId++, .type = type};
}
std::vector<MIRInstr> MIRBuilder::lowerHIRInstr(const IRInstr& instr) {

    // SERIOUS TODO: This assumes 1:1 mapping, which is obviously false.
    // This is a temporary solution. We will need to handle cases where one IRInstr maps to multiple MIRInstrs.
    auto make = [&](MIROp op) {
        MIRInstr out;

        out.op = op;
        out.dst = instr.dst;
        out.args = instr.args;
        out.imm = instr.imm;

        return out;
    };


    switch (instr.op) {

        // =====================
        // Constants
        // =====================
        case IROp::CONST_INT:
            return { make(MIROp::CONST) };


        // =====================
        // Arithmetic
        // =====================
        case IROp::ADD:
            return { make(MIROp::ADD) };

        case IROp::SUB:
            return { make(MIROp::SUB) };

        case IROp::MUL:
            return { make(MIROp::MUL) };

        case IROp::DIV:
            return { make(MIROp::DIV) };

        case IROp::MOD:
            return { make(MIROp::MOD) };


        // =====================
        // Comparison
        // =====================
        case IROp::EQUAL:
            return { make(MIROp::CMP_EQ) };

        case IROp::NOT_EQUAL:
            return { make(MIROp::CMP_NE) };

        case IROp::LESS:
            return { make(MIROp::CMP_LT) };

        case IROp::LESS_EQUAL:
            return { make(MIROp::CMP_LE) };

        case IROp::GREATER:
            return { make(MIROp::CMP_GT) };

        case IROp::GREATER_EQUAL:
            return { make(MIROp::CMP_GE) };


        // =====================
        // Bitwise
        // =====================
        case IROp::BIT_AND:
            return { make(MIROp::AND) };

        case IROp::BIT_OR:
            return { make(MIROp::OR) };

        case IROp::BIT_XOR:
            return { make(MIROp::XOR) };

        case IROp::LEFT_SHIFT:
            return { make(MIROp::SHL) };

        case IROp::RIGHT_SHIFT:
            return { make(MIROp::SHR) };


        // =====================
        // Unary
        // =====================
        case IROp::NEG:
            return { make(MIROp::NEG) };

        case IROp::BIT_NOT:
            return { make(MIROp::NOT) };


        // =====================
        // Memory
        // =====================
        case IROp::LOAD_CELL:
            // Load from a cell (dereference)
            // arg0: cell pointer
            // dst: value loaded from cell
            return { 
                MIRInstr {
                    .op = MIROp::LOAD,
                    .dst = instr.dst,
                    .args = instr.args, // [cell_ptr]
                    .imm = 0,
                }
            };

        case IROp::STORE_CELL:
            // Store to a cell (dereference)
            // arg0: value to store
            // arg1: cell pointer
            return { 
                MIRInstr {
                    .op = MIROp::STORE,
                    .args = instr.args, // [cell_ptr, value_to_store]
                    .imm = 0,
                }
            };


        // UNUSED
        case IROp::GET_ADDR:
            return { make(MIROp::LEA) };


        // =====================
        // Calls
        // =====================
        case IROp::CALL: 
            return { make(MIROp::CALL) };


        case IROp::PARAM:
            return { 
                MIRInstr {
                    .op = MIROp::PARAM,
                    .dst = instr.dst,
                    .args = instr.args,
                    .imm = instr.imm.value() + 1, // +1 because MIR PARAM's 0th idx is for closure pointer for now. TODO
                }
            };


        case IROp::RETURN:
            return { make(MIROp::RET) };


        // =====================
        // Runtime
        // =====================
        case IROp::PRINT:
        {
            MIRInstr call;
            call.op = MIROp::RUNTIME_CALL;
            call.args = instr.args;
            call.imm = /* runtime_print id */ 0;

            return {
                call
            };
        }


        // =====================
        // Closures
        // =====================
        case IROp::ALLOC_CELL_INIT: {
            // arg0: value to store in cell
            // dst: cell pointer

            // Allocate a cell.
            MIRInstr alloc;
            alloc.op = MIROp::ALLOC;
            alloc.dst = instr.dst;
            alloc.imm = 8;

            // Populate the cell with the value.
            MIRInstr store;
            store.op = MIROp::STORE;
            store.args = {
                instr.dst.value(),
                instr.args[0]
            };
            store.imm = 0;

            return {
                alloc,
                store
            };
        }


        // These should disappear before MIR
        case IROp::LOGICAL_AND:
        case IROp::LOGICAL_OR:
        case IROp::LOGICAL_NOT:
            throw KMYCompileError(
                "Logical operation survived into MIR lowering"
            );

        case IROp::FUNC_LABEL: {
            // Arg 0: env pointer (if any)
            // Imm: function id

            // code pointer
            MIRInstr code;
            code.op = MIROp::LABEL;
            code.dst = makeValue(instr.dst->type);
            code.imm = instr.imm.value();


            // allocate closure object
            MIRInstr alloc;
            alloc.op = MIROp::ALLOC;
            alloc.dst = instr.dst;
            alloc.imm = 16; // 2 pointers


            // store code pointer
            MIRInstr storeCode;
            storeCode.op = MIROp::STORE;
            storeCode.args = {
                instr.dst.value(),
                code.dst.value()
            };
            storeCode.imm = 0;


            if (instr.args.size() > 0) {
                MIRInstr storeEnv;
                storeEnv.op = MIROp::STORE;
                storeEnv.args = {
                    instr.dst.value(), // Base address of closure object
                    instr.args[0]
                };
                storeEnv.imm = 8;
                return {
                    code,
                    alloc,
                    storeCode,
                    storeEnv
                };
            } 

            // TODO: THIS WILL NEVER HAPPEN!
            throw KMYCompileError("This should have been handled in HIR.");
            // No env pointer.
            // i.e., no env exists on my function. (Children captures nothing)
            IRValue nullValue = makeValue(&Types::INT_TYPE);
            MIRInstr nullValueInstr {
                .op = MIROp::CONST,
                .dst = nullValue,
                .imm = 0
            };

            MIRInstr storeEnv {
                .op = MIROp::STORE,
                .args = {
                    instr.dst.value(), // Base address of closure object
                    nullValue
                },
                .imm = 8
            };
            
            return {
                code,
                alloc,
                storeCode,
                nullValueInstr,
                storeEnv
            };
        }


        case IROp::ALLOC_ENV: {
            MIRInstr alloc;
            alloc.op = MIROp::ALLOC;
            alloc.dst = instr.dst;
            alloc.imm = instr.imm.value() * 8; // Assuming the cell is a pointer to the base type

            return {
                alloc
            };
        }
        case IROp::INTRODUCE_ENV: {
            // Convention. Env pointer is the first argument of the function.
            return { 
                MIRInstr {
                    .op = MIROp::PARAM,
                    .dst = instr.dst,
                    .imm = 0,
                }
            };
        }
        case IROp::SET_ENV: {
            // Arg 0: env pointer value
            // Arg 1: value to set
            // Imm: env slot index

            // format: STORE base offset value
            MIRInstr store {
                .op = MIROp::STORE,
                .args = instr.args, // [env_ptr, value_to_set]
                .imm = instr.imm.value() * 8 // Assuming 64-bit architecture, each cell is 8 bytes
            };
            return { store };
        }
        case IROp::GET_ENV: {
            // Arg 0: env pointer value
            // Imm: env slot index

            // format: LOAD base offset
            MIRInstr load {
                .op = MIROp::LOAD,
                .dst = instr.dst,
                .args = instr.args, // [env_ptr]
                .imm = instr.imm.value() * 8 // Assuming 64-bit architecture, each cell is 8 bytes
            };
            return { load };
        }


        case IROp::GARBAGE:
        default:
            throw KMYCompileError(
                "Unhandled HIR instruction"
            );
    }
}

static MIRTerm lowerTerm(
    const HIRTerm& term,
    const std::unordered_map<HIRBlock*, MIRBlock*>& blockMap
) {
    return std::visit([&](const auto& t) -> MIRTerm {

        using T = std::decay_t<decltype(t)>;

        if constexpr (std::is_same_v<T, JumpTerm<IRInstr>>) {

            return JumpTerm<MIRInstr>{
                blockMap.at(t.target)
            };

        } else if constexpr (std::is_same_v<T, BranchTerm<IRInstr>>) {

            return BranchTerm<MIRInstr>{
                t.cond,
                blockMap.at(t.trueTarget),
                blockMap.at(t.falseTarget)
            };

        } else {
            return t;
        }
    }, term);
}

MIRFunction* MIRBuilder::lowerHIRFunc(HIRFunction* hirFunc) {
    auto* mirFunc = new MIRFunction;
    mirFunc->isEntryFunc = hirFunc->isEntryFunc;
    mirFunc->functionId = hirFunc->functionId;

    // 1. Create all blocks (for map)
    std::unordered_map<HIRBlock*, MIRBlock*> blockMap;
    for (const auto& block : hirFunc->blocks) {
        auto* mirBlock = new MIRBlock;
        mirBlock->id = block->id;
        blockMap[block] = mirBlock;
        mirFunc->blocks.push_back(mirBlock);
    }

    mirFunc->entry = blockMap[hirFunc->entry];
    
    this->lastValueId = hirFunc->lastValueId;
    // 2. Fill them
    for (const auto& block : hirFunc->blocks) {
        auto* mirBlock = blockMap[block];
        for (const auto& instr : block->code) {
            auto loweredInstrs = lowerHIRInstr(instr);
            for (const auto& loweredInstr : loweredInstrs) {
                mirBlock->code.push_back(loweredInstr);
            }
        }
        if (block->term.has_value()) {
            mirBlock->term = lowerTerm(block->term.value(), blockMap);
        }

        for (const auto& succ : block->succs) {
            mirBlock->succs.push_back(blockMap[succ]);
        }
        for (const auto& pred : block->preds) {
            mirBlock->preds.push_back(blockMap[pred]);
        }
    }
    // Update MIR func's value cnt (since it may have increased).
    // Used for stack offset.
    mirFunc->lastValueId = this->lastValueId;
    return mirFunc;
}

std::vector<MIRFunction*> MIRBuilder::lower() {
    std::vector<MIRFunction*> loweredFunctions;

    for (auto* irFunc : irFunctions) {
        // Lower each IRFunction to MIRFunction
        loweredFunctions.push_back(lowerHIRFunc(irFunc));
    }
    return loweredFunctions;
}

