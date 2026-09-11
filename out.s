.intel_syntax noprefix
.globl main
# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 160
    mov rax, 0
    mov [rbp-8], rax
    mov rax, [rbp-8]
    mov [rbp-152], rax
    jmp block1_f0


block1_f0:
    mov rax, 4
    mov [rbp-16], rax
    mov rax, [rbp-152]
    cmp rax, [rbp-16]
    setl al
    movzx rax, al
    mov [rbp-24], rax
    mov rax, [rbp-24]
    cmp rax, 0
    jne block2_f0
    jmp block3_f0


block2_f0:
    mov rax, 0
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov [rbp-136], rax
    jmp block4_f0


block3_f0:
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


block4_f0:
    mov rax, 5
    mov [rbp-40], rax
    mov rax, [rbp-136]
    cmp rax, [rbp-40]
    setl al
    movzx rax, al
    mov [rbp-48], rax
    mov rax, [rbp-136]
    mov [rbp-144], rax
    mov rax, [rbp-48]
    cmp rax, 0
    jne block5_f0
    jmp block6_f0


block5_f0:
    mov rax, 1
    mov [rbp-56], rax
    mov rax, [rbp-136]
    add rax, [rbp-56]
    mov [rbp-64], rax
    mov rax, 2
    mov [rbp-72], rax
    mov rax, [rbp-64]
    cmp rax, [rbp-72]
    sete al
    movzx rax, al
    mov [rbp-80], rax
    mov rax, [rbp-80]
    cmp rax, 0
    jne block7_f0
    jmp block8_f0


block6_f0:
    mov rax, 1
    mov [rbp-120], rax
    mov rax, [rbp-152]
    add rax, [rbp-120]
    mov [rbp-128], rax
    mov rax, [rbp-128]
    mov [rbp-152], rax
    jmp block1_f0


block7_f0:
    mov rax, [rbp-64]
    mov [rbp-136], rax
    jmp block4_f0


block8_f0:
    mov rax, 2
    mov [rbp-88], rax
    mov rax, [rbp-152]
    cmp rax, [rbp-88]
    sete al
    movzx rax, al
    mov [rbp-96], rax
    mov rax, [rbp-96]
    cmp rax, 0
    jne block9_f0
    jmp block10_f0


block9_f0:
    mov rax, 4
    mov [rbp-104], rax
    mov rax, [rbp-64]
    cmp rax, [rbp-104]
    sete al
    movzx rax, al
    mov [rbp-112], rax
    mov rax, [rbp-112]
    cmp rax, 0
    jne block11_f0
    jmp block12_f0


block10_f0:
    mov rax, [rbp-64]
    mov [rbp-136], rax
    jmp block4_f0


block11_f0:
    mov rax, [rbp-64]
    mov [rbp-144], rax
    jmp block6_f0


block12_f0:
    jmp block10_f0


