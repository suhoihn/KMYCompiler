.intel_syntax noprefix
.section .rdata
kmy_str_0:
.asciz "Wassup"
.section .bss
.align 8
kmy_globals:
.zero 48
.text
.globl main
# entry to function 1
block0_f1:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov rcx, 1
    mov rdx, 8
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-8], rax
    mov rax, 42
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, 0
    mov [rbp-24], rax
    mov rax, [rbp-8]
    mov rcx, [rbp-24]
    mov rax, [rax+rcx*8]
    mov [rbp-32], rax
    mov rcx, [rbp-32]
    sub rsp, 32
    call kmy_print_int
    add rsp, 32
    mov rsp, rbp
    pop rbp
    ret


# entry to function 2
block0_f2:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov [rbp-8], rdx
    mov rcx, 1
    mov rdx, 8
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-16], rax
    mov rax, 42
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-16]
    mov [rcx], rax
    mov rax, [rbp-8]
    cmp rax, 0
    jne block1_f2
    jmp block3_f2


block1_f2:
    mov rax, 0
    mov [rbp-32], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-32]
    mov rax, [rax+rcx*8]
    mov [rbp-40], rax
    mov rcx, [rbp-40]
    sub rsp, 32
    call kmy_print_int
    add rsp, 32
    jmp block2_f2


block2_f2:
    mov rsp, rbp
    pop rbp
    ret


block3_f2:
    mov rax, 0
    mov [rbp-48], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-48]
    mov rax, [rax+rcx*8]
    mov [rbp-56], rax
    mov rcx, [rbp-56]
    sub rsp, 32
    call kmy_print_int
    add rsp, 32
    jmp block2_f2


# entry to function 3
block0_f3:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov rcx, 1
    mov rdx, 8
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-8], rax
    mov rax, 42
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, 0
    mov [rbp-24], rax
    mov rsp, rbp
    pop rbp
    ret


# entry to function 4
block0_f4:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov [rbp-8], rdx
    mov [rbp-16], r8
    mov rcx, 1
    mov rdx, 8
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-24], rax
    mov rax, 42
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-24]
    mov [rcx], rax
    jmp block1_f4


block1_f4:
    mov rax, [rbp-8]
    cmp rax, 0
    jne block2_f4
    jmp block3_f4


block2_f4:
    mov rax, [rbp-16]
    cmp rax, 0
    jne block4_f4
    jmp block6_f4


block3_f4:
    mov rsp, rbp
    pop rbp
    ret


block4_f4:
    mov rax, 0
    mov [rbp-40], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-40]
    mov rax, [rax+rcx*8]
    mov [rbp-48], rax
    mov rcx, [rbp-48]
    sub rsp, 32
    call kmy_print_int
    add rsp, 32
    jmp block3_f4


block6_f4:
    mov rax, 0
    mov [rbp-56], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-56]
    mov rax, [rax+rcx*8]
    mov [rbp-64], rax
    mov rcx, [rbp-64]
    sub rsp, 32
    call kmy_print_int
    add rsp, 32
    jmp block3_f4


# entry to function 5
block0_f5:
    push rbp
    mov rbp, rsp
    sub rsp, 144
    mov rcx, 1
    mov rdx, 8
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-8], rax
    mov rax, 42
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, 0
    mov [rbp-24], rax
    mov rax, [rbp-8]
    mov [rbp-128], rax
    mov rax, [rbp-24]
    mov [rbp-136], rax
    jmp block1_f5


block1_f5:
    mov rax, 2
    mov [rbp-32], rax
    mov rax, [rbp-136]
    cmp rax, [rbp-32]
    setl al
    movzx rax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    cmp rax, 0
    jne block2_f5
    jmp block3_f5


block2_f5:
    mov rax, 1
    mov [rbp-48], rax
    mov rax, [rbp-136]
    add rax, [rbp-48]
    mov [rbp-56], rax
    mov rax, 1
    mov [rbp-64], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-64]
    sete al
    movzx rax, al
    mov [rbp-72], rax
    mov rax, [rbp-72]
    cmp rax, 0
    jne block4_f5
    jmp block5_f5


block3_f5:
    mov rsp, rbp
    pop rbp
    ret


block4_f5:
    mov rax, 0
    mov [rbp-80], rax
    mov rax, [rbp-128]
    mov rcx, [rbp-80]
    mov rax, [rax+rcx*8]
    mov [rbp-88], rax
    mov rcx, [rbp-88]
    sub rsp, 32
    call kmy_print_int
    add rsp, 32
    mov rcx, 1
    mov rdx, 8
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-96], rax
    mov rax, 99
    mov [rbp-104], rax
    mov rax, [rbp-104]
    mov rcx, [rbp-96]
    mov [rcx], rax
    mov rax, [rbp-96]
    mov [rbp-128], rax
    mov rax, [rbp-56]
    mov [rbp-136], rax
    jmp block1_f5


block5_f5:
    mov rax, 0
    mov [rbp-112], rax
    mov rax, [rbp-128]
    mov rcx, [rbp-112]
    mov rax, [rax+rcx*8]
    mov [rbp-120], rax
    mov rcx, [rbp-120]
    sub rsp, 32
    call kmy_print_int
    add rsp, 32
    mov rax, [rbp-128]
    mov [rbp-128], rax
    mov rax, [rbp-56]
    mov [rbp-136], rax
    jmp block1_f5


# entry to function 8
block0_f8:
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


# entry to function 7
block0_f7:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp-8], rdx
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 6
block0_f6:
    push rbp
    mov rbp, rsp
    sub rsp, 80
    mov rcx, 1
    mov rdx, 8
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-8], rax
    mov rax, 0
    mov [rbp-24], rax
    lea rax, [rip + block0_f8]
    mov [rbp-72], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-16], rax
    mov rax, [rbp-72]
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
    lea rax, [rip + block0_f7]
    mov [rbp-80], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-40], rax
    mov rax, [rbp-80]
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
    mov rax, [rbp-56]
    mov rax, [rax]
    mov [rbp-64], rax
    mov rcx, [rbp-64]
    sub rsp, 32
    call kmy_print_int
    add rsp, 32
    mov rsp, rbp
    pop rbp
    ret


# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 160
    mov rax, 0
    mov [rbp-16], rax
    lea rax, [rip + block0_f1]
    mov [rbp-112], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-8], rax
    mov rax, [rbp-112]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov [rip + kmy_globals + 0], rax
    mov rax, 0
    mov [rbp-32], rax
    lea rax, [rip + block0_f2]
    mov [rbp-120], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-24], rax
    mov rax, [rbp-120]
    mov rcx, [rbp-24]
    mov [rcx], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-24]
    mov [rcx+8], rax
    mov rax, [rbp-24]
    mov [rip + kmy_globals + 8], rax
    mov rax, 0
    mov [rbp-48], rax
    lea rax, [rip + block0_f3]
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
    mov [rip + kmy_globals + 16], rax
    mov rax, 0
    mov [rbp-64], rax
    lea rax, [rip + block0_f4]
    mov [rbp-136], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-56], rax
    mov rax, [rbp-136]
    mov rcx, [rbp-56]
    mov [rcx], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-56]
    mov [rcx+8], rax
    mov rax, [rbp-56]
    mov [rip + kmy_globals + 24], rax
    mov rax, 0
    mov [rbp-80], rax
    lea rax, [rip + block0_f5]
    mov [rbp-144], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-72], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-72]
    mov [rcx], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-72]
    mov [rcx+8], rax
    mov rax, [rbp-72]
    mov [rip + kmy_globals + 32], rax
    mov rax, 0
    mov [rbp-96], rax
    lea rax, [rip + block0_f6]
    mov [rbp-152], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-88], rax
    mov rax, [rbp-152]
    mov rcx, [rbp-88]
    mov [rcx], rax
    mov rax, [rbp-96]
    mov rcx, [rbp-88]
    mov [rcx+8], rax
    mov rax, [rbp-88]
    mov [rip + kmy_globals + 40], rax
    lea rax, [rip + kmy_str_0]
    mov [rbp-104], rax
    mov rcx, [rbp-104]
    sub rsp, 32
    call kmy_print_string
    add rsp, 32
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


