!int <64> sum(!int <64> a, !int <64> b, !int <64> c, !int <64> d, !int <64> e, !int <64> f) 
{
    ret a + b + c + d + e + f;
}

!int <64> main() 
{
    ret sum(1, 2, 3, 4, 5, 6);
}

: expected exit code : 21