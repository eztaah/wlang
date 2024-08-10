!int <64> main()
{
    !int <64> a = 0;
    !int <64> b = 0;

    if (a) {
        a = 2;
    } else {
        a = 3;
    }

    if (b) {
        b = 4;
    } else {
        b = 5;
    }

    ret a + b;
}

: expected exit code : 8