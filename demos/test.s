.section .data

.section .bss

.section .text
.global _start

_start:
    # Call w__main
    call w__main
    
    # Exit program (sys_exit)
    movq %rax, %rdi            # exit status
    movq $60, %rax             # syscall number for exit
    syscall                    # make the syscall

w__main:
    # Function prologue
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $16, %rsp          # Allocate 16 bytes on the stack

    # Allocate 1 byte with malloc
    movq    $1, %rdi           # Size: 1 byte
    call    malloc             # Call malloc
    movq    %rax, -8(%rbp)     # Store the pointer returned by malloc at -8(%rbp)

    # Store ASCII value 72 ('H') in the allocated memory
    movq    $72, (%rax)        # Store the value 72 (ASCII for 'H') at the allocated memory

    # Write the byte to stdout
    movq    $1, %rdi           # File descriptor: 1 (stdout)
    movq    -8(%rbp), %rsi     # Buffer: pointer to the allocated memory
    movq    $1, %rdx           # Count: 1 byte
    movq    $1, %rax           # Syscall number for sys_write
    syscall                    # Make the syscall

    # Return 0 (success)
    movq    $0, %rax           # Return 0
    leave                      # Function epilogue: mov %rbp, %rsp; pop %rbp
    ret                        # Return from function
