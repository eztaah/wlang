!int <64> main()
{
    !int <64> a = 10;
    !int <64> b = 20;

    b = -20;

    loop {
        if (b >= 0) {
            break;
        }

        a = a - 1;
        b = b + 1;

        if (a <= 0) {
            break;
        }
    }

    ret a == 0 && b == -11;
}

: expected exit code : 1