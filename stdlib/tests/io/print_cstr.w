<64> main()
{
    : create a cstr
    !cstr& <64> cstr = malloc(5);
    ^(cstr + 0) = 'h';
    ^(cstr + 1) = 'e';
    ^(cstr + 2) = 'l';
    ^(cstr + 3) = 'l';
    ^(cstr + 4) = 'o';

    print_cstr(cstr, 5);

    ret 0;
}

: input : echo hello
: expected output : hello