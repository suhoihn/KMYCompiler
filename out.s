.intel_syntax noprefix
.section .rdata
.section .bss
.align 8
kmy_globals:
.zero 8
.text
.globl main
# entry to function 2
block0_f2:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp-8], rdx
    mov rax, 42
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rsp, rbp
    pop rbp
    ret


# entry to function 1
block0_f1:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp-8], rdx
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 3
block0_f3:
    push rbp
    mov rbp, rsp
    sub rsp, 144
    mov rcx, 8
    xor rdx, rdx
    sub rsp, 32
    call kmy_rc_alloc
    add rsp, 32
    mov [rbp-8], rax
    mov rax, 0
    mov [rbp-24], rax
    lea rax, [rip + block0_f2]
    mov [rbp-120], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-16], rax
    mov rax, [rbp-120]
    mov rcx, [rbp-16]
    mov [rcx], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-16]
    mov [rcx+8], rax
    mov rax, [rbp-16]
    sub rsp, 32
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-32], rax
    mov rax, 0
    mov [rbp-48], rax
    lea rax, [rip + block0_f1]
    mov [rbp-128], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-40], rax
    mov rax, [rbp-128]
    mov rcx, [rbp-40]
    mov [rcx], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-40]
    mov [rcx+8], rax
    mov rax, [rbp-40]
    sub rsp, 32
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-56], rax
    mov rcx, 8
    xor rdx, rdx
    sub rsp, 32
    call kmy_rc_alloc
    add rsp, 32
    mov [rbp-64], rax
    mov rax, 0
    mov [rbp-80], rax
    lea rax, [rip + block0_f2]
    mov [rbp-136], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-72], rax
    mov rax, [rbp-136]
    mov rcx, [rbp-72]
    mov [rcx], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-72]
    mov [rcx+8], rax
    mov rax, [rbp-72]
    sub rsp, 32
    mov rcx, [rax+8]
    mov rdx, [rbp-64]
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-88], rax
    mov rax, 0
    mov [rbp-104], rax
    lea rax, [rip + block0_f1]
    mov [rbp-144], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-96], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-96]
    mov [rcx], rax
    mov rax, [rbp-104]
    mov rcx, [rbp-96]
    mov [rcx+8], rax
    mov rax, [rbp-96]
    sub rsp, 32
    mov rcx, [rax+8]
    mov rdx, [rbp-64]
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-112], rax
    mov rcx, [rbp-112]
    sub rsp, 32
    call kmy_rc_release
    add rsp, 32
    mov rcx, [rbp-112]
    sub rsp, 32
    call kmy_rc_release
    add rsp, 32
    mov rsp, rbp
    pop rbp
    ret


# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov rax, 0
    mov [rbp-16], rax
    lea rax, [rip + block0_f3]
    mov [rbp-40], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-8], rax
    mov rax, [rbp-40]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov [rip + kmy_globals + 0], rax
    mov rax, [rip + kmy_globals + 0]
    mov [rbp-24], rax
    mov rax, [rbp-24]
    sub rsp, 32
    mov rcx, [rax+8]
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-32], rax
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


