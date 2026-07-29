!int <64> main()
{
    !int <64> a = 0;

    loop {
        a = a + 1;

        if (a == 100) {
            break;
        }
    }

    ret a;
}

: expected exit code : 100