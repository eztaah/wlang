!int <64> main()
{
    !int <64> a = 0;
    !int <64> b = 1;
    if (a) {
        b = 1;
        ret b;
    }
    else {
        if (~b) {
            b = 2;
        }
        ret b;
    } 
}

: expected exit code : 2