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
    subq    $120, %rsp

    # array declaration
    movq    $72, %rax
    movq    %rax, -16(%rbp)
    movq    $101, %rax
    movq    %rax, -24(%rbp)
    movq    $108, %rax
    movq    %rax, -32(%rbp)
    movq    $108, %rax
    movq    %rax, -40(%rbp)
    movq    $111, %rax
    movq    %rax, -48(%rbp)
    movq    $32, %rax
    movq    %rax, -56(%rbp)
    movq    $87, %rax
    movq    %rax, -64(%rbp)
    movq    $111, %rax
    movq    %rax, -72(%rbp)
    movq    $114, %rax
    movq    %rax, -80(%rbp)
    movq    $108, %rax
    movq    %rax, -88(%rbp)
    movq    $100, %rax
    movq    %rax, -96(%rbp)
    movq    $33, %rax
    movq    %rax, -104(%rbp)
    movq    $10, %rax
    movq    %rax, -112(%rbp)

    # function call
    leaq    -8(%rbp), %rax
    movq    %rax, %rdi
    movq    $13, %rax
    movq    %rax, %rsi
    call    w__print_array

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
