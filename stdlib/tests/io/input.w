<64> main()
{
    !wstr& <64> buffer[10];
    input(buffer, 10);

    print_wstr(buffer, 10);

    ret 0;
}

: input : echo 5678
: expected output : 5678\n