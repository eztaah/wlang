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
    movq    -8(%rbp), %rax
    movq    %rax, %rdi
    movq    $0, %rax
    movq    %rax, %rsi
    movq    $0, %rax
    movq    %rax, %rdx
    movq    $0, %rax
    movq    %rax, %rcx
    movq    $0, %rax
    movq    %rax, %r8
    movq    $0, %rax
    movq    %rax, %r9
    movq    $60, %rax

    # clean stack
    movq    %rbp, %rsp
    pop     %rbp
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
    movq    -8(%rbp), %rax
    movq    %rax, %rdi
    movq    -16(%rbp), %rax
    movq    %rax, %rsi
    movq    -24(%rbp), %rax
    movq    %rax, %rdx
    movq    $0, %rax
    movq    %rax, %rcx
    movq    $0, %rax
    movq    %rax, %r8
    movq    $0, %rax
    movq    %rax, %r9
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

test:
    # function prelude
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $88, %rsp
    # storing arguments into stackframe

    # variables declaration
    movq    $1, %rax
    movq    %rax, -8(%rbp)

    # return statement
    movq    -8(%rbp), %rax
    movq    %rbp, %rsp
    pop     %rbp
    ret

_start:
    # function prelude
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $88, %rsp

    # variables declaration
    call    test
    movq    %rax, -8(%rbp)

    # variables declaration
    movq    $7, %rax
    movq    %rax, -16(%rbp)
    movq    -16(%rbp), %rax
    movq    %rax, %rdi
    call    print_num
    movq    $0, %rax
    movq    %rax, %rdi
    call    exit
