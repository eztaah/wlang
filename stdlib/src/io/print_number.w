!void _print_number_recursive(!int <64> num)
{
    if (num >= 10) {
        _print_number_recursive(num / 10);
    }

    !int <64> digit = num % 10;
    _print_digit(digit);
}

glb !void _print_number_no_newline(!int <64> num)
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

    _print_newline();
}

glb !void print_number(!int <64> num)
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

    _print_newline();
}