!void initialize_array(!ascii& <64> array_addr, !int <64> array_size, !ascii <64> value)
{
    !int <64> i = 1;

    loop {
        ^(array_addr - i*8) = value;

        i = i + 1;
        if (i == array_size + 1) {
            break;
        }
    }
}

!int <64> main()
{
    !ascii <64> arr[1000];
    initialize_array(arr, 1000, 123);
    ret ^(arr - 999*8);
}

: expected exit code : 123