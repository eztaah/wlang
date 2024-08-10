!int <64> main()
{
    !int <64> sum = 0;
    !int <64> i = 1;

    loop {
        sum = sum + i;
        i = i + 1;

        if (i > 10) {
            break;
        }
    }

    ret sum;
}

: expected exit code : 55