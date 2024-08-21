!int <64> sum_of_elements(!int <64> a, !int <64> b, !int <64> c)
{
    ret a + b + c;
}

!int <64> main()
{
    !int <64> arr[3] = [10, 20, 30];
    !int <64> result = sum_of_elements(^(arr - 1*8), ^(arr - 2*8), ^(arr - 3*8));
    ret result; 
}

: expected exit code : 60