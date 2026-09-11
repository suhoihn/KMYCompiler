.intel_syntax noprefix
.globl main
# entry to function 1
block0_f1:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-24], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-32], rax
    mov rax, [rbp-24]
    cmp rax, [rbp-32]
    setge al
    movzx rax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    cmp rax, 0
    jne block1_f1
    jmp block2_f1


block1_f1:
    mov rax, 0
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rsp, rbp
    pop rbp
    ret


block2_f1:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-56], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-64], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-64]
    mov rdx, [rbp-56]
    mov [rdx+rcx*8], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-72], rax
    mov rax, 1
    mov [rbp-80], rax
    mov rax, [rbp-72]
    add rax, [rbp-80]
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, 1
    mov [rbp-96], rax
    mov rax, [rbp-96]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 2
block0_f2:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    mov rax, 0
    mov [rbp-24], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-24]
    setl al
    movzx rax, al
    mov [rbp-32], rax
    mov rax, [rbp-32]
    cmp rax, 0
    jne block3_f2
    jmp block4_f2


block1_f2:
    mov rax, 1
    mov [rbp-72], rax
    mov rax, [rbp-72]
    neg rax
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov rsp, rbp
    pop rbp
    ret


block2_f2:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-16]
    mov rax, [rax+rcx*8]
    mov [rbp-96], rax
    mov rax, [rbp-96]
    mov rsp, rbp
    pop rbp
    ret


block3_f2:
    mov rax, 1
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov [rbp-64], rax
    jmp block5_f2


block4_f2:
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-48], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-48]
    setge al
    movzx rax, al
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov [rbp-64], rax
    jmp block5_f2


block5_f2:
    mov rax, [rbp-64]
    cmp rax, 0
    jne block1_f2
    jmp block2_f2


# entry to function 3
block0_f3:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-24], r9
    mov rax, 0
    mov [rbp-32], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-32]
    setl al
    movzx rax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    cmp rax, 0
    jne block3_f3
    jmp block4_f3


block1_f3:
    mov rax, 0
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov rsp, rbp
    pop rbp
    ret


block2_f3:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-88], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-16]
    mov rdx, [rbp-88]
    mov [rdx+rcx*8], rax
    mov rax, 1
    mov [rbp-96], rax
    mov rax, [rbp-96]
    mov rsp, rbp
    pop rbp
    ret


block3_f3:
    mov rax, 1
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov [rbp-72], rax
    jmp block5_f3


block4_f3:
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-56], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-56]
    setge al
    movzx rax, al
    mov [rbp-64], rax
    mov rax, [rbp-64]
    mov [rbp-72], rax
    jmp block5_f3


block5_f3:
    mov rax, [rbp-72]
    cmp rax, 0
    jne block1_f3
    jmp block2_f3


# entry to function 4
block0_f4:
    push rbp
    mov rbp, rsp
    sub rsp, 240
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    mov rax, 0
    mov [rbp-24], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-24]
    setl al
    movzx rax, al
    mov [rbp-32], rax
    mov rax, [rbp-32]
    cmp rax, 0
    jne block3_f4
    jmp block4_f4


block1_f4:
    mov rax, 1
    mov [rbp-72], rax
    mov rax, [rbp-72]
    neg rax
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov rsp, rbp
    pop rbp
    ret


block2_f4:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-16]
    mov rax, [rax+rcx*8]
    mov [rbp-96], rax
    mov rax, [rbp-16]
    mov [rbp-240], rax
    jmp block6_f4


block3_f4:
    mov rax, 1
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov [rbp-64], rax
    jmp block5_f4


block4_f4:
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-48], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-48]
    setge al
    movzx rax, al
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov [rbp-64], rax
    jmp block5_f4


block5_f4:
    mov rax, [rbp-64]
    cmp rax, 0
    jne block1_f4
    jmp block2_f4


block6_f4:
    mov rax, 1
    mov [rbp-104], rax
    mov rax, [rbp-240]
    add rax, [rbp-104]
    mov [rbp-112], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-120], rax
    mov rax, [rbp-112]
    cmp rax, [rbp-120]
    setl al
    movzx rax, al
    mov [rbp-128], rax
    mov rax, [rbp-128]
    cmp rax, 0
    jne block7_f4
    jmp block8_f4


block7_f4:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-136], rax
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-144], rax
    mov rax, 1
    mov [rbp-152], rax
    mov rax, [rbp-240]
    add rax, [rbp-152]
    mov [rbp-160], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-160]
    mov rax, [rax+rcx*8]
    mov [rbp-168], rax
    mov rax, [rbp-168]
    mov rcx, [rbp-240]
    mov rdx, [rbp-136]
    mov [rdx+rcx*8], rax
    mov rax, 1
    mov [rbp-176], rax
    mov rax, [rbp-240]
    add rax, [rbp-176]
    mov [rbp-184], rax
    mov rax, [rbp-184]
    mov [rbp-240], rax
    jmp block6_f4


block8_f4:
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-192], rax
    mov rax, 1
    mov [rbp-200], rax
    mov rax, [rbp-192]
    sub rax, [rbp-200]
    mov [rbp-208], rax
    mov rax, [rbp-208]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-216], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-224], rax
    mov rax, 0
    mov [rbp-232], rax
    mov rax, [rbp-232]
    mov rcx, [rbp-224]
    mov rdx, [rbp-216]
    mov [rdx+rcx*8], rax
    mov rax, [rbp-96]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 5
block0_f5:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, 0
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rsp, rbp
    pop rbp
    ret


# entry to function 6
block0_f6:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 7
block0_f7:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-16], rax
    mov rax, 0
    mov [rbp-24], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-24]
    sete al
    movzx rax, al
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 8
block0_f8:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    mov rax, 0
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov [rbp-104], rax
    jmp block1_f8


block1_f8:
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-32], rax
    mov rax, [rbp-104]
    cmp rax, [rbp-32]
    setl al
    movzx rax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    cmp rax, 0
    jne block2_f8
    jmp block3_f8


block2_f8:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-104]
    mov rax, [rax+rcx*8]
    mov [rbp-56], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-16]
    sete al
    movzx rax, al
    mov [rbp-64], rax
    mov rax, [rbp-64]
    cmp rax, 0
    jne block4_f8
    jmp block5_f8


block3_f8:
    mov rax, 0
    mov [rbp-96], rax
    mov rax, [rbp-96]
    mov rsp, rbp
    pop rbp
    ret


block4_f8:
    mov rax, 1
    mov [rbp-72], rax
    mov rax, [rbp-72]
    mov rsp, rbp
    pop rbp
    ret


block5_f8:
    mov rax, 1
    mov [rbp-80], rax
    mov rax, [rbp-104]
    add rax, [rbp-80]
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov [rbp-104], rax
    jmp block1_f8


# entry to function 9
block0_f9:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    mov rax, 0
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov [rbp-104], rax
    jmp block1_f9


block1_f9:
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-32], rax
    mov rax, [rbp-104]
    cmp rax, [rbp-32]
    setl al
    movzx rax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    cmp rax, 0
    jne block2_f9
    jmp block3_f9


block2_f9:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-104]
    mov rax, [rax+rcx*8]
    mov [rbp-56], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-16]
    sete al
    movzx rax, al
    mov [rbp-64], rax
    mov rax, [rbp-64]
    cmp rax, 0
    jne block4_f9
    jmp block5_f9


block3_f9:
    mov rax, 1
    mov [rbp-88], rax
    mov rax, [rbp-88]
    neg rax
    mov [rbp-96], rax
    mov rax, [rbp-96]
    mov rsp, rbp
    pop rbp
    ret


block4_f9:
    mov rax, [rbp-104]
    mov rsp, rbp
    pop rbp
    ret


block5_f9:
    mov rax, 1
    mov [rbp-72], rax
    mov rax, [rbp-104]
    add rax, [rbp-72]
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov [rbp-104], rax
    jmp block1_f9


# entry to function 11
block0_f11:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rcx, 1
    mov rdx, 128
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, 0
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, 16
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    mov rsp, rbp
    pop rbp
    ret


# entry to function 10
block0_f10:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 432
    mov rcx, 1
    mov rdx, 24
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-8], rax
    mov rax, 0
    mov [rbp-24], rax
    lea rax, [rip + block0_f11]
    mov [rbp-344], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-16], rax
    mov rax, [rbp-344]
    mov rcx, [rbp-16]
    mov [rcx], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-16]
    mov [rcx+8], rax
    mov rax, [rbp-16]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-32], rax
    mov rax, 0
    mov [rbp-48], rax
    lea rax, [rip + block0_f10]
    mov [rbp-352], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-40], rax
    mov rax, [rbp-352]
    mov rcx, [rbp-40]
    mov [rcx], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-40]
    mov [rcx+8], rax
    mov rax, [rbp-40]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-56], rax
    mov rax, 0
    mov [rbp-72], rax
    lea rax, [rip + block0_f1]
    mov [rbp-360], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-64], rax
    mov rax, [rbp-360]
    mov rcx, [rbp-64]
    mov [rcx], rax
    mov rax, [rbp-72]
    mov rcx, [rbp-64]
    mov [rcx+8], rax
    mov rax, 10
    mov [rbp-80], rax
    mov rax, [rbp-64]
    mov rcx, [rax+8]
    mov rdx, [rbp-80]
    mov r8, [rbp-56]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-88], rax
    mov rax, 0
    mov [rbp-104], rax
    lea rax, [rip + block0_f1]
    mov [rbp-368], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-96], rax
    mov rax, [rbp-368]
    mov rcx, [rbp-96]
    mov [rcx], rax
    mov rax, [rbp-104]
    mov rcx, [rbp-96]
    mov [rcx+8], rax
    mov rax, 20
    mov [rbp-112], rax
    mov rax, [rbp-96]
    mov rcx, [rax+8]
    mov rdx, [rbp-112]
    mov r8, [rbp-56]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-120], rax
    mov rax, 0
    mov [rbp-136], rax
    lea rax, [rip + block0_f1]
    mov [rbp-376], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-128], rax
    mov rax, [rbp-376]
    mov rcx, [rbp-128]
    mov [rcx], rax
    mov rax, [rbp-136]
    mov rcx, [rbp-128]
    mov [rcx+8], rax
    mov rax, 30
    mov [rbp-144], rax
    mov rax, [rbp-128]
    mov rcx, [rax+8]
    mov rdx, [rbp-144]
    mov r8, [rbp-56]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-152], rax
    mov rax, 0
    mov [rbp-168], rax
    lea rax, [rip + block0_f2]
    mov [rbp-384], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-160], rax
    mov rax, [rbp-384]
    mov rcx, [rbp-160]
    mov [rcx], rax
    mov rax, [rbp-168]
    mov rcx, [rbp-160]
    mov [rcx+8], rax
    mov rax, 1
    mov [rbp-176], rax
    mov rax, [rbp-160]
    mov rcx, [rax+8]
    mov rdx, [rbp-176]
    mov r8, [rbp-56]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-184], rax
    mov rcx, [rbp-184]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov rax, 0
    mov [rbp-200], rax
    lea rax, [rip + block0_f4]
    mov [rbp-392], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-192], rax
    mov rax, [rbp-392]
    mov rcx, [rbp-192]
    mov [rcx], rax
    mov rax, [rbp-200]
    mov rcx, [rbp-192]
    mov [rcx+8], rax
    mov rax, 1
    mov [rbp-208], rax
    mov rax, [rbp-192]
    mov rcx, [rax+8]
    mov rdx, [rbp-208]
    mov r8, [rbp-56]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-216], rax
    mov rcx, [rbp-216]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov rax, 0
    mov [rbp-232], rax
    lea rax, [rip + block0_f2]
    mov [rbp-400], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-224], rax
    mov rax, [rbp-400]
    mov rcx, [rbp-224]
    mov [rcx], rax
    mov rax, [rbp-232]
    mov rcx, [rbp-224]
    mov [rcx+8], rax
    mov rax, 1
    mov [rbp-240], rax
    mov rax, [rbp-224]
    mov rcx, [rax+8]
    mov rdx, [rbp-240]
    mov r8, [rbp-56]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-248], rax
    mov rcx, [rbp-248]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov rax, 0
    mov [rbp-264], rax
    lea rax, [rip + block0_f6]
    mov [rbp-408], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-256], rax
    mov rax, [rbp-408]
    mov rcx, [rbp-256]
    mov [rcx], rax
    mov rax, [rbp-264]
    mov rcx, [rbp-256]
    mov [rcx+8], rax
    mov rax, [rbp-256]
    mov rcx, [rax+8]
    mov rdx, [rbp-56]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-272], rax
    mov rcx, [rbp-272]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov rax, 0
    mov [rbp-288], rax
    lea rax, [rip + block0_f8]
    mov [rbp-416], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-280], rax
    mov rax, [rbp-416]
    mov rcx, [rbp-280]
    mov [rcx], rax
    mov rax, [rbp-288]
    mov rcx, [rbp-280]
    mov [rcx+8], rax
    mov rax, 30
    mov [rbp-296], rax
    mov rax, [rbp-280]
    mov rcx, [rax+8]
    mov rdx, [rbp-296]
    mov r8, [rbp-56]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-304], rax
    mov rcx, [rbp-304]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov rax, 0
    mov [rbp-320], rax
    lea rax, [rip + block0_f9]
    mov [rbp-424], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 40
    call calloc
    add rsp, 40
    mov [rbp-312], rax
    mov rax, [rbp-424]
    mov rcx, [rbp-312]
    mov [rcx], rax
    mov rax, [rbp-320]
    mov rcx, [rbp-312]
    mov [rcx+8], rax
    mov rax, 30
    mov [rbp-328], rax
    mov rax, [rbp-312]
    mov rcx, [rax+8]
    mov rdx, [rbp-328]
    mov r8, [rbp-56]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-336], rax
    mov rcx, [rbp-336]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


