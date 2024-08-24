#def STDIN_FILENO 0
#def STDOUT_FILENO 1
#def STDERR_FILENO 2

: performs a write operation to the specified file descriptor
:
: this function writes `count` bytes from the buffer `buf` to the file descriptor `fd`.
: it uses a system call to perform the write operation. if the write fails, it prints 
: an error message and terminates the program.
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

: performs a read operation from the specified file descriptor
:
: this function reads `count` bytes from the file descriptor `fd` into the buffer `buf`.
: it uses a system call to perform the read operation. if the read fails, it prints 
: an error message and terminates the program.
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

: prints a single digit (0-9) to the standard output
:
: this function converts a digit to its ASCII representation and prints it to 
: the standard output using the `_write` function. if the digit is outside the 
: valid range (0-9), it prints an error message and terminates the program.
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

: prints a single ASCII character to the standard output
:
: this function prints a single ASCII character (`aiic`) to the standard output 
: using the `_write` function.
glb print_aiic(!aiic <64> aiic)
{
    _write(STDOUT_FILENO, &aiic, 1);
}

: recursively prints an integer by processing each digit
:
: this function recursively processes each digit of the integer `num`, from the 
: most significant to the least significant, and prints each digit using `_print_digt`.
_print_num_recursive(<64> num)
{
    if (num >= 10) {
        _print_num_recursive(num / 10);
    }

    !digt <64> digt = num % 10;
    _print_digt(digt);
}

: prints an integer, handling negative numbers
:
: this function prints an integer `num` to the standard output. it handles negative 
: numbers by printing a minus sign first, then calling `_print_num_recursive` to print 
: the absolute value of the number. if `num` is zero, it prints `0`.
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

: prints the contents of an array as a formatted list
:
: this function prints the elements of an array, formatted as a list (e.g., `[1, 2, 3]`).
: it iterates over the array, printing each element followed by a comma and a space,
: except for the last element, which is followed by a closing bracket and a newline.
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

: prints a cstr (standard C string)
:
: this function prints a cstr (which is a standard C string where each character 
: is stored on 8 bits) to the standard output using the `_write` function.
glb print_cstr(!cstr& <64> cstr, <64> str_size)
{
    _write(STDOUT_FILENO, cstr, str_size);
}

: prints a wstr (wide string) by converting it to a cstr
:
: this function converts a wstr (where each character is stored on 64 bits) to a 
: cstr (where each character is stored on 8 bits) and then prints it to the standard 
: output. after conversion, the cstr is printed using `print_cstr` and the dynamically 
: allocated memory is freed.
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

    : free the allocated memory
    free(cstr, str_size);
}

: reads input from stdin into a cstr and converts it to a wstr
:
: this function reads a string from stdin into a cstr (where each character is stored 
: on 8 bits) and then converts it into a wstr (where each character is stored on 64 bits).
: the cstr is read using `_read`, and each character is expanded to 64 bits and stored 
: in the wstr buffer.
glb input(!cstr& <64> cstr, <64> str_size)
{
    : read the input string from stdin
    _read(STDIN_FILENO, cstr, str_size);

    : copy and expand each 8-bit character to 64 bits
    <64> i = 1;
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

