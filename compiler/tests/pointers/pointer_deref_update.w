!int <64> main()
{
    !int <64> x = 5;
    !int& <64> p = &x;
    ^p = 15;

    ret x;  
}

: expected exit code : 15