!int <64> main()
{
    !int <64> a = 0;
    if (a) {
        ret 1;
    }
    else {
        ret 2;
    }
}

: expected exit code : 2