!int <64> main()
{
    !int <64> x = 10;
    !int& <64> p = &x;
    !int <64> y = ^p;

    ret y; 
}

: expected exit code : 10