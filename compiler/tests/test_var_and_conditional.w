!void print_digit(!digit <64> num)
{
    !ascii <64> asciicode = num + 48;
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);
    !ascii <64> new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}

!int <64> main()
{
    !int <64> num = 1;

    if (num == 1) {
        print_digit(1);
    }
    else {
        print_digit(0);
    }

    ret 0;
}
