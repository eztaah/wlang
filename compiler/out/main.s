    # --- SECTION DATA ---
    .section .data


    # --- SECTION BSS ---
    .section .bss


    # --- SECTION TEXT ---
    .section .text

_start:
    call w__main
    movq %rax, %rdi
    movq $60, %rax
    syscall
    .global _start

w__main:
    # function prologue
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $88, %rsp
    # storing arguments into stackframe

    # variables assignement
    leaq   -8(%rbp), %rax
    movq    %rax, %r15
    movq    $15, %rax
    movq    %rax, (%r15)

    # variables assignement
    leaq   -16(%rbp), %rax
    movq    %rax, %r15
    # funcall
    movq    -8(%rbp), %rax
    movq    %rax, %rdi
    call    w__malloc
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $0, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $72, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $8, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $101, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $16, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $108, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $24, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $108, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $32, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $111, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $40, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $44, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $48, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $32, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $56, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $87, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $64, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $111, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $72, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $114, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $80, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $108, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $88, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $100, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $96, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $33, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $104, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $10, %rax
    movq    %rax, (%r15)

    # variables assignement
    # < binop
    movq    $112, %rax
    pushq   %rax
    movq    -16(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, %r15
    movq    $0, %rax
    movq    %rax, (%r15)
    # funcall
    movq    -16(%rbp), %rax
    movq    %rax, %rdi
    call    w__print_string
    # funcall
    movq    -16(%rbp), %rax
    movq    %rax, %rdi
    call    w__free

    # return statement
    movq    $0, %rax
    # function epilogue
    movq    %rbp, %rsp
    pop     %rbp
    ret
    # function epilogue
    movq    %rbp, %rsp
    pop     %rbp
    ret

