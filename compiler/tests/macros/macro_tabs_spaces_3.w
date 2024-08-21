#def	MULTIPLIER	3
#def	USE_MULTIPLIER

!int <64> main()
{
    #ifdef	USE_MULTIPLIER
        !int <64> val = 5 *	MULTIPLIER	;
    #else
        !int <64> val = 5	;
    #endif

    ret val;
}

: expected exit code : 15