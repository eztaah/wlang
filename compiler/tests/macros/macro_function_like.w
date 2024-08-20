#def MAX(a, b) \
    if ((a) > (b)) \
        (a); \
    else \
        (b);

!int <64> main()
{
    !int <64> a = 10;
    !int <64> b = 20;
    !int <64> result = MAX(a, b);
    ret result;
}

: expected exit code : 20