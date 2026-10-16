.intel_syntax noprefix
.section .rdata
.section .bss
.align 8
kmy_globals:
.zero 16
.text
.globl main
# entry to function 1
block0_f1:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rcx, [rbp-8]
    mov rax, [rcx]
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov rax, 0
    mov [rbp-16], rax
    lea rax, [rip + block0_f1]
    mov [rbp-56], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-8], rax
    mov rax, [rbp-56]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov [rip + kmy_globals + 8], rax
    mov rax, 16
    mov [rbp-24], rax
    mov rcx, [rbp-24]
    sub rsp, 32
    call malloc
    add rsp, 32
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov [rip + kmy_globals + 0], rax
    mov rax, [rip + kmy_globals + 0]
    mov [rbp-40], rax
    mov rcx, [rbp-40]
    sub rsp, 32
    call kmy_print_int
    add rsp, 32
    mov rax, [rip + kmy_globals + 8]
    mov [rbp-48], rax
    mov rcx, [rbp-48]
    sub rsp, 32
    call kmy_print_int
    add rsp, 32
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


