!int <64> main()
{
    !int <64> var_to_shift = 1234;
    !int <64> x = 0;
    x = var_to_shift >> 4;
    ret x;
}

: expected exit code : 77