!int <64> add(!int <64> a, !int <64> b)
{
    ret a + b;
}

!int <64> main()
{
    !int <64> result = add(2 * 3, 4 + 5);
    ret result;
}

: expected exit code : 15