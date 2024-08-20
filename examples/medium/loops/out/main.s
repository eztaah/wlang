    .section .data

    .section .bss

    .section .text
    .global _start

_start:
    # function call
    call w__main

    # syscall
    movq %rax, %rdi
    movq $60, %rax
    syscall


w__main:
    # prologue
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $24, %rsp

    # variable declaration
    movq    $0, %rax
    movq    %rax, -8(%rbp)

    # loop
.Lloop0:

    # function call
    movq    -8(%rbp), %rax
    movq    %rax, %rdi
    call    w__print_number

    # variable assignement
    leaq    -8(%rbp), %rax
    movq    %rax, %r15
    # < binop
    movq    $1, %rax
    pushq   %rax
    movq    -8(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, (%r15)

    # if statement
    # < binop
    movq    $5, %rax
    pushq   %rax
    movq    -8(%rbp), %rax
    pop     %rbx
    cmpq    %rbx, %rax
    sete    %al
    movzbq  %al, %rax
    # binop >
    cmpq    $0, %rax
    je      .Lelse0

    # break
    jmp     .Lloopend0
    jmp     .Lend0
.Lelse0:
.Lend0:
    jmp     .Lloop0
.Lloopend0:

    # return statement
    movq    $0, %rax

    # epilogue (from ret)
    movq    %rbp, %rsp
    pop     %rbp
    ret

    # epilogue (from brace)
    movq    %rbp, %rsp
    pop     %rbp
    ret

    .section .note.GNU-stack,"",@progbits
