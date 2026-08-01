    .section .text
    .global _start

_start:
    # put 2 + 1 in rax
    movq    $2, %rax
    pushq   %rax
    movq    $1, %rax
    popq     %rbx
    add     %rbx, %rax

    # syscall exit
    movq %rax, %rdi
    movq $60, %rax
    syscall
