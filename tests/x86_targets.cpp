// Assembly-only ABI regression: never executes emitted KMY code.
#include "../X86Codegen/x86Builder.hpp"
#include <sstream>
#include <stdexcept>

static void require(const std::string& text, const std::string& fragment) {
    if (text.find(fragment) == std::string::npos) {
        throw std::runtime_error("Missing target assembly: " + fragment);
    }
}

static std::string build(X86Target target) {
    StringPool pool;
    pool.intern("ABI");
    MIRBlock block;
    block.id = 0;
    block.term = ReturnTerm{};
    auto value = [](int id) { return IRValue{id, &Types::INT_TYPE}; };
    block.code = {
        {.op = MIROp::PARAM, .dst = value(0), .imm = 0},
        {.op = MIROp::PARAM, .dst = value(1), .imm = 4},
        {.op = MIROp::PARAM, .dst = value(2), .imm = 6},
        {.op = MIROp::ALLOC, .dst = value(3), .imm = 16},
        {.op = MIROp::ALLOC_SHARED, .dst = value(4), .imm = 8},
        {.op = MIROp::RETAIN, .args = {value(4)}},
        {.op = MIROp::RELEASE, .args = {value(4)}},
        {.op = MIROp::MALLOC_BYTES, .dst = value(5), .args = {value(0)}},
        {.op = MIROp::FREE, .args = {value(5)}},
        {.op = MIROp::ALLOC_DYNAMIC, .dst = value(6), .args = {value(0)}, .imm = 8},
        {.op = MIROp::RUNTIME_CALL, .args = {value(0)}, .imm = 0},
        {.op = MIROp::CALL, .dst = value(7),
            .args = {value(3), value(0), value(1), value(2), value(4), value(5), value(6), value(0), value(1)}}
    };
    MIRFunction function;
    function.functionId = 1;
    function.entry = &block;
    function.blocks = {&block};
    function.lastValueId = 8;
    std::ostringstream assembly;
    X86Builder({&function}, assembly, pool, 1, target).build();
    return assembly.str();
}

int main() {
    const auto windows = build(X86Target::Windows);
    const auto linux = build(X86Target::Linux);
    require(windows, ".section .rdata");
    require(windows, "mov [rbp-8], rcx");
    require(windows, "mov rax, [rbp+48]");
    require(windows, "mov rax, [rbp+64]");
    require(windows, "sub rsp, 80"); // Five stack args + 32-byte shadow, aligned.
    require(windows, "mov [rsp+32], r10");
    require(windows, "mov rcx, 1");
    require(windows, "xor rdx, rdx");
    require(linux, ".section .rodata");
    require(linux, "mov [rbp-8], rdi");
    require(linux, "mov [rbp-16], r8");
    require(linux, "mov rax, [rbp+16]");
    require(linux, "mov [rsp+0], r10");
    require(linux, "mov rdi, 1");
    require(linux, "xor rsi, rsi");
    require(linux, ".section .note.GNU-stack,\"\",@progbits");
    if (linux.find("sub rsp, 32\n    call kmy_") != std::string::npos) {
        throw std::runtime_error("Linux runtime call reserved Windows shadow space");
    }
}
