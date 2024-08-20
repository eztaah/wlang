glb !void print_digit(!digit <64> num)
{
    !ascii <64> asciicode = num + 48;
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);
}

glb !void print_new_line()
{
    !ascii <64> new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}

glb !void print_number_recursive(!int <64> num)
{
    if (num >= 10) {
        print_number_recursive(num / 10);
    }

    !int <64> digit = num % 10;
    print_digit(digit);
}

glb !void print_number(!int <64> num)
{
    if (num < 0) {
        @exit(-1);
    }
    if (num == 0) {
        print_digit(0);
    } 
    else {
        print_number_recursive(num);
    }

    print_new_line();
}

!int <64> main()
{
    !int <64> num = -27832374;
    print_number(num);

    ret 0;
}

