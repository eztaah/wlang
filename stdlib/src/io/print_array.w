glb !void print_array(!int& <64> array_addr, !int <64> array_size)
{
    !int <64> i = 1;
    loop {
        !int <64> number = ^(array_addr - i*8);
        _print_number_no_newline(number);  

        i = i + 1;
        if (i == array_size + 1) {
            break;
        }  
    }
    _print_newline();
}
