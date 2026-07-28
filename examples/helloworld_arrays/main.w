!void print_array(!int& <64> array_addr, !int <64> array_size)
{
    : allocate memory for the string
    !ascii& <64> str_addr = @malloc(array_size);
    print_number(str_addr);

    !int <64> i = 0;
    loop {
        ^(str_addr + i) = ^(array_addr - i*8);  

        i = i + 1;
        if (i == array_size) {
            break;
        }  
    }

    : print the string
    %sysc(1, 1, str_addr, array_size, 0, 0, 0);

    @free(str_addr);
}

!int <64> main()
{
    : Define the string "Hello World"
    !ascii <64> str[15] = ['H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\n', '!', ':', '\0'];

    !ascii <64> str2[15] = "Hello World:!\n\0"; 

    print_number(str2);

    : Print the string
    print_array(str2, 15);

    ret 0;
}