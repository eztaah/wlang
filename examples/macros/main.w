#incl ./function.w
#def ASCII_START 48

: #def DEV_MODE

!int <64> main()
{
#ifdef DEV_MODE
    print_digit(1);
#else
    print_digit(2);
#endif

    ret 0;
}
