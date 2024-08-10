!int <64> main()
{
    !int& <64> arr[3] = [10, 20, 30];
    !int& <64> p = arr;

    ret ^(p - 2*8);  
}

: expected exit code : 20