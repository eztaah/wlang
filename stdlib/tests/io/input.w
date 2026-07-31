!int <64> main()
{
    !ascii& <64> buffer[10];
    input(buffer, 10);

    print_ascii_array(buffer, 10);

    ret 0;
}

: input : echo 5678
: expected output : 5678\n