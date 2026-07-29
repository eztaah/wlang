!int <64> main()
{
    !int <64> count = 0;

    loop {
        count = count + 1;

        if (count >= 5) {
            break;
        }
    }

    ret count;
}

: expected exit code : 5