!int <64> main()
{
    !void& <64> mem = malloc(8);
    ^(mem + 0) = 42;
    print_number(^(mem + 0));
    free(mem, 8);
    ret 0;
}

: expected output : 42\n
