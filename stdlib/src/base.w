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

glb !int <64> ascii_array_to_int(!ascii& <64> str_array, !int <64> array_size)
{
    !int <64> result = 0;
    !int <64> i = 1;

    loop {
        if (i == array_size + 1) {
            break; 
        }

        !ascii <64> current_char = ^(str_array - i*8);
        !digit <64> current_digit = ascii_to_digit(current_char);

        result = result * 10 + current_digit;
        i = i + 1;
    }

    ret result;
}

glb !ascii& int_to_ascii_array(!int number)
{

}