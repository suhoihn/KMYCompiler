#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <ostream>
#include "IRInstr.hpp"

// Forward decl.    
template<typename Instr>
struct BasicBlock;

template<typename Instr>
struct JumpTerm {
    BasicBlock<Instr>* target;
};

template<typename Instr>
struct BranchTerm {
    IRValue cond;
    BasicBlock<Instr>* trueTarget;
    BasicBlock<Instr>* falseTarget;
};

struct ReturnTerm {
    std::optional<IRValue> value;
};

struct HaltTerm {};

template<typename Instr>
using Terminator =
    std::variant<
        JumpTerm<Instr>,
        BranchTerm<Instr>,
        ReturnTerm,
        HaltTerm
>;

template<typename Instr>
struct BasicBlock {
    int id;
    std::optional<Terminator<Instr>> term = std::nullopt;

    std::vector<Instr> code;    
    
    std::vector<BasicBlock<Instr>*> preds; // Optional graph
    std::vector<BasicBlock<Instr>*> succs; // Optional graph
};

template<typename Instr>
inline std::ostream& operator<<(std::ostream& os, const JumpTerm<Instr>& t) {
    os << "jump B" << t.target->id;
    return os;
}

template<typename Instr>
inline std::ostream& operator<<(std::ostream& os, const BranchTerm<Instr>& t) {
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

template<typename Instr>
inline std::ostream& operator<<(std::ostream& os, const Terminator<Instr>& term) {
    std::visit([&](const auto& t) {
        os << t;
    }, term);

    return os;
}

template<typename Instr>
inline std::ostream& operator<<(std::ostream& os, const BasicBlock<Instr>& bb) {
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