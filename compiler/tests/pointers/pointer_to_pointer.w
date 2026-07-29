!int <64> main()
{
    !int <64> x = 100;
    !int& <64> p1 = &x;
    !int&& <64> p2 = &p1;

    ret ^^p2; 
}

: expected exit code : 100