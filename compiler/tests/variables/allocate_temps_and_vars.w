!int <64> main()
{
    !int <64> a = 2147483646;
    !int <64> b = 0;
    !int <64> c = a / 6 + ~b;
    ret c * 2 == a - 1431655762;
}

: expected exit code : 0