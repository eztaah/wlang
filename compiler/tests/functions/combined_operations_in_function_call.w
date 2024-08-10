!int <64> calculate(!int <64> a, !int <64> b, !int <64> c)
{
    ret (a + b) * c;
}

!int <64> main()
{
    !int <64> x = 2;
    !int <64> y = 3;
    !int <64> z = 4;
    !int <64> result = calculate(x * y, y + z, z - 1);
    ret result;
}

: expected exit code : 39