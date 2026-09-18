#pragma once

#include <vector>
#include "../CodegenIR/IRFunction.hpp"
#include "../MachineIR/MIRInstr.hpp"
#include "../MachineIR/MIRBuilder.hpp"
#include "../CodegenIR/StringPool.hpp"

class X86Builder {
private:
    std::ostream& out;
    std::vector<MIRFunction*> mirFunctions;
    const StringPool& stringPool;
    int globalSlotCount;
    int indent = 0;
    MIRFunction* currFunc = nullptr;
    // std::unordered_map<int, int> stackOffset;

    void emitIndent();
    void emit(std::string s);
    std::string makeBlockLabel(int blockId);

    void lowerMIRFunc(MIRFunction* mirFunc);
    void lowerMIRBlock(MIRBlock* mirBlock, MIRFunction* mirFunc);
    void lowerMIRTerm(const MIRTerm& term);
    void lowerMIRInstr(const MIRInstr& mirInstr);
    
public:
    X86Builder(
        std::vector<MIRFunction*> mirFunctions,
        std::ostream& out,
        const StringPool& stringPool,
        int globalSlotCount
    );

    void build();
};
