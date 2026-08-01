!int <64> mutliply(!int <64> a, !int <64> b)
{
    ret a * b;
}

!int <64> do_math(!int <64> a, !int <64> b)
{
    !int <64> test = 0;
    test = mutliply(1, 2);

    ret a + b + test;
}

!int <64> main()
{
    !int <64> result = 0;
    result = do_math(2, 4);
    ret result;
}

: expected exit code : 8