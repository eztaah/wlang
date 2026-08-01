#def STDIN_FILENO 0
#def STDOUT_FILENO 1
#def STDERR_FILENO 2

_write(<64> fd, !cstr& <64> buf, <64> count)
{
#ifdef C_WRITE
    <64> result = @write(fd, buf, count);
#else
    <64> result = %sysc(1, fd, buf, count, 0, 0, 0);
#endif

    if (result < 0) {
        !wstr& <64> err_msg[64] = "error: libw/io/_write: write operation failed\n";
        print_wstr(err_msg, 64);
        exit(1);
    }
}

_read(<64> fd, !cstr& <64> buf, <64> count)
{
#ifdef C_READ
    <64> result = @read(fd, buf, count);
#else
    <64> result = %sysc(0, fd, buf, count, 0, 0, 0);
#endif

    if (result < 0) {
        !wstr& <64> err_msg[64] = "error: libw/io/_read: read operation failed\n";
        print_wstr(err_msg, 64);
        exit(1);
    }
}

_print_digt(!digt <64> digt)
{
    if (digt < 0 || digt > 9) {
        !wstr& <64> err_msg[64] = "error: libw/io/_print_digt: invalid digit, must be between 0 and 9\n";
        print_wstr(err_msg, 64);
        exit(1);
    }

    !aiic <64> aiic = digt_to_aiic(digt);
    _write(STDOUT_FILENO, &aiic, 1);
}

glb print_aiic(!aiic <64> aiic)
{
    _write(STDOUT_FILENO, &aiic, 1);
}

_print_num_recursive(<64> num)
{
    if (num >= 10) {
        _print_num_recursive(num / 10);
    }

    !digt <64> digt = num % 10;
    _print_digt(digt);
}

glb print_num(<64> num)
{
    if (num < 0) {
        print_aiic('-');
        _print_num_recursive(-num);
    }
    else {
        if (num == 0) {
            _print_digt(0);
        } 
        else {
            _print_num_recursive(num);
        }
    }
}

glb print_array(!array& <64> array_addr, <64> array_size)
{
    : Print opening bracket
    !aiic <64> open_bracket = '[';
    print_aiic(open_bracket);

    <64> i = 1;
    loop {
        <64> number = ^(array_addr - i*8);
        print_num(number);

        : Print comma and space if not the last element
        if (i != array_size) {
            print_aiic(',');
            print_aiic(' ');
        }

        i = i + 1;
        if (i == array_size + 1) {
            break;
        }  
    }

    : Print closing bracket and newline
    print_aiic(']');
    print_aiic('\n');
}

glb print_cstr(!cstr& <64> cstr, <64> str_size)
{
    _write(STDOUT_FILENO, cstr, str_size);
}

glb print_wstr(!wstr& <64> wstr, <64> str_size)
{
    : allocate memory for the string
    !cstr& <64> cstr = malloc(str_size);

    <64> i = 0;
    loop {
        ^(cstr + i) = ^(wstr - (i+1)*8);  

#ifndef PRINT_NULL_CHAR
        : stop if a null character is encountered
        if (^(cstr + i) == '\0') {
            break;
        }
#endif

        if (i == str_size + 1) {
            : if we stop because we reached the size of wstr, ensure the cstr is null-terminated
            ^(cstr + i) = '\0';
            break;
        }  

        i = i + 1;
    }

    : print the generated string
    print_cstr(cstr, str_size);

    free(cstr, str_size);
}

glb !void input(!cstr& <64> cstr, !int <64> str_size)
{
    : read the input string from stdin
    _read(STDIN_FILENO, cstr, str_size);

    : copy and expand each 8-bit character to 64 bits
    !int <64> i = 1;
    loop {
        if (i >= str_size) {
            break;  
        }

        : Retrieve the 8-bit character and expand to 64-bit
        !aiic <64> char_8bit = ^(cstr + i - 1);
        !aiic <64> char_64bit = char_8bit & 0xFF;  : Mask to ensure only the lower 8 bits are kept
        
        : Store the 64-bit value in the array
        ^(cstr - i * 8) = char_64bit;

        i = i + 1;
    }

    ret cstr;
}
