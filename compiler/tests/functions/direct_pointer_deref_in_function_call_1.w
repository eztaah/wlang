!int <64> double_value(!int <64> value)
{
    ret value * 2;
}

!int <64> main()
{
    !int <64> num = 5;
    !int <64> result = double_value(^&num);
    ret result; 
}

: expected exit code : 10
