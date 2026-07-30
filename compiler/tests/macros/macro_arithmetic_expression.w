: The language does not support function-like macros with arguments.
: However, it is possible to use such macros in a limited way as demonstrated here.
: Note that MULTIPLY_BY_3(7) will not work because the preprocessor will only replace MULTIPLY_BY_3(x) exactly as written.

#def MULTIPLY_BY_3(x) ((x) * 3)

!int <64> main()
{
    !int <64> x = 7;
    !int <64> result = MULTIPLY_BY_3(x);
    ret result;
}

: expected exit code : 21