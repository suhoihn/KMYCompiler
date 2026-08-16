.intel_syntax noprefix
.globl main
# entry to function 5
block0_f5:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rcx
    mov rax, [rbp-8]
    mov rax, [rax+40]
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 6
block0_f6:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rcx
    mov rax, [rbp-8]
    mov rax, [rax+48]
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 7
block0_f7:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rcx
    mov rax, [rbp-8]
    mov rax, [rax+56]
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 8
block0_f8:
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
    mov rsp, rbp
    pop rbp
    ret


# entry to function 9
block0_f9:
    push rbp
    mov rbp, rsp
    sub rsp, 128
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rcx
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-16], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-24], rax
    mov rax, [rbp-8]
    mov rax, [rax+24]
    mov [rbp-32], rax
    mov rax, [rbp-8]
    mov rax, [rax+32]
    mov [rbp-40], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-56], rax
    mov rax, [rbp-24]
    mov rax, [rax]
    mov [rbp-64], rax
    mov rax, [rbp-64]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-72], rax
    mov rax, [rbp-56]
    add rax, [rbp-72]
    mov [rbp-80], rax
    mov rax, [rbp-32]
    mov rax, [rax]
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-96], rax
    mov rax, [rbp-80]
    add rax, [rbp-96]
    mov [rbp-104], rax
    mov rax, [rbp-40]
    mov rax, [rax]
    mov [rbp-112], rax
    mov rax, [rbp-112]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-120], rax
    mov rax, [rbp-104]
    add rax, [rbp-120]
    mov [rbp-128], rax
    mov rax, [rbp-128]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 4
block0_f4:
    push rbp
    mov rbp, rsp
    sub rsp, 176
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rcx
    mov rcx, 64
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-16], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-16]
    mov [rcx+40], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-16]
    mov [rcx+48], rax
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov rcx, [rbp-16]
    mov [rcx+56], rax
    lea rax, [rip + block0_f5]
    mov [rbp-136], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-48], rax
    mov rax, [rbp-136]
    mov rcx, [rbp-48]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-48]
    mov [rcx+8], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-56], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-56]
    mov [rcx], rax
    mov rax, [rbp-56]
    mov rcx, [rbp-16]
    mov [rcx+8], rax
    lea rax, [rip + block0_f6]
    mov [rbp-144], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-64], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-64]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-64]
    mov [rcx+8], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-72], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-72]
    mov [rcx], rax
    mov rax, [rbp-72]
    mov rcx, [rbp-16]
    mov [rcx+16], rax
    lea rax, [rip + block0_f7]
    mov [rbp-152], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-80], rax
    mov rax, [rbp-152]
    mov rcx, [rbp-80]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-80]
    mov [rcx+8], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-88], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-88]
    mov [rcx], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-16]
    mov [rcx+24], rax
    lea rax, [rip + block0_f8]
    mov [rbp-160], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-96], rax
    mov rax, [rbp-160]
    mov rcx, [rbp-96]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-96]
    mov [rcx+8], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-104], rax
    mov rax, [rbp-96]
    mov rcx, [rbp-104]
    mov [rcx], rax
    mov rax, [rbp-104]
    mov rcx, [rbp-16]
    mov [rcx+32], rax
    lea rax, [rip + block0_f9]
    mov [rbp-168], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-112], rax
    mov rax, [rbp-168]
    mov rcx, [rbp-112]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-112]
    mov [rcx+8], rax
    mov rax, 1000
    mov [rbp-120], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-128], rax
    mov rax, [rbp-120]
    mov rcx, [rbp-128]
    mov [rcx], rax
    mov rax, [rbp-128]
    mov rcx, [rbp-16]
    mov [rcx], rax
    mov rax, [rbp-112]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 3
block0_f3:
    push rbp
    mov rbp, rsp
    sub rsp, 80
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rcx
    mov rcx, 24
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-16], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-16]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-16]
    mov [rcx+16], rax
    lea rax, [rip + block0_f4]
    mov [rbp-72], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-40], rax
    mov rax, [rbp-72]
    mov rcx, [rbp-40]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-40]
    mov [rcx+8], rax
    mov rax, 100
    mov [rbp-48], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-56], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-56]
    mov [rcx], rax
    mov rax, [rbp-56]
    mov rcx, [rbp-16]
    mov [rcx], rax
    mov rax, [rbp-40]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-64], rax
    mov rax, [rbp-64]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 2
block0_f2:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rcx
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-16], rax
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-16]
    mov [rcx+8], rax
    lea rax, [rip + block0_f3]
    mov [rbp-64], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-32], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-32]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-32]
    mov [rcx+8], rax
    mov rax, 10
    mov [rbp-40], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-48], rax
    mov rax, [rbp-40]
    mov rcx, [rbp-48]
    mov [rcx], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-16]
    mov [rcx], rax
    mov rax, [rbp-32]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 10
block0_f10:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rcx
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-24], rax
    mov rax, 1
    mov [rbp-32], rax
    mov rax, [rbp-24]
    add rax, [rbp-32]
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov rcx, [rbp-16]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 11
block0_f11:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rcx
    mov rax, [rbp-8]
    mov rax, [rax+8]
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
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 12
block0_f12:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rcx
    mov rax, [rbp-8]
    mov rax, [rax+24]
    mov [rbp-16], rax
    mov rax, [rbp-8]
    mov rax, [rax+32]
    mov [rbp-24], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-40], rax
    mov rax, [rbp-16]
    mov rax, [rax]
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-56], rax
    mov rax, [rbp-40]
    add rax, [rbp-56]
    mov [rbp-64], rax
    mov rax, [rbp-24]
    mov rax, [rax]
    mov [rbp-72], rax
    mov rax, [rbp-72]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-80], rax
    mov rax, [rbp-64]
    add rax, [rbp-80]
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 1
block0_f1:
    push rbp
    mov rbp, rsp
    sub rsp, 144
    mov rcx, 40
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-8], rax
    lea rax, [rip + block0_f2]
    mov [rbp-112], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-16], rax
    mov rax, [rbp-112]
    mov rcx, [rbp-16]
    mov [rcx], rax
    mov rax, [rbp-8]
    mov rcx, [rbp-16]
    mov [rcx+8], rax
    lea rax, [rip + block0_f10]
    mov [rbp-120], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-24], rax
    mov rax, [rbp-120]
    mov rcx, [rbp-24]
    mov [rcx], rax
    mov rax, [rbp-8]
    mov rcx, [rbp-24]
    mov [rcx+8], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-32], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-32]
    mov [rcx], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-8]
    mov [rcx+24], rax
    lea rax, [rip + block0_f11]
    mov [rbp-128], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-40], rax
    mov rax, [rbp-128]
    mov rcx, [rbp-40]
    mov [rcx], rax
    mov rax, [rbp-8]
    mov rcx, [rbp-40]
    mov [rcx+8], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-48], rax
    mov rax, [rbp-40]
    mov rcx, [rbp-48]
    mov [rcx], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-8]
    mov [rcx+32], rax
    lea rax, [rip + block0_f12]
    mov [rbp-136], rax
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-56], rax
    mov rax, [rbp-136]
    mov rcx, [rbp-56]
    mov [rcx], rax
    mov rax, [rbp-8]
    mov rcx, [rbp-56]
    mov [rcx+8], rax
    mov rax, 1
    mov [rbp-64], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-72], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-72]
    mov [rcx], rax
    mov rax, [rbp-72]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, [rbp-16]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-80], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-88], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-88]
    mov [rcx], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, 0
    mov [rbp-96], rax
    mov rcx, 8
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-104], rax
    mov rax, [rbp-96]
    mov rcx, [rbp-104]
    mov [rcx], rax
    mov rax, [rbp-104]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    mov rax, [rbp-56]
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
    mov rcx, 16
    sub rsp, 40
    call malloc
    add rsp, 40
    mov [rbp-8], rax
    mov rax, [rbp-56]
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
    mov [rbp-32], rax
    mov rcx, [rbp-32]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov rax, [rbp-24]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-40], rax
    mov rcx, [rbp-40]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov rax, [rbp-24]
    mov rcx, [rax+8]
    sub rsp, 40
    mov r11, [rax]
    call r11
    add rsp, 40
    mov [rbp-48], rax
    mov rcx, [rbp-48]
    sub rsp, 40
    call runtime_0
    add rsp, 40
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


