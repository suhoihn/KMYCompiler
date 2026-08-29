#include "x86Builder.hpp"

#include <sstream>
#include <iostream>

#include "../Core/errorhandler.hpp"
X86Builder::X86Builder(
    std::vector<MIRFunction*> mirFunctions, 
    std::ostream& out
) : mirFunctions(mirFunctions), out(out) {}

static const std::vector<std::string> argRegs_linux = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };

static const std::vector<std::string> argRegs = {
    "rcx", "rdx", "r8", "r9"
};

static int mirValToStackPos(int id) {
    return (id + 1) * 8;
};

static std::string loc(const IRValue& v) {
    return "[rbp-" + std::to_string(mirValToStackPos(v.id)) + "]";
}

void X86Builder::emitIndent() {
    out << std::string(indent * 4, ' ');
}

std::string X86Builder::makeBlockLabel(int blockId) {
    return "block" + std::to_string(blockId) + "_f" + std::to_string(currFunc->functionId);
}

void X86Builder::emit(std::string s) {
    emitIndent();
    out << s << "\n";
}

void X86Builder::lowerMIRInstr(const MIRInstr& instr) {

    switch(instr.op) {

        case MIROp::NOP:
            emit("nop");
            break;


        // =========================
        // Constants
        // =========================

        case MIROp::CONST:
            emit("mov rax, " + std::to_string(instr.imm.value()));
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;


        // =========================
        // Arithmetic
        // =========================

        case MIROp::ADD:
            emit("mov rax, " + loc(instr.args[0]));
            emit("add rax, " + loc(instr.args[1]));
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;


        case MIROp::SUB:
            emit("mov rax, " + loc(instr.args[0]));
            emit("sub rax, " + loc(instr.args[1]));
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;


        case MIROp::MUL:
            emit("mov rax, " + loc(instr.args[0]));
            emit("imul rax, " + loc(instr.args[1]));
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;


        case MIROp::DIV:
            emit("mov rax, " + loc(instr.args[0]));
            emit("cqo");
            emit("idiv " + loc(instr.args[1]));
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;


        case MIROp::MOD:
            emit("mov rax, " + loc(instr.args[0]));
            emit("cqo");
            emit("idiv " + loc(instr.args[1]));
            emit("mov " + loc(instr.dst.value()) + ", rdx");
            break;


        // =========================
        // Comparison
        // =========================

        case MIROp::CMP_EQ:
        case MIROp::CMP_NE:
        case MIROp::CMP_LT:
        case MIROp::CMP_LE:
        case MIROp::CMP_GT:
        case MIROp::CMP_GE:
        {
            emit("mov rax, " + loc(instr.args[0]));
            emit("cmp rax, " + loc(instr.args[1]));

            const char* set;

            switch(instr.op) {
                case MIROp::CMP_EQ: set="sete"; break;
                case MIROp::CMP_NE: set="setne"; break;
                case MIROp::CMP_LT: set="setl"; break;
                case MIROp::CMP_LE: set="setle"; break;
                case MIROp::CMP_GT: set="setg"; break;
                case MIROp::CMP_GE: set="setge"; break;
                default: set="";
            }

            emit(std::string(set) + " al");
            emit("movzx rax, al");
            emit("mov " + loc(instr.dst.value()) + ", rax");

            break;
        }


        // =========================
        // Bitwise
        // =========================

        case MIROp::AND:
        case MIROp::OR:
        case MIROp::XOR:
        {
            emit("mov rax, " + loc(instr.args[0]));

            if(instr.op == MIROp::AND)
                emit("and rax, " + loc(instr.args[1]));

            else if(instr.op == MIROp::OR)
                emit("or rax, " + loc(instr.args[1]));

            else
                emit("xor rax, " + loc(instr.args[1]));

            emit("mov " + loc(instr.dst.value()) + ", rax");

            break;
        }


        case MIROp::SHL:
            emit("mov rax, " + loc(instr.args[0]));
            emit("mov rcx, " + loc(instr.args[1]));
            emit("shl rax, cl");
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;


        case MIROp::SHR:
            emit("mov rax, " + loc(instr.args[0]));
            emit("mov rcx, " + loc(instr.args[1]));
            emit("shr rax, cl");
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;


        // =========================
        // Unary
        // =========================

        case MIROp::NEG:
            emit("mov rax, " + loc(instr.args[0]));
            emit("neg rax");
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;


        case MIROp::NOT:
            emit("mov rax, " + loc(instr.args[0]));
            emit("not rax");
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;


        // =========================
        // Memory
        // =========================

        case MIROp::LOAD: {
            emit("mov rax, " + loc(instr.args[0]));

            int offset = instr.imm.value();

            if (offset == 0)
                emit("mov rax, [rax]");
            else
                emit("mov rax, [rax+" + std::to_string(offset) + "]");

            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;
        }


       case MIROp::STORE: {
            emit("mov rax, " + loc(instr.args[1]));
            emit("mov rcx, " + loc(instr.args[0]));

            int offset = instr.imm.value();

            if (offset == 0)
                emit("mov [rcx], rax");
            else
                emit("mov [rcx+" + std::to_string(offset) + "], rax");

            break;
        }


        case MIROp::LEA:
            emit("# TODO LEA");
            break;


        // =========================
        // Heap
        // =========================

        case MIROp::ALLOC: {
            emit("mov rcx, " + std::to_string(instr.imm.value()));
            emit("sub rsp, 40");
            emit("call malloc");
            emit("add rsp, 40");
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;

            
            /*
            // For linux
            emit("mov rdi, " + std::to_string(instr.imm.value()));
            emit("call malloc");
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;
            */
        }


        // =========================
        // Calls
        // =========================

        case MIROp::PARAM: {
            int i = instr.imm.value();
            if (i >= argRegs.size()) {
                std::cerr << "BAD argRegs index: " << i << "\n";
                std::cerr << instr << "\n";
                throw KMYCompileError("argRegs overflow");
            }

            emit("# TODO: PARAM handled in prologue by storing directly in func");
            emit("mov " + loc(instr.dst.value()) + ", " + argRegs[instr.imm.value()]);
            break;
        }


        case MIROp::ARG:
            emit("# ARG handled by CALL. This is unused tho. If you see this, call 010-2916-1277");
            break;


        case MIROp::CALL: {
            // Call gets a closure pointer as an argument.
            emit("mov rax, " + loc(instr.args[0]));

            // Fill in arguments 
            // TODO NOTE: ONLY LESS THAN 6 ARGUMENTS WORK FOR NOW.
            if (instr.args.size() > 3) {
                throw KMYCompileError("Arg >4");
            }

            // First argument is always the env pointer
            // Currently if no env exists, it passes garbage.
            emit("mov " + argRegs[0] + ", [rax+8]"); // Offset 8 is the env ptr from closure* 
            for (int i = 1; i < instr.args.size(); ++i) {
                if (i >= argRegs.size()) {
                    std::cerr << "BAD argRegs index: " << i << "\n";
                    std::cerr << instr << "\n";
                    throw KMYCompileError("argRegs overflow");
                }
                emit("mov " + argRegs[i] + ", " + loc(instr.args[i]));
            }
            
            // For linux
            // emit("call qword [rax]"); // Offset 0 is code ptr from closure*

            emit("sub rsp, 40");
            emit("mov r11, [rax]");
            emit("call r11");
            emit("add rsp, 40");

            // Return value.
            if (instr.dst.has_value()) {
                emit("mov " + loc(*instr.dst) + ", rax");
            }
            break;
        }

        case MIROp::LABEL: {
            // imm: function id

            MIRFunction* f = nullptr;
            for (const auto& mirFunc : mirFunctions)  {
                if (mirFunc->functionId == instr.imm.value()) {
                    f = mirFunc;
                    break;
                }
            }
            if (!f) { throw KMYCompileError("???"); }

            MIRBlock* entryBlock = f->entry;

            auto oldFuncId = currFunc->functionId;
            currFunc->functionId = f->functionId;

            emit("lea rax, [rip + " + makeBlockLabel(entryBlock->id) + "]");
            emit("mov " + loc(instr.dst.value()) + ", rax");

            currFunc->functionId = oldFuncId;
            break;
        }

        case MIROp::RUNTIME_CALL: {
            for (int i = 0; i < instr.args.size(); ++i) {
                emit("mov " + argRegs[i] + ", " + loc(instr.args[i]));
            }

            emit("sub rsp, 40");
            emit("call runtime_" + std::to_string(instr.imm.value()));
            emit("add rsp, 40");

            // For linux
            // emit("call runtime_" + std::to_string(instr.imm.value()));
            break;
        }

        case MIROp::MOVE: {
            emit("mov rax, " + loc(instr.args[0]));
            emit("mov " + loc(instr.dst.value()) + ", rax");
        }


        default: {
            std::ostringstream tmp;
            tmp << instr;
            emit("# unknown MIR: " + tmp.str());
            break;
        }
    }
}

void X86Builder::lowerMIRTerm(const MIRTerm& term) {
    std::visit([&](const auto& t) {

        using T = std::decay_t<decltype(t)>;

        if constexpr (std::is_same_v<T, JumpTerm<MIRInstr>>) {

            emit("jmp " + makeBlockLabel(t.target->id));

        } 
        else if constexpr (std::is_same_v<T, BranchTerm<MIRInstr>>) {

            // Move condition value to rax for comparison
            emit("mov rax, " + loc(std::get<IRValue>(t.cond)));
            // Compare it with 0 (Extract only the flags)
            emit("cmp rax, 0");
            emit("jne " + makeBlockLabel(t.trueTarget->id));
            emit("jmp " + makeBlockLabel(t.falseTarget->id));

        } 
        else if constexpr (std::is_same_v<T, ReturnTerm>) {

            if (t.value.has_value()) {
                // move return value into ABI register
                // e.g. rax
                emit("mov rax, " + loc(std::get<IRValue>(t.value.value())));
            }

            if (currFunc->isEntryFunc) {
                emit("mov eax, 0");
            }

            emit("mov rsp, rbp");
            emit("pop rbp");
            emit("ret");

        }
        else if constexpr (std::is_same_v<T, HaltTerm>) {
           emit("ret");
        }

    }, term);
}

void X86Builder::lowerMIRBlock(MIRBlock* mirBlock, MIRFunction* mirFunc) {

    bool isEntry = false;
    if (mirFunc->entry == mirBlock) {
        emit("# entry to function " + std::to_string(mirFunc->functionId));
        if (mirFunc->isEntryFunc) {
            emit("# also the entry function 'main'");
            isEntry = true;
        }
    }
    if (isEntry) {
        emit("main: ");
    } else {
        emit(makeBlockLabel(mirBlock->id) + ":");
    }
    indent++;
    if (mirFunc->entry == mirBlock) {
        
        int stackSize = mirFunc->lastValueId * 8; // Align in 8 bits.
        stackSize = (stackSize + 15) & ~15;

        // By this point, rsp (stack pointer) stores the return addr to the caller.
        // since call foo will be called from caller.

        /*
        After call foo

        | caller stuff         | 
        | arguments...(if any) |
        | return addr          | <- rsp 
        */
        
        // Save current frame pointer (rbp) on stack
        // push instr also moves the stack by 8 bytes too.
        /*
        After push rbp

        | caller stuff           | 
        | arguments...(if any)   |
        | return addr to caller  |
        | rbp for the caller     | <- rsp
        */

        emit("push rbp");
        // New frame base pointer (rbp) = rsp
        emit("mov rbp, rsp");
        // Adjust stack size
        emit("sub rsp, " + std::to_string(stackSize));

        /*
        After function entry
        | caller stuff           | 
        | arguments...(if any)   |
        | return addr to caller  |
        | rbp for the caller     | <- rbp
        | callee locals (v0...)  |
        | ...                    |
        | callee locals end      | <- rsp

        */
    }
    for (const auto& mirInstr : mirBlock->code) {
        lowerMIRInstr(mirInstr);
    }
    lowerMIRTerm(mirBlock->term.value()); // TODO: why optional?
    
    indent--;
    emit("\n");
}

void X86Builder::lowerMIRFunc(MIRFunction* mirFunc) {
    currFunc->functionId = mirFunc->functionId;
    // Allocate stack slots
    for (const auto& mirBlock : mirFunc->blocks) {
        lowerMIRBlock(mirBlock, mirFunc);
    }
}

void X86Builder::build() {
    emit(".intel_syntax noprefix");
    emit(".globl main");
    for (const auto& mirFunc : mirFunctions) {
        currFunc = mirFunc;
        lowerMIRFunc(mirFunc);
    }
}