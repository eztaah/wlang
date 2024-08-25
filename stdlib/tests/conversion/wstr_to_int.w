<64> main()
{
    !wstr& <64> str[5] = "1234\n";
    <64> result = wstr_to_int(str, 5);
    print_num(result);

    ret 0;
}

: expected output : 1234