!ascii digit_to_ascii(!digit digit) 
{   
    ret digit + 48;
}

:Convert a digit to its corresponding ASCII code and print it.
glb !void print_digit(!digit digit)
{
    :convert the number to its ASCII code
    !ascii asciicode = digit_to_ascii(digit);

    :print the ASCII character (using write syscall)
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);

    :print a newline character (using write syscall)
    !ascii new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}


:Convert a digit to its corresponding ASCII code and print it.
glb !void print_string(!string& string_addr, !int length)
{
    :print the string (using write syscall)
    %sysc(1, 1, string_adddr, length, 0, 0, 0);
}