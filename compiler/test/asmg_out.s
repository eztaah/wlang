    # --- SECTION DATA ---
    .section .data


    # --- SECTION BSS ---
    .section .bss


    # --- SECTION TEXT ---
    .section .text
    .global _start

i32_to_asciicode:
    # function prelude
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $88, %rsp
    # storing arguments into stackframe
    movq    %rdi, -8(%rbp)

    # variables declaration
    # < binop
    movq    $48, %rax
    pushq   %rax
    movq    -8(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, -16(%rbp)

    # return statement
    movq    -16(%rbp), %rax
    movq    %rbp, %rsp
    pop     %rbp
    ret

print_num:
    # function prelude
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $88, %rsp
    # storing arguments into stackframe
    movq    %rdi, -8(%rbp)

    # variables declaration
    # Function call: i32_to_asciicode
    movq    -8(%rbp), %rax
    movq    %rax, %rdi
    call    i32_to_asciicode
    movq    %rax, -16(%rbp)

    # syscall write
    lea    -16(%rbp), %rsi
    movq    $1, %rax
    movq    $1, %rdi
    movq    $1, %rdx
    syscall

    # variables declaration
    movq    $10, %rax
    movq    %rax, -24(%rbp)

    # syscall write
    lea    -24(%rbp), %rsi
    movq    $1, %rax
    movq    $1, %rdi
    movq    $1, %rdx
    syscall

    # return statement
    movq    $0, %rax
    movq    %rbp, %rsp
    pop     %rbp
    ret

add:
    # function prelude
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $88, %rsp
    # storing arguments into stackframe
    movq    %rdi, -8(%rbp)
    movq    %rsi, -16(%rbp)

    # variables declaration
    # < binop
    movq    -16(%rbp), %rax
    pushq   %rax
    movq    -8(%rbp), %rax
    pop     %rbx
    add     %rbx, %rax
    # binop >
    movq    %rax, -24(%rbp)

    # return statement
    movq    -24(%rbp), %rax
    movq    %rbp, %rsp
    pop     %rbp
    ret

_start:
    # function prelude
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $88, %rsp

    # variables declaration
    movq    $4, %rax
    movq    %rax, -8(%rbp)

    # variables declaration
    movq    $2, %rax
    movq    %rax, -16(%rbp)

    # variables declaration
    # Function call: add
    movq    -8(%rbp), %rax
    movq    %rax, %rdi
    movq    -16(%rbp), %rax
    movq    %rax, %rsi
    call    add
    movq    %rax, -24(%rbp)
    # Function call: print_num
    movq    -24(%rbp), %rax
    movq    %rax, %rdi
    call    print_num

    # function prologue
    movq    %rbp, %rsp
    pop     %rbp

    # exit syscall
    movq    $60, %rax
    movq    $0, %rdi
    syscall
