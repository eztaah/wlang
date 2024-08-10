#def VALUE 10

!int <64> main()
{
    !int <64> a = VALUE + 5;
    ret a;  : should ret 15 (10 + 5)
}

: expected exit code : 15