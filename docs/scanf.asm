section .data
    inputFormat db "%d", 0
    outputFormat db "Vous avez saisi : %d", 10, 0

section .bss
    number resb 4

section .text
    global _start

extern scanf
extern printf
extern exit

_start:
    ; scanf
    mov rdi, inputFormat
    mov rsi, number
    call scanf

    ; printf
    mov rdi, outputFormat
    mov rsi, [number]
    call printf

    ; exit
    mov rdi, 0
    call exit
