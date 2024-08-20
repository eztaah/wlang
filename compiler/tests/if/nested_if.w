!int <64> main()
{
    !int <64> a = 0;
    if (a) {
        !int <64> b = 2;
        ret b;
    } 
    else {
        !int <64> c = 3;
        if (a < c) {
            ret a;
        } 
        else {
            ret 5;
        }
    }
    ret a;
}

: expected exit code : 0