!int <64> main()
{
    !ascii& <64> result = int_to_ascii_array(1234);
    : print_ascii_array(result, 20);

    ret 0;
}

: expected output : 1234