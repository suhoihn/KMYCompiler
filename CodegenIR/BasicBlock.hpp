#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <ostream>
#include "IRInstr.hpp"

// Forward decl.    
struct BasicBlock;

struct JumpTerm {
    BasicBlock* target;
};

struct BranchTerm {
    IRValue cond;
    BasicBlock* trueTarget;
    BasicBlock* falseTarget;
};

struct ReturnTerm {
    std::optional<IRValue> value;
};

struct HaltTerm {};

using Terminator =
    std::variant<
        JumpTerm,
        BranchTerm,
        ReturnTerm,
        HaltTerm
>;


struct BasicBlock {
    int id;
    std::optional<Terminator> term = std::nullopt;

    std::vector<IRInstr> code;    
    
    std::vector<BasicBlock*> preds; // Optional graph
    std::vector<BasicBlock*> succs; // Optional graph
};

inline std::ostream& operator<<(std::ostream& os, const JumpTerm& t) {
    os << "jump B" << t.target->id;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const BranchTerm& t) {
    os << "branch v" << t.cond.id
       << ", B" << t.trueTarget->id
       << ", B" << t.falseTarget->id;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ReturnTerm& t) {
    os << "return";

    if (t.value) {
        os << " v" << t.value->id;
    }

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const HaltTerm& t) {
    os << "HALT";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Terminator& term) {
    std::visit([&](const auto& t) {
        os << t;
    }, term);

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const BasicBlock& bb) {
    os << "B" << bb.id << ":\n";

    for (const auto& instr : bb.code) {
        os << "    " << instr << '\n';
    }

    os << "    ";

    if (bb.term.has_value()) {
        os << bb.term.value();
    } else {
        os << "<NO TERM>";
    }

    os << '\n';

    return os;
}