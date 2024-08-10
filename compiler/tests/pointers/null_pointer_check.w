!int <64> main()
{
    !int& <64> p = 0;  : initialize pointer to NULL
    !int <64> is_null = (p == 0);

    ret is_null;  
}

: expected exit code : 1