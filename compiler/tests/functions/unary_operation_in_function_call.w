!int <64> negate(!int <64> a)
{
    ret -a;
}

!int <64> main()
{
    !int <64> result = negate(-5);
    ret result; 
}

: expected exit code : 5