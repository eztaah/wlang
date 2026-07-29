!int <64> main()
{
    !int <64> a = 15;
    !int <64> b = a # 5;  
    ret 1 | b;   
}

: expected exit code : 11