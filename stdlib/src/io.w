!void _print_digit(!digit <64> num)
{
    if (num > 9) {
        @exit(1);
    }

    !ascii <64> asciicode = digit_to_ascii(num);
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);
}

!void _print_newline()
{
    !ascii <64> new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}

!void _print_number_recursive(!int <64> num)
{
    if (num >= 10) {
        _print_number_recursive(num / 10);
    }

    !int <64> digit = num % 10;
    _print_digit(digit);
}

!void _print_number_no_newline(!int <64> num)
{
    if (num < 0) {
        @exit(-1);
    }
    if (num == 0) {
        _print_digit(0);
    } 
    else {
        _print_number_recursive(num);
    }
}

glb !void print_libw_version()
{
    !ascii <64> libw_version[19] = "libw version 0.3.0\n";
    print_ascii_array(libw_version, 19);
}

glb !void print_ascii(!ascii <64> ascii)
{
    %sysc(1, 1, &ascii, 1, 0, 0, 0);
}

glb !void print_array(!int& <64> array_addr, !int <64> array_size)
{
    : Print opening bracket
    !ascii <64> open_bracket = '[';
    print_ascii(open_bracket);

    !int <64> i = 1;
    loop {
        !int <64> number = ^(array_addr - i*8);
        _print_number_no_newline(number);

        : Print comma and space if not the last element
        if (i != array_size) {
            !ascii <64> comma = ',';
            !ascii <64> space = ' ';  
            print_ascii(comma);
            print_ascii(space);
        }

        i = i + 1;
        if (i == array_size + 1) {
            break;
        }  
    }

    : Print closing bracket and newline
    !ascii <64> close_bracket = ']'; 
    !ascii <64> newline = '\n';
    print_ascii(close_bracket);
    print_ascii(newline);
}

glb !void print_number(!int <64> num)
{
    if (num < 0) {
        exit(-1);
    }
    if (num == 0) {
        _print_digit(0);
    } 
    else {
        _print_number_recursive(num);
    }

    _print_newline();
}

glb !void print_ascii_array(!int& <64> array_addr, !int <64> array_size)
{
    : allocate memory for the string
    !ascii& <64> str_addr = malloc(array_size);

    !int <64> i = 0;
    loop {
        ^(str_addr + i) = ^(array_addr - (i+1)*8);  

        i = i + 1;

#ifndef PRINT_NULL_CHAR
        : Stop if a null character is encountered
        if (^(str_addr + (i-1)) == '\0') {
            break;
        }
#endif

        if (i == array_size + 1) {
            break;
        }  
    }

    : print the string
    %sysc(1, 1, str_addr, i - 1, 0, 0, 0);  : 'i - 1' because 'i' is incremented after the last valid char

    free(str_addr, array_size);
}

#def MAX_SIZE 100
glb !ascii& <64> input()
{
    : allocate memory for the input string (temporarily stores 8-bit characters)
    !ascii& <64> buffer = malloc(MAX_SIZE);

    : read the input string from stdin
    @read(0, buffer, MAX_SIZE);

    : allocate memory for the output array (64-bit per element)
    !void& <64> str_array_addr[MAX_SIZE];

    : copy and expand each 8-bit character to 64 bits
    !int <64> i = 0;
    loop {
        if (^(buffer + i) == 0 || i == MAX_SIZE) {
            break;  : stop if we reach the end of the string or the maximum size
        }

        ^(str_array_addr - (i+1) * 8) = ^(buffer + i);  : expand 8-bit to 64-bit

        i = i + 1;
    }

    : free the temporary memory
    free(buffer, MAX_SIZE);

    ret str_array_addr;  : return the address of the 64-bit array
}
