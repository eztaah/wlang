!int <64> main()
{
    !ascii <64> str[5] = "1234\n";
    !int <64> result = ascii_array_to_int(str, 5);
    print_number(result);

    ret 0;
}

: expected output : 1234\n