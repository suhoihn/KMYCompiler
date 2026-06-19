#pragma once

#include <vector>
#include "../Core/type.hpp"
#include "BasicBlock.hpp"
#include "../Utils/SymbolPrinter.hpp"

struct IRFunction {
    //VarSymbol* funcSymbol = nullptr;
    int functionId = INVALID_SLOT;
    FunctionType* funcType = nullptr;
    std::vector<BasicBlock*> blocks;
    BasicBlock* entry = nullptr;
};

inline std::ostream& operator<<(std::ostream& os, const IRFunction& fn) {
    os << "Function " << fn.functionId << " (type signature: " << typeToString(fn.funcType) << ")\n";
    os << "--------------------------------\n";

    for (const auto* bb : fn.blocks) {
        if (bb == fn.entry) {
            os << "(entry)\n";
        }
        os << *bb;

        os << "\n";
    }

    return os;
}