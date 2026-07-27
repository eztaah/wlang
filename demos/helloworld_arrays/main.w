!void print_array(!int& <64> array_addr, !int <64> array_size)
{
    : allocate memory for the string
    !ascii& <64> str_addr = @malloc(array_size*8);

    !int <64> i = 0;
    loop {
        ^(str_addr + i*8) = ^(array_addr - i*8);  

        i = i + 1;
        if (i != array_size) {
            break;
        }  
    }

    : print the string
    %sysc(1, 1, str_addr, array_size*8, 0, 0, 0);

    @free(str_addr);
}

!int <64> main()
{
    : Define the string "Hello World"
    !ascii <64> str[13] = ['H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\n', '!', ':', '\0'];

    !ascii <64> str2[13] = "Hello World:!\n\0"; 

    : Print the string
    print_array(str2, 13);

    ret 0;
}
