#pragma once

#include <vector>
#include "../CodegenIR/BasicBlock.hpp"
#include "../CodegenIR/IRFunction.hpp"
#include "MIRInstr.hpp"

using HIRFunction = IRFunction<IRInstr>;
using MIRFunction = IRFunction<MIRInstr>;

class MIRBuilder {
private:
    std::vector<HIRFunction*> irFunctions;

    int lastValueId = 0; // Reset for every function.
    std::vector<MIRInstr> lowerHIRInstr(const IRInstr& instr);
    MIRFunction* lowerHIRFunc(HIRFunction* hirFunc);
    IRValue makeValue(Type* type);

public:
    MIRBuilder(std::vector<HIRFunction*> irFunctions);

    std::vector<MIRFunction*> lower();
};
