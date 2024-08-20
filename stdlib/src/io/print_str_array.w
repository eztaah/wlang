glb !void print_str_array(!int& <64> array_addr, !int <64> array_size)
{
    : allocate memory for the string
    !ascii& <64> str_addr = @malloc(array_size);

    !int <64> i = 0;
    loop {
        ^(str_addr + i) = ^(array_addr - (i+1)*8);  

        i = i + 1;
        if (i == array_size + 1) {
            break;
        }  
    }

    : print the string
    %sysc(1, 1, str_addr, array_size, 0, 0, 0);

    @free(str_addr);
}
