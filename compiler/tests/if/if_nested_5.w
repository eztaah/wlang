!int <64> main()
{
    !int <64> a = 0;
    if (0) {
        if (0) {
            a = 3;
        } else {
            a = 4;
        }
    } else {
        a = 1;
    }

    ret a;
}


: expected exit code : 1