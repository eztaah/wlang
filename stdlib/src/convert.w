: converts a digit (0-9) to its corresponding ASCII code.
:
: this function takes a digit as input and returns its ASCII code equivalent.
: The `digt` parameter must be within the range [0, 9]. 
:
: the ASCII code for digits starts from 48 (corresponding to '0') and ends
: at 57 (corresponding to '9').
:
: if the input digit  is outside this range, the function will print an error message and 
: terminate the program.
glb !aiic <64> digt_to_aiic(!digt <64> digt)
{
    if (digt < 0 || digt > 9) {
        !wstr& <64> err_msg[64] = "error: libw/base/digt_to_aiic: invalid digit, must be between 0 and 9\n";
        print_wstr(err_msg, 64);
        exit(1);
    }

    ret digt + 48;
}

: converts an ASCII code to the corresponding digit (0-9).
:
: this function converts a given ASCII code to the corresponding digit it
: represents. The `aiic_code` parameter must be an ASCII code within the range
: [48, 57], which corresponds to the characters '0' to '9'.
:
: the conversion subtracts 48 from the ASCII code to obtain the corresponding digit.
:
: if the input ASCII code is outside this range, the function will terminate the program.
glb !digt <64> aiic_to_digt(!aiic <64> aiic)
{
    if (aiic < 48 || aiic > 57) {
        !wstr& <64> err_msg[65] = "error: libw/base/aiic_to_digt: invalid ASCII code, must be between 48 and 57\n";
        print_wstr(err_msg, 65);
        exit(1);
    }

    ret aiic - 48;
}

: converts a wstr to a 64-bits integer
:
: this function processes a wstr (which is a custom string type where each ASCII code is stored on 64 bits)
: and converts it into an integer. the function iterates over each character of the wstr, converts it 
: to its corresponding digit, and accumulates the result. 
: 
: the process stops when it encounters a null terminator (\0) or a newline character (\n), 
: or when the end of the string is reached. 
glb <64> wstr_to_int(!wstr& <64> wstr, <64> str_size)
{
    <64> result = 0;
    <64> i = 1;
    loop {
        if (i == str_size + 1) {
            break; 
        }

        !aiic <64> current_aiic = ^(wstr - i*8);

        : the conversion should stop if a \0 or \n is encontered
        if (current_aiic == '\0' || current_aiic == '\n') {
            break;
        }

        !digt <64> current_digt = aiic_to_digt(current_aiic);
        result = result * 10 + current_digt;

        i = i + 1;
    }

    ret result;
}

: converts an integer to a wstr
:
: this function takes an integer and converts it into its wstr equivalent. 
: the wstr returned by the function handles negative numbers by adding a '-' sign at the beginning. 
: digits are converted from the least significant to the most significant, then the string 
: is reversed to correct the order.
: 
: the returned string will always have the size MAX_DIGIT because it is not possible to intialize an array with a value that is not known at compile time
#def MAX_DIGIT 20
glb !wstr& <64> int_to_wstr(<64> num)
{
    !wstr& <64> wstr[MAX_DIGIT];

    : handle negative numbers
    <64> is_negative = 0;
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    <64> i = 1;
    loop {
        : ensure the number of digits does not exceed MAX_DIGIT
        if (i > MAX_DIGIT - is_negative) {
            !wstr& <64> err_msg[64] = "error: libw/base/int_to_wstr: number exceeds MAX_DIGIT\n";
            print_wstr(err_msg, 64);
            exit(1);
        }

        : Convert the current digit to ASCII and store it
        !digt <64> current_digt = num % 10;
        !aiic <64> aiic = digt_to_aiic(current_digt);

        ^(wstr - i*8) = aiic;
        num = num / 10;
        i = i + 1;

        : stop if there are no more digits
        if (num == 0) {
            break;
        }
    }

    : add the negative sign if necessary
    if (is_negative) {
        ^(wstr - i*8) = '-';
        i = i + 1;
    }

    : reverse the string because digits were added from right to left
    <64> start = 1;
    <64> end = i - 1;

    loop {
        if (start >= end) {
            break;
        }

        !aiic <64> temp = ^(wstr - start*8);
        ^(wstr - start*8) = ^(wstr - end*8);
        ^(wstr - end*8) = temp;

        start = start + 1;
        end = end - 1;
    }

    : fill the remaining array with \0 characters after reversing
    loop {
        if (i >= MAX_DIGIT) {
            break;
        }
        ^(wstr - i*8) = '\0';
        i = i + 1;
    }

    ret wstr;
}

: converts a wstr to a standard C string (cstr)
:
: this function converts a wstr, where each ASCII code is stored on 64 bits, into a cstr, 
: where each ASCII code is stored on 8 bits. 
: the function stops copying if a null character (\0) is encountered
: 
: the function will always output a string that is null-terminated, even if the wstr is not
glb !cstr& wstr_to_cstr(!wstr& <64> wstr, <64> str_size)
{
    : allocate memory for the string
    !cstr& <64> cstr = malloc(str_size);

    <64> i = 0;
    loop {
        ^(cstr + i) = ^(wstr - (i+1)*8);  

        : stop if a null character is encountered
        if (^(cstr + i) == '\0') {
            break;
        }

        if (i == str_size + 1) {
            : if we stop because we reached the size of wstr, ensure the cstr is null-terminated
            ^(cstr + i) = '\0';
            break;
        }  

        i = i + 1;
    }


    ret cstr;
}

glb cstr_to_wstr(!cstr& <64> cstr, !wstr& wstr_buffer, <64> str_size)
{
    : copy and expand each 8-bit character to 64 bits
    <64> i = 1;
    loop {
        if (i >= str_size) {
            break;  
        }

        !aiic <64> char_8bit = ^(cstr + i - 1);
        !aiic <64> char_64bit = char_8bit & 0xFF;  : Mask to ensure only the lower 8 bits are kept
        
        : Store the 64-bit value in the array
        ^(wstr_buffer - i * 8) = char_64bit;

        i = i + 1;
    }

    : the cstr was forcement créé avec un malloc
    free(cstr, str_size);
}