.intel_syntax noprefix
.section .rdata
.text
.globl main
# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 208
    mov rcx, 1
    mov rdx, 24
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-8], rax
    mov rcx, 1
    mov rdx, 24
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-16], rax
    mov rax, 1
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-16]
    mov [rcx], rax
    mov rax, 2
    mov [rbp-32], rax
    mov rax, [rbp-32]
    mov rcx, [rbp-16]
    mov [rcx+8], rax
    mov rax, 3
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov rcx, [rbp-16]
    mov [rcx+16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rcx, 1
    mov rdx, 24
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-48], rax
    mov rax, 4
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov rcx, [rbp-48]
    mov [rcx], rax
    mov rax, 5
    mov [rbp-64], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-48]
    mov [rcx+8], rax
    mov rax, 6
    mov [rbp-72], rax
    mov rax, [rbp-72]
    mov rcx, [rbp-48]
    mov [rcx+16], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rcx, 1
    mov rdx, 24
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-80], rax
    mov rax, 7
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-80]
    mov [rcx], rax
    mov rax, 8
    mov [rbp-96], rax
    mov rax, [rbp-96]
    mov rcx, [rbp-80]
    mov [rcx+8], rax
    mov rax, 9
    mov [rbp-104], rax
    mov rax, [rbp-104]
    mov rcx, [rbp-80]
    mov [rcx+16], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    lea rax, [rbp-8]
    mov [rbp-112], rax
    mov rcx, [rbp-112]
    mov rax, [rcx]
    mov [rbp-120], rax
    mov rax, 1
    mov [rbp-128], rax
    mov rax, [rbp-120]
    mov rcx, [rbp-128]
    imul rcx, 8
    add rax, rcx
    mov [rbp-136], rax
    mov rcx, [rbp-136]
    mov rax, [rcx]
    mov [rbp-144], rax
    mov rax, 2
    mov [rbp-152], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-152]
    imul rcx, 8
    add rax, rcx
    mov [rbp-160], rax
    mov rax, 69
    mov [rbp-168], rax
    mov rcx, [rbp-160]
    mov rax, [rbp-168]
    mov [rcx], rax
    mov rax, 1
    mov [rbp-176], rax
    mov rax, [rbp-8]
    mov rcx, [rbp-176]
    mov rax, [rax+rcx*8]
    mov [rbp-184], rax
    mov rax, 2
    mov [rbp-192], rax
    mov rax, [rbp-184]
    mov rcx, [rbp-192]
    mov rax, [rax+rcx*8]
    mov [rbp-200], rax
    mov rcx, [rbp-200]
    sub rsp, 32
    call runtime_0
    add rsp, 32
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


