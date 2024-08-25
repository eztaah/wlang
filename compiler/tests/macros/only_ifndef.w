#def DEBUG_MODE

!int <64> main()
{
#ifndef DEBUG_MODE
    ret 1;
#endif


#ifndef DEBUG_MODE
    ret 1;
#endif


    ret 0;
}

: expected exit code : 0