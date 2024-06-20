    # --- SECTION DATA ---
    .section .data
.S0:
    .string "coucou"
.Sn:
    .string "\n"
.SP:
    .string "Hello world, %d"


    # --- SECTION BSS ---
    .section .bss
box:
    .space 64      # we will store our variables here


    # --- SECTION TEXT ---
    .section .text
    .global _start
    .extern printf

_start:
    # intialisation
    pushq %rbp
    movq %rsp, %rbp
    subq	$8, %rsp

    # print
    movq $.SP, %rdi
    movq $2, %rsi
    // movq $2, %rdx
    xor %rax, %rax
    call print

    # print \n before exiting to prevent display bug
    movq $.Sn, %rdi
    xor %rax, %rax
    call print

    # end of the program
    movq %rbp, %rsp
    pop %rbp
    # exit syscall
    movq $60, %rax
    xor %rdi, %rdi
    syscall
