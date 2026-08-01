: add arrays
: add preprocessor
: add semantic analysis
: add missing operators (>>, &, |, #, etc), ajout des unarop manquantes (~, !)
: ajout du dereferencement

#def HELLO
#def NUMBER 10

<64> main()
{
    : arrays
    !array& <64> array[4] = [1, 3, 6, 1];

    : string arrays
    !ascii_array& <64> str[13];
    str = "Hello World!\n"; 
    
    : preprocessor 
#ifdef HELLO
    <64> a = NUMBER;
#else 
    <64> a = 3;
#endif

    : dereferencement 
    !int <64> x = 5;
    !int& <64> p = &x;
    ^p = 15;

    ret 0;
}
