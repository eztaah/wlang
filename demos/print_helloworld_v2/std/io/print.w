!ascii digit_to_ascii(!digit digit) 
{   
    ret digit + 48;
}

:Convert a digit to its corresponding ASCII code and print it.
glb !void print_digit(!digit num)
{
    :convert the number to its ASCII code
    !ascii asciicode = digit_to_ascii(num);

    :print the ASCII character (using write syscall)
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);

    :print a newline character (using write syscall)
    !ascii new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}

glb !void print_string(!string& string_addr, !int length)
{
    :print the string (using write syscall)
    %sysc(1, 1, string_addr, length, 0, 0, 0);
}