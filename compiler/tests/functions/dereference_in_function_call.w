!int <64> increment(!int& <64> ptr)
{
    ret ^ptr + 1;
}

!int <64> main()
{
    !int <64> num = 10;
    !int <64> result = increment(&num);
    ret result;
}

: expected exit code : 11