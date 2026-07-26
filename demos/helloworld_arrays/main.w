! include print_string() from std

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

!void print_array(!int& <64> array_addr, !int <64> array_size)
{
    : allocate memory for the string
    !ascii& <64> str_addr = @malloc(array_size*8);

    !int <64> i = 0;
    loop {
        ^(str_addr + i*8) = ^(array_addr - i*8);  

        i = i + 1;
        if (i == array_size) {
            break;
        }  
    }

    : print the string
    %sysc(1, 1, str_addr, array_size*8, 0, 0, 0);

    @free(str_addr);
}

!int <64> main()
{
    : Define the string "Hello"
    !ascii <64> str[7] = [72, 101, 108, 108, 111, 10, 0];

    : Print the string
    print_array(&str, 7);

    ret 0;
}
