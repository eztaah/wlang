#def DEBUG_MODE

!int <64> main()
{
#ifdef DEBUG_MODE
    !int <64> debug_val = 1;
#else
    !int <64> debug_val = 0;
#endif

    ret debug_val;  : should ret 1 (because DEBUG_MODE is defined)
}

: expected exit code : 1