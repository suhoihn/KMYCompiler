.intel_syntax noprefix
.section .rdata
.text
.globl main
# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 128
    mov rax, 1
    mov [rbp-8], rax
    mov rax, 2
    mov [rbp-16], rax
    mov rax, 9
    mov [rbp-24], rax
    mov rax, 5
    mov [rbp-32], rax
    mov rax, [rbp-24]
    sub rax, [rbp-32]
    mov [rbp-40], rax
    mov rax, [rbp-16]
    imul rax, [rbp-40]
    mov [rbp-48], rax
    mov rax, 2
    mov [rbp-56], rax
    mov rax, [rbp-48]
    cqo
    idiv qword ptr [rbp-56]
    mov [rbp-64], rax
    mov rax, [rbp-8]
    add rax, [rbp-64]
    mov [rbp-72], rax
    mov rax, 2
    mov [rbp-80], rax
    mov rax, [rbp-80]
    neg rax
    mov [rbp-88], rax
    mov rcx, [rbp-88]
    sub rsp, 32
    call malloc
    add rsp, 32
    mov [rbp-96], rax
    mov rcx, [rbp-96]
    mov rax, [rbp-72]
    mov [rcx], rax
    mov rcx, [rbp-96]
    mov rax, [rcx]
    mov [rbp-104], rax
    mov rax, 3
    mov [rbp-112], rax
    mov rax, [rbp-104]
    add rax, [rbp-112]
    mov [rbp-120], rax
    mov rcx, [rbp-96]
    mov rax, [rbp-120]
    mov [rcx], rax
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


