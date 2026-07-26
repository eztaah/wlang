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

#def ARRAY_SIZE 1000

!int <64> main()
{
    !int <64> array[ARRAY_SIZE];
    !int <64> i = 0;

    : intitialisation 
    loop {
        ^(array - i*8) = 7;

        i = i + 1;
        if (i == ARRAY_SIZE) {
            break;
        }
    }

    i = 0;
    loop {
        print_digit(^(array - i*8));

        i = i + 1;
        if (i == ARRAY_SIZE) {
            break;
        }
    }

    ret 0;
}
