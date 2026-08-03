#def DEBUG_MODE

!int <64> main()
{
#ifdef DEBUG_MODE
    ret 1;
#endif

    ret 0;
}

: expected exit code : 1