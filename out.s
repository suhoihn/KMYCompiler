.intel_syntax noprefix
.section .rdata
kmy_str_0:
.asciz "Red"
kmy_str_1:
.asciz "Green"
kmy_str_2:
.asciz "Blue"
.text
.globl main
# entry to function 1
block0_f1:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rcx, [rbp-8]
    mov rax, [rcx]
    mov [rbp-16], rax
    mov rax, 0
    mov [rbp-24], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-24]
    sete al
    movzx rax, al
    mov [rbp-32], rax
    mov rax, [rbp-32]
    cmp rax, 0
    jne block1_f1
    jmp block3_f1


block1_f1:
    lea rax, [rip + kmy_str_0]
    mov [rbp-40], rax
    mov rcx, [rbp-40]
    sub rsp, 32
    call runtime_1
    add rsp, 32
    jmp block2_f1


block2_f1:
    mov rsp, rbp
    pop rbp
    ret


block3_f1:
    mov rcx, [rbp-8]
    mov rax, [rcx]
    mov [rbp-48], rax
    mov rax, 1
    mov [rbp-56], rax
    mov rax, [rbp-48]
    cmp rax, [rbp-56]
    sete al
    movzx rax, al
    mov [rbp-64], rax
    mov rax, [rbp-64]
    cmp rax, 0
    jne block4_f1
    jmp block6_f1


block4_f1:
    lea rax, [rip + kmy_str_1]
    mov [rbp-72], rax
    mov rcx, [rbp-72]
    sub rsp, 32
    call runtime_1
    add rsp, 32
    jmp block5_f1


block5_f1:
    jmp block2_f1


block6_f1:
    mov rcx, [rbp-8]
    mov rax, [rcx]
    mov [rbp-80], rax
    mov rax, 2
    mov [rbp-88], rax
    mov rax, [rbp-80]
    cmp rax, [rbp-88]
    sete al
    movzx rax, al
    mov [rbp-96], rax
    mov rax, [rbp-96]
    cmp rax, 0
    jne block7_f1
    jmp block8_f1


block7_f1:
    lea rax, [rip + kmy_str_2]
    mov [rbp-104], rax
    mov rcx, [rbp-104]
    sub rsp, 32
    call runtime_1
    add rsp, 32
    jmp block8_f1


block8_f1:
    jmp block5_f1


# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 272
    mov rax, 0
    mov [rbp-16], rax
    lea rax, [rip + block0_f1]
    mov [rbp-272], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-8], rax
    mov rax, [rbp-272]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rcx, 1
    mov rdx, 24
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-24], rax
    mov rcx, 1
    mov rdx, 24
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-32], rax
    mov rax, 1
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov rcx, [rbp-32]
    mov [rcx], rax
    mov rax, 2
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-32]
    mov [rcx+8], rax
    mov rax, 3
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov rcx, [rbp-32]
    mov [rcx+16], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-24]
    mov [rcx], rax
    mov rcx, 1
    mov rdx, 24
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-64], rax
    mov rax, 4
    mov [rbp-72], rax
    mov rax, [rbp-72]
    mov rcx, [rbp-64]
    mov [rcx], rax
    mov rax, 5
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-64]
    mov [rcx+8], rax
    mov rax, 6
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-64]
    mov [rcx+16], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-24]
    mov [rcx+8], rax
    mov rcx, 1
    mov rdx, 24
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-96], rax
    mov rax, 7
    mov [rbp-104], rax
    mov rax, [rbp-104]
    mov rcx, [rbp-96]
    mov [rcx], rax
    mov rax, 8
    mov [rbp-112], rax
    mov rax, [rbp-112]
    mov rcx, [rbp-96]
    mov [rcx+8], rax
    mov rax, 9
    mov [rbp-120], rax
    mov rax, [rbp-120]
    mov rcx, [rbp-96]
    mov [rcx+16], rax
    mov rax, [rbp-96]
    mov rcx, [rbp-24]
    mov [rcx+16], rax
    lea rax, [rbp-24]
    mov [rbp-128], rax
    mov rcx, [rbp-128]
    mov rax, [rcx]
    mov [rbp-136], rax
    mov rax, 1
    mov [rbp-144], rax
    mov rax, [rbp-136]
    mov rcx, [rbp-144]
    imul rcx, 8
    add rax, rcx
    mov [rbp-152], rax
    mov rcx, [rbp-152]
    mov rax, [rcx]
    mov [rbp-160], rax
    mov rax, 2
    mov [rbp-168], rax
    mov rax, [rbp-160]
    mov rcx, [rbp-168]
    imul rcx, 8
    add rax, rcx
    mov [rbp-176], rax
    mov rax, 69
    mov [rbp-184], rax
    mov rcx, [rbp-176]
    mov rax, [rbp-184]
    mov [rcx], rax
    mov rax, 1
    mov [rbp-192], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-192]
    mov rax, [rax+rcx*8]
    mov [rbp-200], rax
    mov rax, 2
    mov [rbp-208], rax
    mov rax, [rbp-200]
    mov rcx, [rbp-208]
    mov rax, [rax+rcx*8]
    mov [rbp-216], rax
    mov rcx, [rbp-216]
    sub rsp, 32
    call runtime_0
    add rsp, 32
    mov rax, 0
    mov [rbp-224], rax
    lea rax, [rbp-224]
    mov [rbp-232], rax
    mov rax, 0
    mov [rbp-240], rax
    lea rax, [rbp-240]
    mov [rbp-248], rax
    mov rax, [rbp-232]
    cmp rax, [rbp-248]
    sete al
    movzx rax, al
    mov [rbp-256], rax
    mov rcx, [rbp-256]
    sub rsp, 32
    call runtime_0
    add rsp, 32
    mov rax, [rbp-8]
    mov rcx, [rax+8]
    mov rdx, [rbp-232]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-264], rax
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


