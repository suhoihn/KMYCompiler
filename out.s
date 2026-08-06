.intel_syntax noprefix
.globl main
# entry to function 2
block0_f2:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov rax, 123
    mov [rbp-8], rax
    mov rcx, [rbp-8]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov rsp, rbp
    pop rbp
    ret


# entry to function 3
block0_f3:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rcx
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov rsp, rbp
    pop rbp
    ret


# entry to function 1
block0_f1:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-8], rax
    lea rax, [rip + block0_f2]
    mov [rbp-40], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-16], rax
    mov rax, [rbp-40]
    mov rcx, [rbp-16]
    mov [rcx], rax
    mov rax, [rbp-8]
    mov rcx, [rbp-16]
    mov [rcx+8], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-24], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-24]
    mov [rcx], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-8]
    mov [rcx], rax
    lea rax, [rip + block0_f3]
    mov [rbp-48], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-32], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-32]
    mov [rcx], rax
    mov rax, [rbp-8]
    mov rcx, [rbp-32]
    mov [rcx+8], rax
    mov rax, [rbp-32]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov rax, 0
    mov [rbp-16], rax
    lea rax, [rip + block0_f1]
    mov [rbp-32], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-8], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


