#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "IROp.hpp"
#include "IRValue.hpp"

struct IRInstr {
    IROp op;

    std::optional<IRValue> dst;

    std::vector<IRValue> args;
    std::vector<IRCapture> captures;

    int64_t imm;
};

inline std::ostream& operator<<(std::ostream& os, const IRInstr& instr) {
    switch (instr.op) {
        case IROp::CONST_INT:
            os << instr.dst.value() // Guaranteed to have one
               << " = const "
               << instr.imm;
            break;

            
        case IROp::PRINT:
            os << "print "
               << instr.args[0];
            break;

        case IROp::FUNC_LABEL: {
            os << instr.dst.value()
            << " = func_label (func id: "
            << instr.imm
            << ")";

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

            break;
        }
        
        case IROp::PARAM:
            os << instr.dst.value() 
               << " = param "
               << instr.imm;
            break;
        
        case IROp::UPVALUE:
            os << instr.dst.value() 
               << " = upvalue "
               << instr.imm;
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