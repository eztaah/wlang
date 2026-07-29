!int <64> main()
{
#ifdef VALUE
    ret VALUE;
#else
    ret 0;  : should ret 0 (because VALUE is undefined)
#endif
}


: expected exit code : 0