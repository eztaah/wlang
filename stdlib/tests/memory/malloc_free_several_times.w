<64> main()
{
    : First allocation and use
    !void& <64> mem1 = malloc(8);
    ^(mem1 + 0) = 10;
    ^(mem1 + 8) = 20;
    free(mem1, 8);

    : Second allocation and use
    !void& <64> mem2 = malloc(16);
    ^(mem2 + 0) = 30;
    ^(mem2 + 8) = 40;
    free(mem2, 16);

    : Third allocation and use
    !void& <64> mem3 = malloc(8);
    ^(mem3 + 0) = 50;
    ^(mem3 + 8) = 60;
    free(mem3, 8);

    ret 0;
}

: expected output : SKIP
