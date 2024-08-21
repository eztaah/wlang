!void multiply_array(!ascii& <64> array_addr, !int <64> array_size, !int <64> scalar)
{
    !int <64> i = 1;

    loop {
        ^(array_addr - i*8) = ^(array_addr - i*8) * scalar;

        i = i + 1;
        if (i == array_size + 1) {
            break;
        }
    }
}

!int <64> main()
{
    !ascii <64> arr[3] = [1, 2, 3];
    multiply_array(arr, 3, 10);

    ret ^(arr - 2 * 8); 
}

: expected exit code : 20