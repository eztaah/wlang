!ascii <64> digit_to_ascii(!digit <64> digit) 
{   
    ret digit + 48;
}

:Convert a digit to its corresponding ASCII code and print it.
glb !void print_digit(!digit <64> num)
{
    :convert the number to its ASCII code
    !ascii <64> asciicode = digit_to_ascii(num);

    :print the ASCII character (using write syscall)
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);

    :print a newline character (using write syscall)
    !ascii <64> new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}