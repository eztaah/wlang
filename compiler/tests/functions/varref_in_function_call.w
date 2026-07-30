!int <64> multiply(!int <64> a, !int <64> b)
{
    ret a * b;
}

!int <64> main()
{
    !int <64> x = 4;
    !int <64> y = 7;
    !int <64> result = multiply(x, y);
    ret result;
}

: expected exit code : 28