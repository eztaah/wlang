!int <64> sum_array(!ascii& <64> array_addr, !int <64> array_size)
{
    !int <64> sum = 0;
    !int <64> i = 1;

    loop {
        sum = sum + ^(array_addr - i*8);

        i = i + 1;
        if (i == array_size + 1) {
            break;
        }
    }

    ret sum;
}

!int <64> main()
{
    !ascii <64> arr[4] = [10, 20, 30, 40];
    !int <64> result = sum_array(arr, 4);
    ret result;  : should ret 100 (sum of array elements)
}

: expected exit code : 100