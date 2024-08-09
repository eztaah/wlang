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

exit:
    # function prelude
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $88, %rsp
    # storing arguments into stackframe
    movq    %rdi, -8(%rbp)
    movq    $60, %rax
    syscall    
write:
    # function prelude
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $88, %rsp
    # storing arguments into stackframe
    movq    %rdi, -8(%rbp)
    movq    %rsi, -16(%rbp)
    movq    %rdx, -24(%rbp)
    movq    $1, %rax
    syscall    
    # return statement
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
    movq    -8(%rbp), %rax
    movq    %rax, %rdi
    call    i32_to_asciicode
    movq    %rax, -16(%rbp)
    movq    $1, %rax
    movq    %rax, %rdi
    leaq   -16(%rbp), %rax
    movq    %rax, %rsi
    movq    $1, %rax
    movq    %rax, %rdx
    call    write

    # variables declaration
    movq    $10, %rax
    movq    %rax, -24(%rbp)
    movq    $1, %rax
    movq    %rax, %rdi
    leaq   -24(%rbp), %rax
    movq    %rax, %rsi
    movq    $1, %rax
    movq    %rax, %rdx
    call    write

    # return statement
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
    movq    $4, %rax
    movq    %rax, -16(%rbp)

    # variables declaration
    movq    -8(%rbp), %rax
    movq    %rax, %rdi
    movq    -16(%rbp), %rax
    movq    %rax, %rsi
    call    add
    movq    %rax, -24(%rbp)
    movq    -24(%rbp), %rax
    movq    %rax, %rdi
    call    print_num
    movq    $0, %rax
    movq    %rax, %rdi
    call    exit
