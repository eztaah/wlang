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
    movq    $1, %rax
    movq    %rax, -8(%rbp)

    # if statement
    # < binop
    movq    $1, %rax
    pushq   %rax
    movq    -8(%rbp), %rax
    pop     %rbx
    cmpq    %rbx, %rax
    sete    %al
    movzbq  %al, %rax
    # binop >
    cmpq    $0, %rax
    je      .Lelse0

    # function call
    movq    $1, %rax
    movq    %rax, %rdi
    call    w__print_number
    jmp     .Lend0
.Lelse0:

    # function call
    movq    $0, %rax
    movq    %rax, %rdi
    call    w__print_number
.Lend0:

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
