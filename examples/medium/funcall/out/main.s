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


w__add:
    # prologue
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $32, %rsp

    # retrieve arguments
    movq    %rdi, -8(%rbp)
    movq    %rsi, -16(%rbp)

    # return statement
    # < binop
    movq    -16(%rbp), %rax
    pushq   %rax
    movq    -8(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >

    # epilogue (from ret)
    movq    %rbp, %rsp
    pop     %rbp
    ret

    # epilogue (from brace)
    movq    %rbp, %rsp
    pop     %rbp
    ret


w__main:
    # prologue
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $24, %rsp

    # variable declaration

    # function call
    movq    $2, %rax
    movq    %rax, %rdi
    movq    $3, %rax
    movq    %rax, %rsi
    call    w__add
    movq    %rax, -8(%rbp)

    # function call
    movq    -8(%rbp), %rax
    movq    %rax, %rdi
    call    w__print_number

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
