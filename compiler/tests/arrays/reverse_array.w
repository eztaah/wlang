!void reverse_array(!ascii& <64> array_addr, !int <64> array_size)
{
    !int <64> i = 1;
    !int <64> j = array_size;

    loop {
        if (i >= j) {
            break;
        }

        !ascii <64> temp = ^(array_addr - i*8);
        ^(array_addr - i*8) = ^(array_addr - j*8);
        ^(array_addr - j*8) = temp;

        i = i + 1;
        j = j - 1;
    }
}

!int <64> main()
{
    !ascii <64> arr[5] = [1, 2, 3, 4, 5];
    reverse_array(arr, 5);
    ret ^(arr - 1*8);
}

: expected exit code : 5