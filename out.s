.intel_syntax noprefix
.section .rdata
kmy_str_0:
.asciz "Red"
kmy_str_1:
.asciz "Green"
kmy_str_2:
.asciz "Blue"
.section .bss
.align 8
kmy_globals:
.zero 72
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
    sub rsp, 336
    mov rax, 0
    mov [rbp-16], rax
    lea rax, [rip + block0_f1]
    mov [rbp-336], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-8], rax
    mov rax, [rbp-336]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov [rip + kmy_globals + 64], rax
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
    mov rax, [rbp-24]
    mov [rip + kmy_globals + 0], rax
    lea rax, [rip + kmy_globals + 0]
    mov [rbp-128], rax
    mov rax, [rbp-128]
    mov [rip + kmy_globals + 8], rax
    mov rax, [rip + kmy_globals + 8]
    mov [rbp-136], rax
    mov rcx, [rbp-136]
    mov rax, [rcx]
    mov [rbp-144], rax
    mov rax, 1
    mov [rbp-152], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-152]
    imul rcx, 8
    add rax, rcx
    mov [rbp-160], rax
    mov rax, [rbp-160]
    mov [rip + kmy_globals + 16], rax
    mov rax, [rip + kmy_globals + 16]
    mov [rbp-168], rax
    mov rcx, [rbp-168]
    mov rax, [rcx]
    mov [rbp-176], rax
    mov rax, 2
    mov [rbp-184], rax
    mov rax, [rbp-176]
    mov rcx, [rbp-184]
    imul rcx, 8
    add rax, rcx
    mov [rbp-192], rax
    mov rax, [rbp-192]
    mov [rip + kmy_globals + 24], rax
    mov rax, [rip + kmy_globals + 24]
    mov [rbp-200], rax
    mov rax, 69
    mov [rbp-208], rax
    mov rcx, [rbp-200]
    mov rax, [rbp-208]
    mov [rcx], rax
    mov rax, [rip + kmy_globals + 0]
    mov [rbp-216], rax
    mov rax, 1
    mov [rbp-224], rax
    mov rax, [rbp-216]
    mov rcx, [rbp-224]
    mov rax, [rax+rcx*8]
    mov [rbp-232], rax
    mov rax, 2
    mov [rbp-240], rax
    mov rax, [rbp-232]
    mov rcx, [rbp-240]
    mov rax, [rax+rcx*8]
    mov [rbp-248], rax
    mov rcx, [rbp-248]
    sub rsp, 32
    call runtime_0
    add rsp, 32
    mov rax, 0
    mov [rbp-256], rax
    mov rax, [rbp-256]
    mov [rip + kmy_globals + 32], rax
    lea rax, [rip + kmy_globals + 32]
    mov [rbp-264], rax
    mov rax, [rbp-264]
    mov [rip + kmy_globals + 40], rax
    mov rax, 0
    mov [rbp-272], rax
    mov rax, [rbp-272]
    mov [rip + kmy_globals + 48], rax
    lea rax, [rip + kmy_globals + 48]
    mov [rbp-280], rax
    mov rax, [rbp-280]
    mov [rip + kmy_globals + 56], rax
    mov rax, [rip + kmy_globals + 40]
    mov [rbp-288], rax
    mov rax, [rip + kmy_globals + 56]
    mov [rbp-296], rax
    mov rax, [rbp-288]
    cmp rax, [rbp-296]
    sete al
    movzx rax, al
    mov [rbp-304], rax
    mov rcx, [rbp-304]
    sub rsp, 32
    call runtime_0
    add rsp, 32
    mov rax, [rip + kmy_globals + 64]
    mov [rbp-312], rax
    mov rax, [rip + kmy_globals + 40]
    mov [rbp-320], rax
    mov rax, [rbp-312]
    mov rcx, [rax+8]
    mov rdx, [rbp-320]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-328], rax
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


