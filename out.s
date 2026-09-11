.intel_syntax noprefix
.globl main
# entry to function 1
block0_f1:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-16], rax
    mov rax, 1
    mov [rbp-24], rax
    mov rax, [rbp-16]
    add rax, [rbp-24]
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 3
block0_f3:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, 10
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rsp, rbp
    pop rbp
    ret


# entry to function 2
block0_f2:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 160
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-8], rax
    mov rax, 0
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, 0
    mov [rbp-32], rax
    lea rax, [rip + block0_f3]
    mov [rbp-128], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-24], rax
    mov rax, [rbp-128]
    mov rcx, [rbp-24]
    mov [rcx], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-24]
    mov [rcx+8], rax
    mov rax, [rbp-24]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-40], rax
    mov rax, 0
    mov [rbp-56], rax
    lea rax, [rip + block0_f2]
    mov [rbp-136], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-48], rax
    mov rax, [rbp-136]
    mov rcx, [rbp-48]
    mov [rcx], rax
    mov rax, [rbp-56]
    mov rcx, [rbp-48]
    mov [rcx+8], rax
    mov rax, 4
    mov [rbp-64], rax
    mov rax, [rbp-48]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-64]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-72], rax
    mov rax, 0
    mov [rbp-88], rax
    lea rax, [rip + block0_f1]
    mov [rbp-144], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-80], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-80]
    mov [rcx], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-80]
    mov [rcx+8], rax
    mov rax, [rbp-80]
    mov rcx, [rax+8]
    mov rdx, [rbp-72]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-96], rax
    mov rcx, [rbp-96]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov rax, 0
    mov [rbp-112], rax
    lea rax, [rip + block0_f1]
    mov [rbp-152], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-104], rax
    mov rax, [rbp-152]
    mov rcx, [rbp-104]
    mov [rcx], rax
    mov rax, [rbp-112]
    mov rcx, [rbp-104]
    mov [rcx+8], rax
    mov rax, [rbp-104]
    mov rcx, [rax+8]
    mov rdx, [rbp-72]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-120], rax
    mov rcx, [rbp-120]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


