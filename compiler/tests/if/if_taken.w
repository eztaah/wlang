!int <64> main()
{
    !int <64> a = 1;
    !int <64> b = 0;
    if (a) {
        b = 1;
    }
    ret b;
}

: expected exit code : 1