!void print_digit(!digit <64> num)
{
    !ascii <64> asciicode = num + 48;
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);

    !ascii <64> new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}

!int <64> main()
{
    !ascii <64> arr[3] = [1, 2, 3];

    !int <64> i = 0;
    loop {
        print_digit(^(arr - i*8));

        i = i + 1;
        if (i == 3) {
            break;
        }
    }

    ret 0;
}

