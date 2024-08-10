!int <64> find_max(!ascii& <64> array_addr, !int <64> array_size)
{
    !int <64> max = ^(array_addr - 0*8);
    !int <64> i = 1;

    loop {
        if (i == array_size + 1) {
            break;
        }

        !int <64> current = ^(array_addr - i*8);
        if (current > max) {
            max = current;
        }

        i = i + 1;
    }

    ret max;
}

!int <64> main()
{
    !ascii <64> arr[5] = [3, 5, 7, 2, 1];
    !int <64> result = find_max(arr, 5);
    ret result; 
}

: expected exit code : 7