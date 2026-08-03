<64> main()
{
    : create a cstr
    !cstr& <64> cstr = malloc(5);
    ^(cstr + 0) = 'h';
    ^(cstr + 1) = 'e';
    ^(cstr + 2) = 'l';
    ^(cstr + 3) = 'l';
    ^(cstr + 4) = 'o';

    !wstr& <64> wstr[6];
    cstr_to_wstr(cstr, wstr, 6);

    print_wstr(wstr, 6);

    free(cstr, 5);
    ret 0;
}
: expected output : hello