.intel_syntax noprefix
.section .rdata
kmy_str_0:
.asciz " "
kmy_str_1:
.asciz "let"
kmy_str_2:
.asciz "LET"
kmy_str_3:
.asciz "const"
kmy_str_4:
.asciz "CONST"
kmy_str_5:
.asciz "fun"
kmy_str_6:
.asciz "FUN"
kmy_str_7:
.asciz "class"
kmy_str_8:
.asciz "CLASS"
kmy_str_9:
.asciz "if"
kmy_str_10:
.asciz "IF"
kmy_str_11:
.asciz "else"
kmy_str_12:
.asciz "ELSE"
kmy_str_13:
.asciz "while"
kmy_str_14:
.asciz "WHILE"
kmy_str_15:
.asciz "for"
kmy_str_16:
.asciz "FOR"
kmy_str_17:
.asciz "return"
kmy_str_18:
.asciz "RETURN"
kmy_str_19:
.asciz "new"
kmy_str_20:
.asciz "NEW"
kmy_str_21:
.asciz "true"
kmy_str_22:
.asciz "TRUE"
kmy_str_23:
.asciz "false"
kmy_str_24:
.asciz "FALSE"
kmy_str_25:
.asciz "IDENT"
kmy_str_26:
.asciz ""
kmy_str_27:
.asciz "INT"
kmy_str_28:
.asciz "EQEQ"
kmy_str_29:
.asciz "=="
kmy_str_30:
.asciz "NE"
kmy_str_31:
.asciz "!="
kmy_str_32:
.asciz "LE"
kmy_str_33:
.asciz "<="
kmy_str_34:
.asciz "GE"
kmy_str_35:
.asciz ">="
kmy_str_36:
.asciz "AND"
kmy_str_37:
.asciz "&&"
kmy_str_38:
.asciz "OR"
kmy_str_39:
.asciz "||"
kmy_str_40:
.asciz "ASSIGN"
kmy_str_41:
.asciz "="
kmy_str_42:
.asciz "PLUS"
kmy_str_43:
.asciz "+"
kmy_str_44:
.asciz "MINUS"
kmy_str_45:
.asciz "-"
kmy_str_46:
.asciz "STAR"
kmy_str_47:
.asciz "*"
kmy_str_48:
.asciz "SLASH"
kmy_str_49:
.asciz "/"
kmy_str_50:
.asciz "SEMICOLON"
kmy_str_51:
.asciz ";"
kmy_str_52:
.asciz "COMMA"
kmy_str_53:
.asciz ","
kmy_str_54:
.asciz "LEFT_PAREN"
kmy_str_55:
.asciz "("
kmy_str_56:
.asciz "RIGHT_PAREN"
kmy_str_57:
.asciz ")"
kmy_str_58:
.asciz "LEFT_BRACE"
kmy_str_59:
.asciz "{"
kmy_str_60:
.asciz "RIGHT_BRACE"
kmy_str_61:
.asciz "}"
kmy_str_62:
.asciz "UNKNOWN"
kmy_str_63:
.asciz "EOF"
kmy_str_64:
.asciz "build/kmy_lexer_input.kmy"
kmy_str_65:
.asciz "let total = 3 + 4; // comment\nif (total >= 7) { return total; }"
.text
.globl main
# entry to function 1
block0_f1:
    push rbp
    mov rbp, rsp
    sub rsp, 224
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-24], r9
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-32], rax
    mov rax, [rbp-8]
    mov rax, [rax+24]
    mov [rbp-40], rax
    mov rax, [rbp-32]
    cmp rax, [rbp-40]
    setge al
    movzx rax, al
    mov [rbp-48], rax
    mov rax, [rbp-48]
    cmp rax, 0
    jne block1_f1
    jmp block2_f1


block1_f1:
    mov rax, [rbp-8]
    mov rax, [rax+24]
    mov [rbp-56], rax
    mov rax, 2
    mov [rbp-64], rax
    mov rax, [rbp-56]
    imul rax, [rbp-64]
    mov [rbp-72], rax
    mov rax, [rbp-72]
    imul rax, 8
    mov rcx, 1
    mov rdx, rax
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-80], rax
    mov rax, [rbp-72]
    imul rax, 8
    mov rcx, 1
    mov rdx, rax
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-88], rax
    mov rax, 0
    mov [rbp-96], rax
    mov rax, [rbp-96]
    mov [rbp-224], rax
    jmp block3_f1


block2_f1:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-168], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-176], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-176]
    mov rdx, [rbp-168]
    mov [rdx+rcx*8], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-184], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-192], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-192]
    mov rdx, [rbp-184]
    mov [rdx+rcx*8], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-200], rax
    mov rax, 1
    mov [rbp-208], rax
    mov rax, [rbp-200]
    add rax, [rbp-208]
    mov [rbp-216], rax
    mov rax, [rbp-216]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    mov rsp, rbp
    pop rbp
    ret


block3_f1:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-104], rax
    mov rax, [rbp-224]
    cmp rax, [rbp-104]
    setl al
    movzx rax, al
    mov [rbp-112], rax
    mov rax, [rbp-112]
    cmp rax, 0
    jne block4_f1
    jmp block5_f1


block4_f1:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-120], rax
    mov rax, [rbp-120]
    mov rcx, [rbp-224]
    mov rax, [rax+rcx*8]
    mov [rbp-128], rax
    mov rax, [rbp-128]
    mov rcx, [rbp-224]
    mov rdx, [rbp-80]
    mov [rdx+rcx*8], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-136], rax
    mov rax, [rbp-136]
    mov rcx, [rbp-224]
    mov rax, [rax+rcx*8]
    mov [rbp-144], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-224]
    mov rdx, [rbp-88]
    mov [rdx+rcx*8], rax
    mov rax, 1
    mov [rbp-152], rax
    mov rax, [rbp-224]
    add rax, [rbp-152]
    mov [rbp-160], rax
    mov rax, [rbp-160]
    mov [rbp-224], rax
    jmp block3_f1


block5_f1:
    mov rax, [rbp-80]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, [rbp-72]
    mov rcx, [rbp-8]
    mov [rcx+24], rax
    jmp block2_f1


# entry to function 2
block0_f2:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, 0
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov [rbp-112], rax
    jmp block1_f2


block1_f2:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-24], rax
    mov rax, [rbp-112]
    cmp rax, [rbp-24]
    setl al
    movzx rax, al
    mov [rbp-32], rax
    mov rax, [rbp-32]
    cmp rax, 0
    jne block2_f2
    jmp block3_f2


block2_f2:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov rcx, [rbp-112]
    mov rax, [rax+rcx*8]
    mov [rbp-48], rax
    lea rax, [rip + kmy_str_0]
    mov [rbp-56], rax
    mov rcx, [rbp-48]
    mov rdx, [rbp-56]
    sub rsp, 32
    call runtime_3
    add rsp, 32
    mov [rbp-64], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-72], rax
    mov rax, [rbp-72]
    mov rcx, [rbp-112]
    mov rax, [rax+rcx*8]
    mov [rbp-80], rax
    mov rcx, [rbp-64]
    mov rdx, [rbp-80]
    sub rsp, 32
    call runtime_3
    add rsp, 32
    mov [rbp-88], rax
    mov rcx, [rbp-88]
    sub rsp, 32
    call runtime_1
    add rsp, 32
    mov rax, 1
    mov [rbp-96], rax
    mov rax, [rbp-112]
    add rax, [rbp-96]
    mov [rbp-104], rax
    mov rax, [rbp-104]
    mov [rbp-112], rax
    jmp block1_f2


block3_f2:
    mov rsp, rbp
    pop rbp
    ret


# entry to function 4
block0_f4:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, 4
    mov [rbp-24], rax
    mov rax, 0
    mov [rbp-32], rax
    mov rax, [rbp-24]
    add rax, [rbp-32]
    mov [rbp-40], rax
    mov rax, [rbp-40]
    imul rax, 8
    mov rcx, 1
    mov rdx, rax
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rax, 4
    mov [rbp-56], rax
    mov rax, 0
    mov [rbp-64], rax
    mov rax, [rbp-56]
    add rax, [rbp-64]
    mov [rbp-72], rax
    mov rax, [rbp-72]
    imul rax, 8
    mov rcx, 1
    mov rdx, rax
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, 0
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    mov rax, 4
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-8]
    mov [rcx+24], rax
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
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 5
block0_f5:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    mov rax, 48
    mov [rbp-24], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-24]
    setge al
    movzx rax, al
    mov [rbp-32], rax
    mov rax, [rbp-32]
    cmp rax, 0
    jne block1_f5
    jmp block2_f5


block1_f5:
    mov rax, 57
    mov [rbp-40], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-40]
    setle al
    movzx rax, al
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov [rbp-64], rax
    jmp block3_f5


block2_f5:
    mov rax, 0
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov [rbp-64], rax
    jmp block3_f5


block3_f5:
    mov rax, [rbp-64]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 6
block0_f6:
    push rbp
    mov rbp, rsp
    sub rsp, 160
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    mov rax, 65
    mov [rbp-24], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-24]
    setge al
    movzx rax, al
    mov [rbp-32], rax
    mov rax, [rbp-32]
    cmp rax, 0
    jne block1_f6
    jmp block2_f6


block1_f6:
    mov rax, 90
    mov [rbp-40], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-40]
    setle al
    movzx rax, al
    mov [rbp-48], rax
    mov rax, [rbp-48]
    mov [rbp-64], rax
    jmp block3_f6


block2_f6:
    mov rax, 0
    mov [rbp-56], rax
    mov rax, [rbp-56]
    mov [rbp-64], rax
    jmp block3_f6


block3_f6:
    mov rax, [rbp-64]
    cmp rax, 0
    jne block4_f6
    jmp block5_f6


block4_f6:
    mov rax, 1
    mov [rbp-72], rax
    mov rax, [rbp-72]
    mov [rbp-128], rax
    jmp block6_f6


block5_f6:
    mov rax, 97
    mov [rbp-80], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-80]
    setge al
    movzx rax, al
    mov [rbp-88], rax
    mov rax, [rbp-88]
    cmp rax, 0
    jne block7_f6
    jmp block8_f6


block6_f6:
    mov rax, [rbp-128]
    cmp rax, 0
    jne block10_f6
    jmp block11_f6


block7_f6:
    mov rax, 122
    mov [rbp-96], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-96]
    setle al
    movzx rax, al
    mov [rbp-104], rax
    mov rax, [rbp-104]
    mov [rbp-120], rax
    jmp block9_f6


block8_f6:
    mov rax, 0
    mov [rbp-112], rax
    mov rax, [rbp-112]
    mov [rbp-120], rax
    jmp block9_f6


block9_f6:
    mov rax, [rbp-120]
    mov [rbp-128], rax
    jmp block6_f6


block10_f6:
    mov rax, 1
    mov [rbp-136], rax
    mov rax, [rbp-136]
    mov [rbp-160], rax
    jmp block12_f6


block11_f6:
    mov rax, 95
    mov [rbp-144], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-144]
    sete al
    movzx rax, al
    mov [rbp-152], rax
    mov rax, [rbp-152]
    mov [rbp-160], rax
    jmp block12_f6


block12_f6:
    mov rax, [rbp-160]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 7
block0_f7:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-24], r9
    mov rax, 0
    mov [rbp-40], rax
    lea rax, [rip + block0_f1]
    mov [rbp-64], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-32], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-32]
    mov [rcx], rax
    mov rax, [rbp-40]
    mov rcx, [rbp-32]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov rax, [rax+24]
    mov [rbp-48], rax
    mov rax, [rbp-32]
    mov rcx, [rax+8]
    mov rdx, [rbp-48]
    mov r8, [rbp-16]
    mov r9, [rbp-24]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-56], rax
    mov rsp, rbp
    pop rbp
    ret


# entry to function 8
block0_f8:
    push rbp
    mov rbp, rsp
    sub rsp, 320
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    lea rax, [rip + kmy_str_1]
    mov [rbp-24], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-24]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-32], rax
    mov rax, [rbp-32]
    cmp rax, 0
    jne block1_f8
    jmp block2_f8


block1_f8:
    lea rax, [rip + kmy_str_2]
    mov [rbp-40], rax
    mov rax, [rbp-40]
    mov rsp, rbp
    pop rbp
    ret


block2_f8:
    lea rax, [rip + kmy_str_3]
    mov [rbp-48], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-48]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-56], rax
    mov rax, [rbp-56]
    cmp rax, 0
    jne block3_f8
    jmp block4_f8


block3_f8:
    lea rax, [rip + kmy_str_4]
    mov [rbp-64], rax
    mov rax, [rbp-64]
    mov rsp, rbp
    pop rbp
    ret


block4_f8:
    lea rax, [rip + kmy_str_5]
    mov [rbp-72], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-72]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-80], rax
    mov rax, [rbp-80]
    cmp rax, 0
    jne block5_f8
    jmp block6_f8


block5_f8:
    lea rax, [rip + kmy_str_6]
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov rsp, rbp
    pop rbp
    ret


block6_f8:
    lea rax, [rip + kmy_str_7]
    mov [rbp-96], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-96]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-104], rax
    mov rax, [rbp-104]
    cmp rax, 0
    jne block7_f8
    jmp block8_f8


block7_f8:
    lea rax, [rip + kmy_str_8]
    mov [rbp-112], rax
    mov rax, [rbp-112]
    mov rsp, rbp
    pop rbp
    ret


block8_f8:
    lea rax, [rip + kmy_str_9]
    mov [rbp-120], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-120]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-128], rax
    mov rax, [rbp-128]
    cmp rax, 0
    jne block9_f8
    jmp block10_f8


block9_f8:
    lea rax, [rip + kmy_str_10]
    mov [rbp-136], rax
    mov rax, [rbp-136]
    mov rsp, rbp
    pop rbp
    ret


block10_f8:
    lea rax, [rip + kmy_str_11]
    mov [rbp-144], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-144]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-152], rax
    mov rax, [rbp-152]
    cmp rax, 0
    jne block11_f8
    jmp block12_f8


block11_f8:
    lea rax, [rip + kmy_str_12]
    mov [rbp-160], rax
    mov rax, [rbp-160]
    mov rsp, rbp
    pop rbp
    ret


block12_f8:
    lea rax, [rip + kmy_str_13]
    mov [rbp-168], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-168]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-176], rax
    mov rax, [rbp-176]
    cmp rax, 0
    jne block13_f8
    jmp block14_f8


block13_f8:
    lea rax, [rip + kmy_str_14]
    mov [rbp-184], rax
    mov rax, [rbp-184]
    mov rsp, rbp
    pop rbp
    ret


block14_f8:
    lea rax, [rip + kmy_str_15]
    mov [rbp-192], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-192]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-200], rax
    mov rax, [rbp-200]
    cmp rax, 0
    jne block15_f8
    jmp block16_f8


block15_f8:
    lea rax, [rip + kmy_str_16]
    mov [rbp-208], rax
    mov rax, [rbp-208]
    mov rsp, rbp
    pop rbp
    ret


block16_f8:
    lea rax, [rip + kmy_str_17]
    mov [rbp-216], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-216]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-224], rax
    mov rax, [rbp-224]
    cmp rax, 0
    jne block17_f8
    jmp block18_f8


block17_f8:
    lea rax, [rip + kmy_str_18]
    mov [rbp-232], rax
    mov rax, [rbp-232]
    mov rsp, rbp
    pop rbp
    ret


block18_f8:
    lea rax, [rip + kmy_str_19]
    mov [rbp-240], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-240]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-248], rax
    mov rax, [rbp-248]
    cmp rax, 0
    jne block19_f8
    jmp block20_f8


block19_f8:
    lea rax, [rip + kmy_str_20]
    mov [rbp-256], rax
    mov rax, [rbp-256]
    mov rsp, rbp
    pop rbp
    ret


block20_f8:
    lea rax, [rip + kmy_str_21]
    mov [rbp-264], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-264]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-272], rax
    mov rax, [rbp-272]
    cmp rax, 0
    jne block21_f8
    jmp block22_f8


block21_f8:
    lea rax, [rip + kmy_str_22]
    mov [rbp-280], rax
    mov rax, [rbp-280]
    mov rsp, rbp
    pop rbp
    ret


block22_f8:
    lea rax, [rip + kmy_str_23]
    mov [rbp-288], rax
    mov rcx, [rbp-16]
    mov rdx, [rbp-288]
    sub rsp, 32
    call runtime_2
    add rsp, 32
    mov [rbp-296], rax
    mov rax, [rbp-296]
    cmp rax, 0
    jne block23_f8
    jmp block24_f8


block23_f8:
    lea rax, [rip + kmy_str_24]
    mov [rbp-304], rax
    mov rax, [rbp-304]
    mov rsp, rbp
    pop rbp
    ret


block24_f8:
    lea rax, [rip + kmy_str_25]
    mov [rbp-312], rax
    mov rax, [rbp-312]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 9
block0_f9:
    push rbp
    mov rbp, rsp
    sub rsp, 368
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-16], rax
    jmp block1_f9


block1_f9:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-24], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-32], rax
    mov rax, [rbp-24]
    cmp rax, [rbp-32]
    setl al
    movzx rax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    cmp rax, 0
    jne block4_f9
    jmp block5_f9


block2_f9:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-176], rax
    mov rax, 1
    mov [rbp-184], rax
    mov rax, [rbp-176]
    add rax, [rbp-184]
    mov [rbp-192], rax
    mov rax, [rbp-192]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block1_f9


block3_f9:
    lea rax, [rip + kmy_str_26]
    mov [rbp-200], rax
    mov rax, [rbp-200]
    mov [rbp-320], rax
    mov rax, [rbp-16]
    mov [rbp-328], rax
    jmp block10_f9


block4_f9:
    mov rax, 0
    mov [rbp-56], rax
    lea rax, [rip + block0_f6]
    mov [rbp-336], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-48], rax
    mov rax, [rbp-336]
    mov rcx, [rbp-48]
    mov [rcx], rax
    mov rax, [rbp-56]
    mov rcx, [rbp-48]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-64], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-72], rax
    mov rcx, [rbp-64]
    mov rdx, [rbp-72]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-80], rax
    mov rax, [rbp-48]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-80]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-88], rax
    mov rax, [rbp-88]
    cmp rax, 0
    jne block7_f9
    jmp block8_f9


block5_f9:
    mov rax, 0
    mov [rbp-160], rax
    mov rax, [rbp-160]
    mov [rbp-168], rax
    jmp block6_f9


block6_f9:
    mov rax, [rbp-168]
    cmp rax, 0
    jne block2_f9
    jmp block3_f9


block7_f9:
    mov rax, 1
    mov [rbp-96], rax
    mov rax, [rbp-96]
    mov [rbp-152], rax
    jmp block9_f9


block8_f9:
    mov rax, 0
    mov [rbp-112], rax
    lea rax, [rip + block0_f5]
    mov [rbp-344], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-104], rax
    mov rax, [rbp-344]
    mov rcx, [rbp-104]
    mov [rcx], rax
    mov rax, [rbp-112]
    mov rcx, [rbp-104]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-120], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-128], rax
    mov rcx, [rbp-120]
    mov rdx, [rbp-128]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-136], rax
    mov rax, [rbp-104]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-136]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-144], rax
    mov rax, [rbp-144]
    mov [rbp-152], rax
    jmp block9_f9


block9_f9:
    mov rax, [rbp-152]
    mov [rbp-168], rax
    jmp block6_f9


block10_f9:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-208], rax
    mov rax, [rbp-328]
    cmp rax, [rbp-208]
    setl al
    movzx rax, al
    mov [rbp-216], rax
    mov rax, [rbp-216]
    cmp rax, 0
    jne block11_f9
    jmp block12_f9


block11_f9:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-224], rax
    mov rcx, [rbp-224]
    mov rdx, [rbp-328]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-232], rax
    mov rcx, [rbp-232]
    sub rsp, 32
    call runtime_8
    add rsp, 32
    mov [rbp-240], rax
    mov rcx, [rbp-320]
    mov rdx, [rbp-240]
    sub rsp, 32
    call runtime_3
    add rsp, 32
    mov [rbp-248], rax
    mov rax, 1
    mov [rbp-256], rax
    mov rax, [rbp-328]
    add rax, [rbp-256]
    mov [rbp-264], rax
    mov rax, [rbp-248]
    mov [rbp-320], rax
    mov rax, [rbp-264]
    mov [rbp-328], rax
    jmp block10_f9


block12_f9:
    mov rax, 0
    mov [rbp-280], rax
    lea rax, [rip + block0_f7]
    mov [rbp-352], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-272], rax
    mov rax, [rbp-352]
    mov rcx, [rbp-272]
    mov [rcx], rax
    mov rax, [rbp-280]
    mov rcx, [rbp-272]
    mov [rcx+8], rax
    mov rax, 0
    mov [rbp-296], rax
    lea rax, [rip + block0_f8]
    mov [rbp-360], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-288], rax
    mov rax, [rbp-360]
    mov rcx, [rbp-288]
    mov [rcx], rax
    mov rax, [rbp-296]
    mov rcx, [rbp-288]
    mov [rcx+8], rax
    mov rax, [rbp-288]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-320]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-304], rax
    mov rax, [rbp-272]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-304]
    mov r9, [rbp-320]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-312], rax
    mov rsp, rbp
    pop rbp
    ret


# entry to function 10
block0_f10:
    push rbp
    mov rbp, rsp
    sub rsp, 272
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-16], rax
    jmp block1_f10


block1_f10:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-24], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-32], rax
    mov rax, [rbp-24]
    cmp rax, [rbp-32]
    setl al
    movzx rax, al
    mov [rbp-40], rax
    mov rax, [rbp-40]
    cmp rax, 0
    jne block4_f10
    jmp block5_f10


block2_f10:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-112], rax
    mov rax, 1
    mov [rbp-120], rax
    mov rax, [rbp-112]
    add rax, [rbp-120]
    mov [rbp-128], rax
    mov rax, [rbp-128]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block1_f10


block3_f10:
    lea rax, [rip + kmy_str_26]
    mov [rbp-136], rax
    mov rax, [rbp-136]
    mov [rbp-240], rax
    mov rax, [rbp-16]
    mov [rbp-248], rax
    jmp block7_f10


block4_f10:
    mov rax, 0
    mov [rbp-56], rax
    lea rax, [rip + block0_f5]
    mov [rbp-256], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-48], rax
    mov rax, [rbp-256]
    mov rcx, [rbp-48]
    mov [rcx], rax
    mov rax, [rbp-56]
    mov rcx, [rbp-48]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-64], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-72], rax
    mov rcx, [rbp-64]
    mov rdx, [rbp-72]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-80], rax
    mov rax, [rbp-48]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-80]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-88], rax
    mov rax, [rbp-88]
    mov [rbp-104], rax
    jmp block6_f10


block5_f10:
    mov rax, 0
    mov [rbp-96], rax
    mov rax, [rbp-96]
    mov [rbp-104], rax
    jmp block6_f10


block6_f10:
    mov rax, [rbp-104]
    cmp rax, 0
    jne block2_f10
    jmp block3_f10


block7_f10:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-144], rax
    mov rax, [rbp-248]
    cmp rax, [rbp-144]
    setl al
    movzx rax, al
    mov [rbp-152], rax
    mov rax, [rbp-152]
    cmp rax, 0
    jne block8_f10
    jmp block9_f10


block8_f10:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-160], rax
    mov rcx, [rbp-160]
    mov rdx, [rbp-248]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-168], rax
    mov rcx, [rbp-168]
    sub rsp, 32
    call runtime_8
    add rsp, 32
    mov [rbp-176], rax
    mov rcx, [rbp-240]
    mov rdx, [rbp-176]
    sub rsp, 32
    call runtime_3
    add rsp, 32
    mov [rbp-184], rax
    mov rax, 1
    mov [rbp-192], rax
    mov rax, [rbp-248]
    add rax, [rbp-192]
    mov [rbp-200], rax
    mov rax, [rbp-184]
    mov [rbp-240], rax
    mov rax, [rbp-200]
    mov [rbp-248], rax
    jmp block7_f10


block9_f10:
    mov rax, 0
    mov [rbp-216], rax
    lea rax, [rip + block0_f7]
    mov [rbp-264], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-208], rax
    mov rax, [rbp-264]
    mov rcx, [rbp-208]
    mov [rcx], rax
    mov rax, [rbp-216]
    mov rcx, [rbp-208]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_27]
    mov [rbp-224], rax
    mov rax, [rbp-208]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-224]
    mov r9, [rbp-240]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-232], rax
    mov rsp, rbp
    pop rbp
    ret


# entry to function 11
block0_f11:
    push rbp
    mov rbp, rsp
    sub rsp, 2960
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    jmp block1_f11


block1_f11:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-16], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-24], rax
    mov rax, [rbp-16]
    cmp rax, [rbp-24]
    setl al
    movzx rax, al
    mov [rbp-32], rax
    mov rax, [rbp-32]
    cmp rax, 0
    jne block2_f11
    jmp block3_f11


block2_f11:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-40], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-48], rax
    mov rcx, [rbp-40]
    mov rdx, [rbp-48]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-56], rax
    mov rax, 32
    mov [rbp-64], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-64]
    sete al
    movzx rax, al
    mov [rbp-72], rax
    mov rax, [rbp-72]
    cmp rax, 0
    jne block7_f11
    jmp block8_f11


block3_f11:
    mov rax, 0
    mov [rbp-2744], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2776], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-2736], rax
    mov rax, [rbp-2776]
    mov rcx, [rbp-2736]
    mov [rcx], rax
    mov rax, [rbp-2744]
    mov rcx, [rbp-2736]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_63]
    mov [rbp-2752], rax
    lea rax, [rip + kmy_str_26]
    mov [rbp-2760], rax
    mov rax, [rbp-2736]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-2752]
    mov r9, [rbp-2760]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-2768], rax
    mov rsp, rbp
    pop rbp
    ret


block4_f11:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-176], rax
    mov rax, 1
    mov [rbp-184], rax
    mov rax, [rbp-176]
    add rax, [rbp-184]
    mov [rbp-192], rax
    mov rax, [rbp-192]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block5_f11


block5_f11:
    jmp block1_f11


block6_f11:
    mov rax, 47
    mov [rbp-200], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-200]
    sete al
    movzx rax, al
    mov [rbp-208], rax
    mov rax, [rbp-208]
    cmp rax, 0
    jne block19_f11
    jmp block20_f11


block7_f11:
    mov rax, 1
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov [rbp-104], rax
    jmp block9_f11


block8_f11:
    mov rax, 9
    mov [rbp-88], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-88]
    sete al
    movzx rax, al
    mov [rbp-96], rax
    mov rax, [rbp-96]
    mov [rbp-104], rax
    jmp block9_f11


block9_f11:
    mov rax, [rbp-104]
    cmp rax, 0
    jne block10_f11
    jmp block11_f11


block10_f11:
    mov rax, 1
    mov [rbp-112], rax
    mov rax, [rbp-112]
    mov [rbp-136], rax
    jmp block12_f11


block11_f11:
    mov rax, 10
    mov [rbp-120], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-120]
    sete al
    movzx rax, al
    mov [rbp-128], rax
    mov rax, [rbp-128]
    mov [rbp-136], rax
    jmp block12_f11


block12_f11:
    mov rax, [rbp-136]
    cmp rax, 0
    jne block13_f11
    jmp block14_f11


block13_f11:
    mov rax, 1
    mov [rbp-144], rax
    mov rax, [rbp-144]
    mov [rbp-168], rax
    jmp block15_f11


block14_f11:
    mov rax, 13
    mov [rbp-152], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-152]
    sete al
    movzx rax, al
    mov [rbp-160], rax
    mov rax, [rbp-160]
    mov [rbp-168], rax
    jmp block15_f11


block15_f11:
    mov rax, [rbp-168]
    cmp rax, 0
    jne block4_f11
    jmp block6_f11


block16_f11:
    jmp block25_f11


block17_f11:
    jmp block5_f11


block18_f11:
    mov rax, 0
    mov [rbp-456], rax
    lea rax, [rip + block0_f6]
    mov [rbp-2784], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-448], rax
    mov rax, [rbp-2784]
    mov rcx, [rbp-448]
    mov [rcx], rax
    mov rax, [rbp-456]
    mov rcx, [rbp-448]
    mov [rcx+8], rax
    mov rax, [rbp-448]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-56]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-464], rax
    mov rax, [rbp-464]
    cmp rax, 0
    jne block31_f11
    jmp block33_f11


block19_f11:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-216], rax
    mov rax, 1
    mov [rbp-224], rax
    mov rax, [rbp-216]
    add rax, [rbp-224]
    mov [rbp-232], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-240], rax
    mov rax, [rbp-232]
    cmp rax, [rbp-240]
    setl al
    movzx rax, al
    mov [rbp-248], rax
    mov rax, [rbp-248]
    mov [rbp-264], rax
    jmp block21_f11


block20_f11:
    mov rax, 0
    mov [rbp-256], rax
    mov rax, [rbp-256]
    mov [rbp-264], rax
    jmp block21_f11


block21_f11:
    mov rax, [rbp-264]
    cmp rax, 0
    jne block22_f11
    jmp block23_f11


block22_f11:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-272], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-280], rax
    mov rax, 1
    mov [rbp-288], rax
    mov rax, [rbp-280]
    add rax, [rbp-288]
    mov [rbp-296], rax
    mov rcx, [rbp-272]
    mov rdx, [rbp-296]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-304], rax
    mov rax, 47
    mov [rbp-312], rax
    mov rax, [rbp-304]
    cmp rax, [rbp-312]
    sete al
    movzx rax, al
    mov [rbp-320], rax
    mov rax, [rbp-320]
    mov [rbp-336], rax
    jmp block24_f11


block23_f11:
    mov rax, 0
    mov [rbp-328], rax
    mov rax, [rbp-328]
    mov [rbp-336], rax
    jmp block24_f11


block24_f11:
    mov rax, [rbp-336]
    cmp rax, 0
    jne block16_f11
    jmp block18_f11


block25_f11:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-344], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-352], rax
    mov rax, [rbp-344]
    cmp rax, [rbp-352]
    setl al
    movzx rax, al
    mov [rbp-360], rax
    mov rax, [rbp-360]
    cmp rax, 0
    jne block28_f11
    jmp block29_f11


block26_f11:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-424], rax
    mov rax, 1
    mov [rbp-432], rax
    mov rax, [rbp-424]
    add rax, [rbp-432]
    mov [rbp-440], rax
    mov rax, [rbp-440]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block25_f11


block27_f11:
    jmp block17_f11


block28_f11:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-368], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-376], rax
    mov rcx, [rbp-368]
    mov rdx, [rbp-376]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-384], rax
    mov rax, 10
    mov [rbp-392], rax
    mov rax, [rbp-384]
    cmp rax, [rbp-392]
    setne al
    movzx rax, al
    mov [rbp-400], rax
    mov rax, [rbp-400]
    mov [rbp-416], rax
    jmp block30_f11


block29_f11:
    mov rax, 0
    mov [rbp-408], rax
    mov rax, [rbp-408]
    mov [rbp-416], rax
    jmp block30_f11


block30_f11:
    mov rax, [rbp-416]
    cmp rax, 0
    jne block26_f11
    jmp block27_f11


block31_f11:
    mov rax, 0
    mov [rbp-480], rax
    lea rax, [rip + block0_f9]
    mov [rbp-2792], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-472], rax
    mov rax, [rbp-2792]
    mov rcx, [rbp-472]
    mov [rcx], rax
    mov rax, [rbp-480]
    mov rcx, [rbp-472]
    mov [rcx+8], rax
    mov rax, [rbp-472]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-488], rax
    jmp block32_f11


block32_f11:
    jmp block17_f11


block33_f11:
    mov rax, 0
    mov [rbp-504], rax
    lea rax, [rip + block0_f5]
    mov [rbp-2800], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-496], rax
    mov rax, [rbp-2800]
    mov rcx, [rbp-496]
    mov [rcx], rax
    mov rax, [rbp-504]
    mov rcx, [rbp-496]
    mov [rcx+8], rax
    mov rax, [rbp-496]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-56]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-512], rax
    mov rax, [rbp-512]
    cmp rax, 0
    jne block34_f11
    jmp block36_f11


block34_f11:
    mov rax, 0
    mov [rbp-528], rax
    lea rax, [rip + block0_f10]
    mov [rbp-2808], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-520], rax
    mov rax, [rbp-2808]
    mov rcx, [rbp-520]
    mov [rcx], rax
    mov rax, [rbp-528]
    mov rcx, [rbp-520]
    mov [rcx+8], rax
    mov rax, [rbp-520]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-536], rax
    jmp block35_f11


block35_f11:
    jmp block32_f11


block36_f11:
    mov rax, 61
    mov [rbp-544], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-544]
    sete al
    movzx rax, al
    mov [rbp-552], rax
    mov rax, [rbp-552]
    cmp rax, 0
    jne block40_f11
    jmp block41_f11


block37_f11:
    mov rax, 0
    mov [rbp-696], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2816], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-688], rax
    mov rax, [rbp-2816]
    mov rcx, [rbp-688]
    mov [rcx], rax
    mov rax, [rbp-696]
    mov rcx, [rbp-688]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_28]
    mov [rbp-704], rax
    lea rax, [rip + kmy_str_29]
    mov [rbp-712], rax
    mov rax, [rbp-688]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-704]
    mov r9, [rbp-712]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-720], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-728], rax
    mov rax, 2
    mov [rbp-736], rax
    mov rax, [rbp-728]
    add rax, [rbp-736]
    mov [rbp-744], rax
    mov rax, [rbp-744]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block38_f11


block38_f11:
    jmp block35_f11


block39_f11:
    mov rax, 33
    mov [rbp-752], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-752]
    sete al
    movzx rax, al
    mov [rbp-760], rax
    mov rax, [rbp-760]
    cmp rax, 0
    jne block49_f11
    jmp block50_f11


block40_f11:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-560], rax
    mov rax, 1
    mov [rbp-568], rax
    mov rax, [rbp-560]
    add rax, [rbp-568]
    mov [rbp-576], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-584], rax
    mov rax, [rbp-576]
    cmp rax, [rbp-584]
    setl al
    movzx rax, al
    mov [rbp-592], rax
    mov rax, [rbp-592]
    mov [rbp-608], rax
    jmp block42_f11


block41_f11:
    mov rax, 0
    mov [rbp-600], rax
    mov rax, [rbp-600]
    mov [rbp-608], rax
    jmp block42_f11


block42_f11:
    mov rax, [rbp-608]
    cmp rax, 0
    jne block43_f11
    jmp block44_f11


block43_f11:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-616], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-624], rax
    mov rax, 1
    mov [rbp-632], rax
    mov rax, [rbp-624]
    add rax, [rbp-632]
    mov [rbp-640], rax
    mov rcx, [rbp-616]
    mov rdx, [rbp-640]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-648], rax
    mov rax, 61
    mov [rbp-656], rax
    mov rax, [rbp-648]
    cmp rax, [rbp-656]
    sete al
    movzx rax, al
    mov [rbp-664], rax
    mov rax, [rbp-664]
    mov [rbp-680], rax
    jmp block45_f11


block44_f11:
    mov rax, 0
    mov [rbp-672], rax
    mov rax, [rbp-672]
    mov [rbp-680], rax
    jmp block45_f11


block45_f11:
    mov rax, [rbp-680]
    cmp rax, 0
    jne block37_f11
    jmp block39_f11


block46_f11:
    mov rax, 0
    mov [rbp-904], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2824], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-896], rax
    mov rax, [rbp-2824]
    mov rcx, [rbp-896]
    mov [rcx], rax
    mov rax, [rbp-904]
    mov rcx, [rbp-896]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_30]
    mov [rbp-912], rax
    lea rax, [rip + kmy_str_31]
    mov [rbp-920], rax
    mov rax, [rbp-896]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-912]
    mov r9, [rbp-920]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-928], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-936], rax
    mov rax, 2
    mov [rbp-944], rax
    mov rax, [rbp-936]
    add rax, [rbp-944]
    mov [rbp-952], rax
    mov rax, [rbp-952]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block47_f11


block47_f11:
    jmp block38_f11


block48_f11:
    mov rax, 60
    mov [rbp-960], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-960]
    sete al
    movzx rax, al
    mov [rbp-968], rax
    mov rax, [rbp-968]
    cmp rax, 0
    jne block58_f11
    jmp block59_f11


block49_f11:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-768], rax
    mov rax, 1
    mov [rbp-776], rax
    mov rax, [rbp-768]
    add rax, [rbp-776]
    mov [rbp-784], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-792], rax
    mov rax, [rbp-784]
    cmp rax, [rbp-792]
    setl al
    movzx rax, al
    mov [rbp-800], rax
    mov rax, [rbp-800]
    mov [rbp-816], rax
    jmp block51_f11


block50_f11:
    mov rax, 0
    mov [rbp-808], rax
    mov rax, [rbp-808]
    mov [rbp-816], rax
    jmp block51_f11


block51_f11:
    mov rax, [rbp-816]
    cmp rax, 0
    jne block52_f11
    jmp block53_f11


block52_f11:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-824], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-832], rax
    mov rax, 1
    mov [rbp-840], rax
    mov rax, [rbp-832]
    add rax, [rbp-840]
    mov [rbp-848], rax
    mov rcx, [rbp-824]
    mov rdx, [rbp-848]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-856], rax
    mov rax, 61
    mov [rbp-864], rax
    mov rax, [rbp-856]
    cmp rax, [rbp-864]
    sete al
    movzx rax, al
    mov [rbp-872], rax
    mov rax, [rbp-872]
    mov [rbp-888], rax
    jmp block54_f11


block53_f11:
    mov rax, 0
    mov [rbp-880], rax
    mov rax, [rbp-880]
    mov [rbp-888], rax
    jmp block54_f11


block54_f11:
    mov rax, [rbp-888]
    cmp rax, 0
    jne block46_f11
    jmp block48_f11


block55_f11:
    mov rax, 0
    mov [rbp-1112], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2832], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-1104], rax
    mov rax, [rbp-2832]
    mov rcx, [rbp-1104]
    mov [rcx], rax
    mov rax, [rbp-1112]
    mov rcx, [rbp-1104]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_32]
    mov [rbp-1120], rax
    lea rax, [rip + kmy_str_33]
    mov [rbp-1128], rax
    mov rax, [rbp-1104]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-1120]
    mov r9, [rbp-1128]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-1136], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1144], rax
    mov rax, 2
    mov [rbp-1152], rax
    mov rax, [rbp-1144]
    add rax, [rbp-1152]
    mov [rbp-1160], rax
    mov rax, [rbp-1160]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block56_f11


block56_f11:
    jmp block47_f11


block57_f11:
    mov rax, 62
    mov [rbp-1168], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-1168]
    sete al
    movzx rax, al
    mov [rbp-1176], rax
    mov rax, [rbp-1176]
    cmp rax, 0
    jne block67_f11
    jmp block68_f11


block58_f11:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-976], rax
    mov rax, 1
    mov [rbp-984], rax
    mov rax, [rbp-976]
    add rax, [rbp-984]
    mov [rbp-992], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-1000], rax
    mov rax, [rbp-992]
    cmp rax, [rbp-1000]
    setl al
    movzx rax, al
    mov [rbp-1008], rax
    mov rax, [rbp-1008]
    mov [rbp-1024], rax
    jmp block60_f11


block59_f11:
    mov rax, 0
    mov [rbp-1016], rax
    mov rax, [rbp-1016]
    mov [rbp-1024], rax
    jmp block60_f11


block60_f11:
    mov rax, [rbp-1024]
    cmp rax, 0
    jne block61_f11
    jmp block62_f11


block61_f11:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-1032], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1040], rax
    mov rax, 1
    mov [rbp-1048], rax
    mov rax, [rbp-1040]
    add rax, [rbp-1048]
    mov [rbp-1056], rax
    mov rcx, [rbp-1032]
    mov rdx, [rbp-1056]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-1064], rax
    mov rax, 61
    mov [rbp-1072], rax
    mov rax, [rbp-1064]
    cmp rax, [rbp-1072]
    sete al
    movzx rax, al
    mov [rbp-1080], rax
    mov rax, [rbp-1080]
    mov [rbp-1096], rax
    jmp block63_f11


block62_f11:
    mov rax, 0
    mov [rbp-1088], rax
    mov rax, [rbp-1088]
    mov [rbp-1096], rax
    jmp block63_f11


block63_f11:
    mov rax, [rbp-1096]
    cmp rax, 0
    jne block55_f11
    jmp block57_f11


block64_f11:
    mov rax, 0
    mov [rbp-1320], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2840], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-1312], rax
    mov rax, [rbp-2840]
    mov rcx, [rbp-1312]
    mov [rcx], rax
    mov rax, [rbp-1320]
    mov rcx, [rbp-1312]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_34]
    mov [rbp-1328], rax
    lea rax, [rip + kmy_str_35]
    mov [rbp-1336], rax
    mov rax, [rbp-1312]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-1328]
    mov r9, [rbp-1336]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-1344], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1352], rax
    mov rax, 2
    mov [rbp-1360], rax
    mov rax, [rbp-1352]
    add rax, [rbp-1360]
    mov [rbp-1368], rax
    mov rax, [rbp-1368]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block65_f11


block65_f11:
    jmp block56_f11


block66_f11:
    mov rax, 38
    mov [rbp-1376], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-1376]
    sete al
    movzx rax, al
    mov [rbp-1384], rax
    mov rax, [rbp-1384]
    cmp rax, 0
    jne block76_f11
    jmp block77_f11


block67_f11:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1184], rax
    mov rax, 1
    mov [rbp-1192], rax
    mov rax, [rbp-1184]
    add rax, [rbp-1192]
    mov [rbp-1200], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-1208], rax
    mov rax, [rbp-1200]
    cmp rax, [rbp-1208]
    setl al
    movzx rax, al
    mov [rbp-1216], rax
    mov rax, [rbp-1216]
    mov [rbp-1232], rax
    jmp block69_f11


block68_f11:
    mov rax, 0
    mov [rbp-1224], rax
    mov rax, [rbp-1224]
    mov [rbp-1232], rax
    jmp block69_f11


block69_f11:
    mov rax, [rbp-1232]
    cmp rax, 0
    jne block70_f11
    jmp block71_f11


block70_f11:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-1240], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1248], rax
    mov rax, 1
    mov [rbp-1256], rax
    mov rax, [rbp-1248]
    add rax, [rbp-1256]
    mov [rbp-1264], rax
    mov rcx, [rbp-1240]
    mov rdx, [rbp-1264]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-1272], rax
    mov rax, 61
    mov [rbp-1280], rax
    mov rax, [rbp-1272]
    cmp rax, [rbp-1280]
    sete al
    movzx rax, al
    mov [rbp-1288], rax
    mov rax, [rbp-1288]
    mov [rbp-1304], rax
    jmp block72_f11


block71_f11:
    mov rax, 0
    mov [rbp-1296], rax
    mov rax, [rbp-1296]
    mov [rbp-1304], rax
    jmp block72_f11


block72_f11:
    mov rax, [rbp-1304]
    cmp rax, 0
    jne block64_f11
    jmp block66_f11


block73_f11:
    mov rax, 0
    mov [rbp-1528], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2848], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-1520], rax
    mov rax, [rbp-2848]
    mov rcx, [rbp-1520]
    mov [rcx], rax
    mov rax, [rbp-1528]
    mov rcx, [rbp-1520]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_36]
    mov [rbp-1536], rax
    lea rax, [rip + kmy_str_37]
    mov [rbp-1544], rax
    mov rax, [rbp-1520]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-1536]
    mov r9, [rbp-1544]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-1552], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1560], rax
    mov rax, 2
    mov [rbp-1568], rax
    mov rax, [rbp-1560]
    add rax, [rbp-1568]
    mov [rbp-1576], rax
    mov rax, [rbp-1576]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block74_f11


block74_f11:
    jmp block65_f11


block75_f11:
    mov rax, 124
    mov [rbp-1584], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-1584]
    sete al
    movzx rax, al
    mov [rbp-1592], rax
    mov rax, [rbp-1592]
    cmp rax, 0
    jne block85_f11
    jmp block86_f11


block76_f11:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1392], rax
    mov rax, 1
    mov [rbp-1400], rax
    mov rax, [rbp-1392]
    add rax, [rbp-1400]
    mov [rbp-1408], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-1416], rax
    mov rax, [rbp-1408]
    cmp rax, [rbp-1416]
    setl al
    movzx rax, al
    mov [rbp-1424], rax
    mov rax, [rbp-1424]
    mov [rbp-1440], rax
    jmp block78_f11


block77_f11:
    mov rax, 0
    mov [rbp-1432], rax
    mov rax, [rbp-1432]
    mov [rbp-1440], rax
    jmp block78_f11


block78_f11:
    mov rax, [rbp-1440]
    cmp rax, 0
    jne block79_f11
    jmp block80_f11


block79_f11:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-1448], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1456], rax
    mov rax, 1
    mov [rbp-1464], rax
    mov rax, [rbp-1456]
    add rax, [rbp-1464]
    mov [rbp-1472], rax
    mov rcx, [rbp-1448]
    mov rdx, [rbp-1472]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-1480], rax
    mov rax, 38
    mov [rbp-1488], rax
    mov rax, [rbp-1480]
    cmp rax, [rbp-1488]
    sete al
    movzx rax, al
    mov [rbp-1496], rax
    mov rax, [rbp-1496]
    mov [rbp-1512], rax
    jmp block81_f11


block80_f11:
    mov rax, 0
    mov [rbp-1504], rax
    mov rax, [rbp-1504]
    mov [rbp-1512], rax
    jmp block81_f11


block81_f11:
    mov rax, [rbp-1512]
    cmp rax, 0
    jne block73_f11
    jmp block75_f11


block82_f11:
    mov rax, 0
    mov [rbp-1736], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2856], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-1728], rax
    mov rax, [rbp-2856]
    mov rcx, [rbp-1728]
    mov [rcx], rax
    mov rax, [rbp-1736]
    mov rcx, [rbp-1728]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_38]
    mov [rbp-1744], rax
    lea rax, [rip + kmy_str_39]
    mov [rbp-1752], rax
    mov rax, [rbp-1728]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-1744]
    mov r9, [rbp-1752]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-1760], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1768], rax
    mov rax, 2
    mov [rbp-1776], rax
    mov rax, [rbp-1768]
    add rax, [rbp-1776]
    mov [rbp-1784], rax
    mov rax, [rbp-1784]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block83_f11


block83_f11:
    jmp block74_f11


block84_f11:
    mov rax, 61
    mov [rbp-1792], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-1792]
    sete al
    movzx rax, al
    mov [rbp-1800], rax
    mov rax, [rbp-1800]
    cmp rax, 0
    jne block91_f11
    jmp block93_f11


block85_f11:
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1600], rax
    mov rax, 1
    mov [rbp-1608], rax
    mov rax, [rbp-1600]
    add rax, [rbp-1608]
    mov [rbp-1616], rax
    mov rax, [rbp-8]
    mov rax, [rax+8]
    mov [rbp-1624], rax
    mov rax, [rbp-1616]
    cmp rax, [rbp-1624]
    setl al
    movzx rax, al
    mov [rbp-1632], rax
    mov rax, [rbp-1632]
    mov [rbp-1648], rax
    jmp block87_f11


block86_f11:
    mov rax, 0
    mov [rbp-1640], rax
    mov rax, [rbp-1640]
    mov [rbp-1648], rax
    jmp block87_f11


block87_f11:
    mov rax, [rbp-1648]
    cmp rax, 0
    jne block88_f11
    jmp block89_f11


block88_f11:
    mov rax, [rbp-8]
    mov rax, [rax]
    mov [rbp-1656], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1664], rax
    mov rax, 1
    mov [rbp-1672], rax
    mov rax, [rbp-1664]
    add rax, [rbp-1672]
    mov [rbp-1680], rax
    mov rcx, [rbp-1656]
    mov rdx, [rbp-1680]
    sub rsp, 32
    call runtime_5
    add rsp, 32
    mov [rbp-1688], rax
    mov rax, 124
    mov [rbp-1696], rax
    mov rax, [rbp-1688]
    cmp rax, [rbp-1696]
    sete al
    movzx rax, al
    mov [rbp-1704], rax
    mov rax, [rbp-1704]
    mov [rbp-1720], rax
    jmp block90_f11


block89_f11:
    mov rax, 0
    mov [rbp-1712], rax
    mov rax, [rbp-1712]
    mov [rbp-1720], rax
    jmp block90_f11


block90_f11:
    mov rax, [rbp-1720]
    cmp rax, 0
    jne block82_f11
    jmp block84_f11


block91_f11:
    mov rax, 0
    mov [rbp-1816], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2864], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-1808], rax
    mov rax, [rbp-2864]
    mov rcx, [rbp-1808]
    mov [rcx], rax
    mov rax, [rbp-1816]
    mov rcx, [rbp-1808]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_40]
    mov [rbp-1824], rax
    lea rax, [rip + kmy_str_41]
    mov [rbp-1832], rax
    mov rax, [rbp-1808]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-1824]
    mov r9, [rbp-1832]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-1840], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1848], rax
    mov rax, 1
    mov [rbp-1856], rax
    mov rax, [rbp-1848]
    add rax, [rbp-1856]
    mov [rbp-1864], rax
    mov rax, [rbp-1864]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block92_f11


block92_f11:
    jmp block83_f11


block93_f11:
    mov rax, 43
    mov [rbp-1872], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-1872]
    sete al
    movzx rax, al
    mov [rbp-1880], rax
    mov rax, [rbp-1880]
    cmp rax, 0
    jne block94_f11
    jmp block96_f11


block94_f11:
    mov rax, 0
    mov [rbp-1896], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2872], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-1888], rax
    mov rax, [rbp-2872]
    mov rcx, [rbp-1888]
    mov [rcx], rax
    mov rax, [rbp-1896]
    mov rcx, [rbp-1888]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_42]
    mov [rbp-1904], rax
    lea rax, [rip + kmy_str_43]
    mov [rbp-1912], rax
    mov rax, [rbp-1888]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-1904]
    mov r9, [rbp-1912]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-1920], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-1928], rax
    mov rax, 1
    mov [rbp-1936], rax
    mov rax, [rbp-1928]
    add rax, [rbp-1936]
    mov [rbp-1944], rax
    mov rax, [rbp-1944]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block95_f11


block95_f11:
    jmp block92_f11


block96_f11:
    mov rax, 45
    mov [rbp-1952], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-1952]
    sete al
    movzx rax, al
    mov [rbp-1960], rax
    mov rax, [rbp-1960]
    cmp rax, 0
    jne block97_f11
    jmp block99_f11


block97_f11:
    mov rax, 0
    mov [rbp-1976], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2880], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-1968], rax
    mov rax, [rbp-2880]
    mov rcx, [rbp-1968]
    mov [rcx], rax
    mov rax, [rbp-1976]
    mov rcx, [rbp-1968]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_44]
    mov [rbp-1984], rax
    lea rax, [rip + kmy_str_45]
    mov [rbp-1992], rax
    mov rax, [rbp-1968]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-1984]
    mov r9, [rbp-1992]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-2000], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-2008], rax
    mov rax, 1
    mov [rbp-2016], rax
    mov rax, [rbp-2008]
    add rax, [rbp-2016]
    mov [rbp-2024], rax
    mov rax, [rbp-2024]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block98_f11


block98_f11:
    jmp block95_f11


block99_f11:
    mov rax, 42
    mov [rbp-2032], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-2032]
    sete al
    movzx rax, al
    mov [rbp-2040], rax
    mov rax, [rbp-2040]
    cmp rax, 0
    jne block100_f11
    jmp block102_f11


block100_f11:
    mov rax, 0
    mov [rbp-2056], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2888], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-2048], rax
    mov rax, [rbp-2888]
    mov rcx, [rbp-2048]
    mov [rcx], rax
    mov rax, [rbp-2056]
    mov rcx, [rbp-2048]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_46]
    mov [rbp-2064], rax
    lea rax, [rip + kmy_str_47]
    mov [rbp-2072], rax
    mov rax, [rbp-2048]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-2064]
    mov r9, [rbp-2072]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-2080], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-2088], rax
    mov rax, 1
    mov [rbp-2096], rax
    mov rax, [rbp-2088]
    add rax, [rbp-2096]
    mov [rbp-2104], rax
    mov rax, [rbp-2104]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block101_f11


block101_f11:
    jmp block98_f11


block102_f11:
    mov rax, 47
    mov [rbp-2112], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-2112]
    sete al
    movzx rax, al
    mov [rbp-2120], rax
    mov rax, [rbp-2120]
    cmp rax, 0
    jne block103_f11
    jmp block105_f11


block103_f11:
    mov rax, 0
    mov [rbp-2136], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2896], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-2128], rax
    mov rax, [rbp-2896]
    mov rcx, [rbp-2128]
    mov [rcx], rax
    mov rax, [rbp-2136]
    mov rcx, [rbp-2128]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_48]
    mov [rbp-2144], rax
    lea rax, [rip + kmy_str_49]
    mov [rbp-2152], rax
    mov rax, [rbp-2128]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-2144]
    mov r9, [rbp-2152]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-2160], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-2168], rax
    mov rax, 1
    mov [rbp-2176], rax
    mov rax, [rbp-2168]
    add rax, [rbp-2176]
    mov [rbp-2184], rax
    mov rax, [rbp-2184]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block104_f11


block104_f11:
    jmp block101_f11


block105_f11:
    mov rax, 59
    mov [rbp-2192], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-2192]
    sete al
    movzx rax, al
    mov [rbp-2200], rax
    mov rax, [rbp-2200]
    cmp rax, 0
    jne block106_f11
    jmp block108_f11


block106_f11:
    mov rax, 0
    mov [rbp-2216], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2904], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-2208], rax
    mov rax, [rbp-2904]
    mov rcx, [rbp-2208]
    mov [rcx], rax
    mov rax, [rbp-2216]
    mov rcx, [rbp-2208]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_50]
    mov [rbp-2224], rax
    lea rax, [rip + kmy_str_51]
    mov [rbp-2232], rax
    mov rax, [rbp-2208]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-2224]
    mov r9, [rbp-2232]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-2240], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-2248], rax
    mov rax, 1
    mov [rbp-2256], rax
    mov rax, [rbp-2248]
    add rax, [rbp-2256]
    mov [rbp-2264], rax
    mov rax, [rbp-2264]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block107_f11


block107_f11:
    jmp block104_f11


block108_f11:
    mov rax, 44
    mov [rbp-2272], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-2272]
    sete al
    movzx rax, al
    mov [rbp-2280], rax
    mov rax, [rbp-2280]
    cmp rax, 0
    jne block109_f11
    jmp block111_f11


block109_f11:
    mov rax, 0
    mov [rbp-2296], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2912], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-2288], rax
    mov rax, [rbp-2912]
    mov rcx, [rbp-2288]
    mov [rcx], rax
    mov rax, [rbp-2296]
    mov rcx, [rbp-2288]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_52]
    mov [rbp-2304], rax
    lea rax, [rip + kmy_str_53]
    mov [rbp-2312], rax
    mov rax, [rbp-2288]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-2304]
    mov r9, [rbp-2312]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-2320], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-2328], rax
    mov rax, 1
    mov [rbp-2336], rax
    mov rax, [rbp-2328]
    add rax, [rbp-2336]
    mov [rbp-2344], rax
    mov rax, [rbp-2344]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block110_f11


block110_f11:
    jmp block107_f11


block111_f11:
    mov rax, 40
    mov [rbp-2352], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-2352]
    sete al
    movzx rax, al
    mov [rbp-2360], rax
    mov rax, [rbp-2360]
    cmp rax, 0
    jne block112_f11
    jmp block114_f11


block112_f11:
    mov rax, 0
    mov [rbp-2376], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2920], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-2368], rax
    mov rax, [rbp-2920]
    mov rcx, [rbp-2368]
    mov [rcx], rax
    mov rax, [rbp-2376]
    mov rcx, [rbp-2368]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_54]
    mov [rbp-2384], rax
    lea rax, [rip + kmy_str_55]
    mov [rbp-2392], rax
    mov rax, [rbp-2368]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-2384]
    mov r9, [rbp-2392]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-2400], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-2408], rax
    mov rax, 1
    mov [rbp-2416], rax
    mov rax, [rbp-2408]
    add rax, [rbp-2416]
    mov [rbp-2424], rax
    mov rax, [rbp-2424]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block113_f11


block113_f11:
    jmp block110_f11


block114_f11:
    mov rax, 41
    mov [rbp-2432], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-2432]
    sete al
    movzx rax, al
    mov [rbp-2440], rax
    mov rax, [rbp-2440]
    cmp rax, 0
    jne block115_f11
    jmp block117_f11


block115_f11:
    mov rax, 0
    mov [rbp-2456], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2928], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-2448], rax
    mov rax, [rbp-2928]
    mov rcx, [rbp-2448]
    mov [rcx], rax
    mov rax, [rbp-2456]
    mov rcx, [rbp-2448]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_56]
    mov [rbp-2464], rax
    lea rax, [rip + kmy_str_57]
    mov [rbp-2472], rax
    mov rax, [rbp-2448]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-2464]
    mov r9, [rbp-2472]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-2480], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-2488], rax
    mov rax, 1
    mov [rbp-2496], rax
    mov rax, [rbp-2488]
    add rax, [rbp-2496]
    mov [rbp-2504], rax
    mov rax, [rbp-2504]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block116_f11


block116_f11:
    jmp block113_f11


block117_f11:
    mov rax, 123
    mov [rbp-2512], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-2512]
    sete al
    movzx rax, al
    mov [rbp-2520], rax
    mov rax, [rbp-2520]
    cmp rax, 0
    jne block118_f11
    jmp block120_f11


block118_f11:
    mov rax, 0
    mov [rbp-2536], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2936], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-2528], rax
    mov rax, [rbp-2936]
    mov rcx, [rbp-2528]
    mov [rcx], rax
    mov rax, [rbp-2536]
    mov rcx, [rbp-2528]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_58]
    mov [rbp-2544], rax
    lea rax, [rip + kmy_str_59]
    mov [rbp-2552], rax
    mov rax, [rbp-2528]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-2544]
    mov r9, [rbp-2552]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-2560], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-2568], rax
    mov rax, 1
    mov [rbp-2576], rax
    mov rax, [rbp-2568]
    add rax, [rbp-2576]
    mov [rbp-2584], rax
    mov rax, [rbp-2584]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block119_f11


block119_f11:
    jmp block116_f11


block120_f11:
    mov rax, 125
    mov [rbp-2592], rax
    mov rax, [rbp-56]
    cmp rax, [rbp-2592]
    sete al
    movzx rax, al
    mov [rbp-2600], rax
    mov rax, [rbp-2600]
    cmp rax, 0
    jne block121_f11
    jmp block123_f11


block121_f11:
    mov rax, 0
    mov [rbp-2616], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2944], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-2608], rax
    mov rax, [rbp-2944]
    mov rcx, [rbp-2608]
    mov [rcx], rax
    mov rax, [rbp-2616]
    mov rcx, [rbp-2608]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_60]
    mov [rbp-2624], rax
    lea rax, [rip + kmy_str_61]
    mov [rbp-2632], rax
    mov rax, [rbp-2608]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-2624]
    mov r9, [rbp-2632]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-2640], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-2648], rax
    mov rax, 1
    mov [rbp-2656], rax
    mov rax, [rbp-2648]
    add rax, [rbp-2656]
    mov [rbp-2664], rax
    mov rax, [rbp-2664]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block122_f11


block122_f11:
    jmp block119_f11


block123_f11:
    mov rax, 0
    mov [rbp-2680], rax
    lea rax, [rip + block0_f7]
    mov [rbp-2952], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-2672], rax
    mov rax, [rbp-2952]
    mov rcx, [rbp-2672]
    mov [rcx], rax
    mov rax, [rbp-2680]
    mov rcx, [rbp-2672]
    mov [rcx+8], rax
    lea rax, [rip + kmy_str_62]
    mov [rbp-2688], rax
    mov rcx, [rbp-56]
    sub rsp, 32
    call runtime_8
    add rsp, 32
    mov [rbp-2696], rax
    mov rax, [rbp-2672]
    mov rcx, [rax+8]
    mov rdx, [rbp-8]
    mov r8, [rbp-2688]
    mov r9, [rbp-2696]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-2704], rax
    mov rax, [rbp-8]
    mov rax, [rax+16]
    mov [rbp-2712], rax
    mov rax, 1
    mov [rbp-2720], rax
    mov rax, [rbp-2712]
    add rax, [rbp-2720]
    mov [rbp-2728], rax
    mov rax, [rbp-2728]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    jmp block122_f11


# entry to function 12
block0_f12:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, 0
    mov [rbp-24], rax
    lea rax, [rip + block0_f2]
    mov [rbp-48], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-16], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-16]
    mov [rcx], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-16]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov rax, [rax+24]
    mov [rbp-32], rax
    mov rax, [rbp-16]
    mov rcx, [rax+8]
    mov rdx, [rbp-32]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-40], rax
    mov rsp, rbp
    pop rbp
    ret


# entry to function 14
block0_f14:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    mov rax, 0
    mov [rbp-16], rax
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, 0
    mov [rbp-24], rax
    mov rax, [rbp-24]
    mov rcx, [rbp-8]
    mov [rcx+16], rax
    mov rsp, rbp
    pop rbp
    ret


# entry to function 13
block0_f13:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-8], rdx
    # TODO: PARAM handled in prologue by storing directly in func
    mov [rbp-16], r8
    mov rax, [rbp-16]
    mov rcx, [rbp-8]
    mov [rcx], rax
    mov rcx, 1
    mov rdx, 32
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-24], rax
    mov rax, 0
    mov [rbp-40], rax
    lea rax, [rip + block0_f4]
    mov [rbp-88], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-32], rax
    mov rax, [rbp-88]
    mov rcx, [rbp-32]
    mov [rcx], rax
    mov rax, [rbp-40]
    mov rcx, [rbp-32]
    mov [rcx+8], rax
    mov rax, [rbp-32]
    mov rcx, [rax+8]
    mov rdx, [rbp-24]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-48], rax
    mov rax, 0
    mov [rbp-64], rax
    lea rax, [rip + block0_f3]
    mov [rbp-96], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-56], rax
    mov rax, [rbp-96]
    mov rcx, [rbp-56]
    mov [rcx], rax
    mov rax, [rbp-64]
    mov rcx, [rbp-56]
    mov [rcx+8], rax
    mov rax, [rbp-56]
    mov rcx, [rax+8]
    mov rdx, [rbp-24]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-72], rax
    mov rax, [rbp-72]
    mov rcx, [rbp-8]
    mov [rcx+24], rax
    mov rcx, [rbp-16]
    sub rsp, 32
    call runtime_4
    add rsp, 32
    mov [rbp-80], rax
    mov rax, [rbp-80]
    mov rcx, [rbp-8]
    mov [rcx+8], rax
    mov rax, [rbp-8]
    mov rsp, rbp
    pop rbp
    ret


# entry to function 0
# also the entry function 'main'
main: 
    push rbp
    mov rbp, rsp
    sub rsp, 176
    lea rax, [rip + kmy_str_64]
    mov [rbp-8], rax
    lea rax, [rip + kmy_str_65]
    mov [rbp-16], rax
    mov rcx, [rbp-8]
    mov rdx, [rbp-16]
    sub rsp, 32
    call runtime_7
    add rsp, 32
    mov [rbp-24], rax
    mov rcx, 1
    mov rdx, 32
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-32], rax
    mov rax, 0
    mov [rbp-48], rax
    lea rax, [rip + block0_f14]
    mov [rbp-144], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-40], rax
    mov rax, [rbp-144]
    mov rcx, [rbp-40]
    mov [rcx], rax
    mov rax, [rbp-48]
    mov rcx, [rbp-40]
    mov [rcx+8], rax
    mov rax, [rbp-40]
    mov rcx, [rax+8]
    mov rdx, [rbp-32]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-56], rax
    mov rax, 0
    mov [rbp-72], rax
    lea rax, [rip + block0_f13]
    mov [rbp-152], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-64], rax
    mov rax, [rbp-152]
    mov rcx, [rbp-64]
    mov [rcx], rax
    mov rax, [rbp-72]
    mov rcx, [rbp-64]
    mov [rcx+8], rax
    mov rcx, [rbp-8]
    sub rsp, 32
    call runtime_6
    add rsp, 32
    mov [rbp-80], rax
    mov rax, [rbp-64]
    mov rcx, [rax+8]
    mov rdx, [rbp-32]
    mov r8, [rbp-80]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-88], rax
    mov rax, 0
    mov [rbp-104], rax
    lea rax, [rip + block0_f11]
    mov [rbp-160], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-96], rax
    mov rax, [rbp-160]
    mov rcx, [rbp-96]
    mov [rcx], rax
    mov rax, [rbp-104]
    mov rcx, [rbp-96]
    mov [rcx+8], rax
    mov rax, [rbp-96]
    mov rcx, [rax+8]
    mov rdx, [rbp-88]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-112], rax
    mov rax, 0
    mov [rbp-128], rax
    lea rax, [rip + block0_f12]
    mov [rbp-168], rax
    mov rcx, 1
    mov rdx, 16
    sub rsp, 32
    call calloc
    add rsp, 32
    mov [rbp-120], rax
    mov rax, [rbp-168]
    mov rcx, [rbp-120]
    mov [rcx], rax
    mov rax, [rbp-128]
    mov rcx, [rbp-120]
    mov [rcx+8], rax
    mov rax, [rbp-120]
    mov rcx, [rax+8]
    mov rdx, [rbp-88]
    sub rsp, 32
    mov r11, [rax]
    call r11
    add rsp, 32
    mov [rbp-136], rax
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret


