<64> main()
{
    !wstr& <64> result[4];
    int_to_wstr(result, 4, 1234);

    print_wstr(result, 20);

    ret 0;
}

: expected output : 1234