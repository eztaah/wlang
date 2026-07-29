!int <64> main()
{
    !int <64> a = -2593;
    a = a % 3;
    !int <64> b = -a;
    ret b;
}

: expected exit code : 1