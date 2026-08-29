.intel_syntax noprefix
.globl main
# entry to function 1
block0_f1:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, 1
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov [rbp-64], rax
    # unknown MIR: v7 (int) = MOVE v1 (int)
    jmp block1_f1


block1_f1:
    mov rax, 9
    mov [rbp-24], rax
    mov rax, [rbp-64]
    cmp rax, [rbp-24]
    setle al
    movzx rax, al
    mov [rbp-32], rax
    mov rax, [rbp-32]
    cmp rax, 0
    jne block2_f1
    jmp block3_f1


block2_f1:
    mov rax, [rbp-8]
    imul rax, [rbp-64]
    mov [rbp-40], rax
    mov rcx, [rbp-40]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov rax, 1
    mov [rbp-48], rax
    mov rax, [rbp-64]
    add rax, [rbp-48]
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov [rbp-64], rax
    # unknown MIR: v7 (int) = MOVE v6 (int)
    jmp block1_f1


block3_f1:
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 80
    mov rax, 0
    mov [rbp-16], rax
    lea rax, [rip + block0_f1]
    mov [rbp-80], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-8], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, 2
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov [rbp-72], rax
    # unknown MIR: v8 (int) = MOVE v2 (int)
    jmp block1_f0


block1_f0:
    mov rax, 9
    mov [rbp-32], rax
    mov rax, [rbp-72]
    cmp rax, [rbp-32]
    setle al
    movzx rax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    cmp rax, 0
    jne block2_f0
    jmp block3_f0


block2_f0:
    mov rax, [rbp-8]
    mov rcx, [rax+8]
    mov rdx, [rbp-72]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-48], rax
    mov rax, 1
    mov [rbp-56], rax
    mov rax, [rbp-72]
    add rax, [rbp-56]
    mov [rbp-64], rax
    mov rax, [rbp-64]
    mov [rbp-72], rax
    # unknown MIR: v8 (int) = MOVE v7 (int)
    jmp block1_f0


block3_f0:
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


