#def MULTIPLY_BY_3(x) ((x) * 3)

!int <64> main()
{
    !int <64> result = MULTIPLY_BY_3(7);
    ret result;  : should ret 21 (7 * 3)
}

: expected exit code : 21