#def       VALUE	42
#def TEST 2

!int <64> main()
{
    !int <64> a =	VALUE   *	2 + TEST;  :mixed tabs and spaces around VALUE and operators
    ret a;  
}

: expected exit code : 84
