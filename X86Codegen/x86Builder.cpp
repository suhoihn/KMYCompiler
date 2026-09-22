#include "x86Builder.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>

#include "../Core/errorhandler.hpp"
X86Builder::X86Builder(
    std::vector<MIRFunction*> mirFunctions, 
    std::ostream& out,
    const StringPool& stringPool,
    int globalSlotCount
) : mirFunctions(mirFunctions), stringPool(stringPool), globalSlotCount(globalSlotCount), out(out) {}

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

static std::string escapeAsmString(const std::string& value);

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

        case MIROp::CONST_STRING: {
            const int stringId = instr.imm.value();
            // RIP-relative displacement resolves to the static string address.
            emit("lea rax, [rip + kmy_str_" + std::to_string(stringId) + "]");
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;
        }

        case MIROp::STRING_EQUAL:
        case MIROp::STRING_CONCAT:
            // Lowered to RUNTIME_CALL by MIRBuilder.
            throw KMYCompileError("String operation reached x86 directly instead of runtime lowering");


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
            emit("idiv qword ptr " + loc(instr.args[1]));
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;


        case MIROp::MOD:
            emit("mov rax, " + loc(instr.args[0]));
            emit("cqo");
            emit("idiv qword ptr " + loc(instr.args[1]));
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


        case MIROp::LEA: {
            if (instr.args.empty()) {
                throw KMYCompileError("LEA requires an address operand");
            }

            if (instr.args.size() == 1 && !instr.imm.has_value()) {
                // Compute the address of a local stack slot without loading
                // the value stored in that slot.
                emit("lea rax, " + loc(instr.args[0]));
            } else if (instr.args.size() == 1) {
                // Load the base object pointer before applying a field byte
                // offset to form the field address.
                emit("mov rax, " + loc(instr.args[0]));
                emit("lea rax, [rax+" + std::to_string(instr.imm.value()) + "]");
            } else if (instr.args.size() == 2) {
                // Load the array base pointer used by an indexed lvalue.
                emit("mov rax, " + loc(instr.args[0]));
                // Load the element index used by an indexed lvalue.
                emit("mov rcx, " + loc(instr.args[1]));
                // Scale the index by the element size in bytes.
                emit("imul rcx, " + std::to_string(instr.imm.value()));
                // Add the scaled index to the base address.
                emit("add rax, rcx");
            } else {
                throw KMYCompileError("LEA received an unsupported operand shape");
            }

            // Spill the computed address into the destination MIR slot.
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;
        }

        case MIROp::LOAD_GLOBAL: {
            const int slot = instr.imm.value();
            emit("mov rax, [rip + kmy_globals + " + std::to_string(slot * 8) + "]");
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;
        }

        case MIROp::STORE_GLOBAL: {
            const int slot = instr.imm.value();
            emit("mov rax, " + loc(instr.args[0]));
            emit("mov [rip + kmy_globals + " + std::to_string(slot * 8) + "], rax");
            break;
        }

        case MIROp::LOAD_INDIRECT: {
            // Load the pointer value from its MIR stack slot.
            emit("mov rcx, " + loc(instr.args[0]));
            // Read one machine-word value through that pointer.
            emit("mov rax, [rcx]");
            // Store the loaded value in the destination MIR slot.
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;
        }

        case MIROp::STORE_INDIRECT: {
            // Load the destination pointer from its MIR stack slot.
            emit("mov rcx, " + loc(instr.args[0]));
            // Load the value that should be written through the pointer.
            emit("mov rax, " + loc(instr.args[1]));
            // Store the value at the pointed-to address.
            emit("mov [rcx], rax");
            break;
        }


        // =========================
        // Heap
        // =========================

        case MIROp::ALLOC: {
            // calloc(1, size) gives deterministic zeroed storage.
            emit("mov rcx, 1");
            emit("mov rdx, " + std::to_string(instr.imm.value()));
            // Windows x64 requires 32 bytes of caller-provided shadow space.
            emit("sub rsp, 32");
            emit("call calloc");
            emit("add rsp, 32");
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
            if (instr.args.size() > argRegs.size()) {
                throw KMYCompileError("Too many call arguments for native ABI");
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

            emit("sub rsp, 32");
            emit("mov r11, [rax]");
            emit("call r11");
            emit("add rsp, 32");

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

            emit("sub rsp, 32");
            emit("call runtime_" + std::to_string(instr.imm.value()));
            emit("add rsp, 32");

            if (instr.dst.has_value()) {
                emit("mov " + loc(instr.dst.value()) + ", rax");
            }

            // For linux
            // emit("call runtime_" + std::to_string(instr.imm.value()));
            break;
        }

        case MIROp::MOVE: {
            emit("mov rax, " + loc(instr.args[0]));
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;
        }

        case MIROp::MALLOC_BYTES: {
            // The byte count is evaluated by KMY and passed in the first
            // Windows x64 argument register. Unlike ALLOC, this calls malloc:
            // the returned bytes have no initialized value.
            emit("mov rcx, " + loc(instr.args[0]));
            emit("sub rsp, 32"); // Required Windows x64 shadow space.
            emit("call malloc");
            emit("add rsp, 32");
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;
        }

        case MIROp::FREE: {
            // Pass the raw pointer in Windows x64's first argument register.
            // C free(NULL) is defined as a no-op, so no explicit null branch.
            emit("mov rcx, " + loc(instr.args[0]));
            emit("sub rsp, 32"); // Required Windows x64 shadow space.
            emit("call free");
            emit("add rsp, 32");
            break;
        }

        case MIROp::FORCE_UNWRAP: {
            // `expr!!` is an unchecked programmer assertion. Its semantic
            // type changes from T? to T, but x86 performs only a copy: a null
            // value is allowed to continue and may fail later if dereferenced.
            emit("mov rax, " + loc(instr.args[0]));
            // To enable explicit trapping instead, uncomment this block:
            // emit("cmp rax, 0");
            // emit("jne 1f");
            // emit("ud2");
            // emit("1:");
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;
        }

        case MIROp::ALLOC_DYNAMIC: {
            // calloc(1, count * element_size) for runtime-sized arrays.
            emit("mov rax, " + loc(instr.args[0]));
            emit("imul rax, " + std::to_string(instr.imm.value()));
            emit("mov rcx, 1");
            emit("mov rdx, rax");
            // Windows x64 requires 32 bytes of caller-provided shadow space.
            emit("sub rsp, 32");
            emit("call calloc");
            emit("add rsp, 32");
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;
        }

        case MIROp::LOAD_INDEX: {
            // Load array base pointer and dynamic element index.
            emit("mov rax, " + loc(instr.args[0]));
            emit("mov rcx, " + loc(instr.args[1]));

            // Read base[index * elementSize]. Bounds checks are intentionally
            // deferred; the static array type supplies the element stride.
            emit("mov rax, [rax+rcx*" + std::to_string(instr.imm.value()) + "]");

            // Spill the loaded element into the MIR destination slot.
            emit("mov " + loc(instr.dst.value()) + ", rax");
            break;
        }

        case MIROp::STORE_INDEX: {
            // Load the value, dynamic index, and array base into scratch regs.
            emit("mov rax, " + loc(instr.args[2]));
            emit("mov rcx, " + loc(instr.args[1]));
            emit("mov rdx, " + loc(instr.args[0]));

            // Write value into base[index * elementSize].
            emit("mov [rdx+rcx*" + std::to_string(instr.imm.value()) + "], rax");
            break;
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

            // HIR/SSA branches (including `??`) become labels and jumps.
            // These are the assembly-level equivalent of if/else control
            // flow, not a second source-level AST rewrite.
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
    emit(".section .rdata");
    for (size_t i = 0; i < stringPool.size(); ++i) {
        emit("kmy_str_" + std::to_string(i) + ":");
        emit(".asciz \"" + escapeAsmString(stringPool.get(static_cast<StringId>(i))) + "\"");
    }
    if (globalSlotCount > 0) {
        emit(".section .bss");
        emit(".align 8");
        emit("kmy_globals:");
        emit(".zero " + std::to_string(globalSlotCount * 8));
    }
    emit(".text");
    emit(".globl main");
    for (const auto& mirFunc : mirFunctions) {
        currFunc = mirFunc;
        lowerMIRFunc(mirFunc);
    }
}

static std::string escapeAsmString(const std::string& value) {
    std::ostringstream escaped;
    for (unsigned char c : value) {
        switch (c) {
            case '\\': escaped << "\\\\"; break;
            case '"':  escaped << "\\\""; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default:
                if (c >= 32 && c <= 126) {
                    escaped << static_cast<char>(c);
                } else {
                    escaped << "\\" << std::oct << std::setw(3)
                            << std::setfill('0') << static_cast<int>(c)
                            << std::dec << std::setfill(' ');
                }
        }
    }
    return escaped.str();
}
