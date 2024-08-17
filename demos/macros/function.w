!void print_digit(!digit <64> num)
{
    :convert the number to its ASCII code
    !ascii <64> asciicode = num + ASCII_START;

    :print the ASCII character (using write syscall)
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);

    : print a newline character (using write syscall) 
    !ascii <64> new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}