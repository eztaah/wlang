glb !void print_digit(!digit num)
{
    :convert the number to its ASCII code
    !ascii asciicode = num + 48;

    :print the ASCII character (using write syscall)
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);

    :print a newline character (using write syscall)
    !ascii new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}

!int main()
{
    !int <64> array[4] = [1, 2, 3, 4];
    ^(array + 2*64) = 7;

    print_digit(^(array + 0*64));
    print_digit(^(array + 1*64));
    print_digit(^(array + 2*64));
    print_digit(^(array + 3*64));

    ret 0;
}
