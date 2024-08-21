!void do_something() 
{
    !int <64> a = 1;

    if (a == 1) {
        ret;
    }
}

!int <64> main() 
{
    do_something();
    
    ret 0;
}

: expected exit code : 0