#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <variant>
#include "IROp.hpp"
#include "IRValue.hpp"
#include "BasicBlock.hpp"

struct IRInstr;
using HIRBlock = BasicBlock<IRInstr>;


struct IncomingPhi {
    HIRBlock* pred;
    HIROperand value;
};

struct IRInstr {
    IROp op;

    std::optional<IRValue> dst;

    std::vector<HIROperand> args;
    //std::optional<IRValue> env;

    ConstValue literal; // For actual literals

    std::optional<int64_t> imm; // For arguments of ints (e.g, env slot, func id, param id)

    std::vector<IncomingPhi> phis;
};


inline std::ostream& operator<<(std::ostream& os, const IRInstr& instr) {
    switch (instr.op) {
        case IROp::CONST_INT:
            os << instr.dst.value() // Guaranteed to have one
               << " = const "
               << instr.imm.value();
            break;

        case IROp::PHI: {
            os << instr.dst.value()
               << "phi ";

            for (size_t i = 0; i < instr.args.size(); i++) {
                os << instr.phis[i].value << "(B" << instr.phis[i].pred->id << ")";

                if (i + 1 < instr.args.size()) {
                    os << ", ";
                }
            }
        }

            
        case IROp::PRINT:
            os << "print "
               << instr.args[0];
            break;

        case IROp::FUNC_LABEL: {
            os << instr.dst.value()
            << " = func_label (func id: "
            << instr.imm.value()
            << ")";

            if (!instr.args.empty()) {
                os << " ";

                for (size_t i = 0; i < instr.args.size(); i++) {
                    os << instr.args[i];

                    if (i + 1 < instr.args.size()) {
                        os << ", ";
                    }
                }
            }

            /*
            if (!instr.captures.empty()) {
                os << " captures [";

                for (size_t i = 0; i < instr.captures.size(); ++i) {
                    const auto& cap = instr.captures[i];

                    if (i != 0)
                        os << ", ";

                    os << (cap.isLocal ? "local " : "upvalue ")
                    << cap.value;
                }

                os << "]";
            }
            */

            break;
        }
        
        case IROp::PARAM:
            os << instr.dst.value() 
               << " = param "
               << instr.imm.value();
            break;
        
        case IROp::UPVALUE:
            os << instr.dst.value() 
               << " = upvalue "
               << instr.imm.value();
            break;
        
        case IROp::ALLOC_ENV:
            os << instr.dst.value() 
               << " = alloc_env of size "
               << instr.imm.value();
            break;
        
        case IROp::SET_ENV:
            os << toString(instr.op)
               << " "
               << instr.args[0] << ", "
               << instr.imm.value() << ", "
               << instr.args[1];
            break;

        case IROp::GET_ENV:
            os << instr.dst.value() 
               << " = " << toString(instr.op) << " "
               << instr.args[0] << ", "
               << instr.imm.value();
            break;
        
        case IROp::ALLOC_CELL_INIT:
            os << instr.dst.value() 
               << " = " << toString(instr.op) << " "
               << instr.args[0];
            break;
        
        case IROp::STORE_CELL:
            os << toString(instr.op) << " "
               << instr.args[0] << ", "
               << instr.args[1];
            break;
        
        case IROp::LOAD_CELL:
            os << instr.dst.value()
               << " = " << toString(instr.op) << " "
               << instr.args[0];
            break;
            
        case IROp::RETURN: {
            os << "return ";
            if (instr.args.size() > 0) {
                os << instr.args[0];
            }
            break;
        }
            
        default: {
            if (instr.dst.has_value()) {
                os << instr.dst.value()
                << " = "
                << toString(instr.op);
            } else {
                os << toString(instr.op);
            }

            if (!instr.args.empty()) {
                os << " ";

                for (size_t i = 0; i < instr.args.size(); i++) {
                    os << instr.args[i];

                    if (i + 1 < instr.args.size()) {
                        os << ", ";
                    }
                }
            }

            break;
        }
    }

    return os;
}