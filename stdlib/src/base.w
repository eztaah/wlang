glb !void exit(!int <64> status)
{
    %sysc(60, status, 0, 0, 0, 0, 0);
}

glb !ascii <64> digit_to_ascii(!digit <64> num)
{
    if (num < 0 || num > 9) {
        exit(1);
    }

    ret num + 48;
}

glb !digit <64> ascii_to_digit(!ascii <64> ascii_code)
{
    if (ascii_code < 48 || ascii_code > 57) {
        exit(1);
    }

    ret ascii_code - 48;
}

glb !int <64> ascii_array_to_int(!ascii& <64> ascii_array, !int <64> array_size)
{
    !int <64> result = 0;
    !int <64> i = 1;

    loop {
        if (i == array_size + 1) {
            break; 
        }

        !ascii <64> current_char = ^(ascii_array - i*8);


        : Check for null terminator or newline character
        if (current_char == '\0' || current_char == '\n') {
            break;
        }

        !digit <64> current_digit = ascii_to_digit(current_char);
        result = result * 10 + current_digit;


        i = i + 1;
    }

    ret result;
}

#def MAX_DIGIT 20
glb !ascii& <64> int_to_ascii_array(!int <64> number)
{
    !ascii& <64> ascii_array[MAX_DIGIT];

    !int <64> i = 1;

    : Gérer le cas des nombres négatifs
    !ascii <64> is_negative = 0;
    if (number < 0) {
        is_negative = 1;
        number = -number;
    }

    : Conversion des chiffres en ASCII en commençant par les moins significatifs
    loop {
        !digit <64> current_digit = number % 10;
        !ascii <64> ascii_char = digit_to_ascii(current_digit);

        ^(ascii_array - i*8) = ascii_char;
        number = number / 10;
        i = i + 1;

        if (number == 0) {
            break;
        }
    }

    : Ajouter le signe négatif si nécessaire
    if (is_negative) {
        ^(ascii_array - i*8) = '-';
        i = i + 1;
    }

    : Reverse the string because digits were added from right to left
    !int <64> start = 1;
    !int <64> end = i - 1;  : i points to the next free position

    loop {
        if (start >= end) {
            break;
        }

        !ascii <64> temp = ^(ascii_array - start*8);
        ^(ascii_array - start*8) = ^(ascii_array - end*8);
        ^(ascii_array - end*8) = temp;

        start = start + 1;
        end = end - 1;
    }

    : Fill the remaining array with \0 characters after reversing
    loop {
        if (i >= MAX_DIGIT) {
            break;
        }
        ^(ascii_array - i*8) = '\0';
        i = i + 1;
    }

    ret ascii_array;
}
