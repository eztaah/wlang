!void print_digit(!digit <64> num)
{
    !ascii <64> asciicode = num + 48;
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);
    !ascii <64> new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}

glb !int <64> add(!int <64> a, !int <64> b)
{
    ret a + b;
}

!int <64> main()
{
    !int <64> result = add(2, 3);
    print_digit(result);

    ret 0;
}
