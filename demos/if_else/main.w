!void print_digit(!digit <64> num)
{
    !ascii <64> asciicode = num + 48;

    : print
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);
    !ascii <64> new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}

!int <64> main()
{
    !int <64> num1 = -10;
    !int <64> num2 = 1;

    if ((num1 < 0) || num1 == -2) {
        print_digit(0);
    }
    else {
        if (num1 == 1 && num2 == 1) {
            print_digit(1);
        }
        else {
            print_digit(2);
        }
    }

    : !int <64> i = 0;
    : loop {
    :     print_digit(i);
    :     i = i + 1;

    :     if (i >= 9) {
    :         break;
    :     }
    : }

    ret 0;
}
