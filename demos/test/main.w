!void print_digit(!digit <64> num)
{
    :convert the number to its ASCII code
    !ascii <64> asciicode = num + 48;

    :print the ASCII character (using write syscall)
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);

    :print a newline character (using write syscall)
    !ascii <64> new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}

!int <64> main()
{
    : !int <64> test_array[2];

    !int <64> array[3] = [1, 2, 3];
    : array + 2*64 = 7;

    print_digit(array);
    print_digit(^(^array));
    print_digit(array + 2*64);

    ret 0;
}
