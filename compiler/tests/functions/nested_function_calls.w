!int <64> multiply(!int <64> a, !int <64> b) 
{
    ret a * b;
}

!int <64> add(!int <64> a, !int <64> b) 
{
    ret a + b;
}

!int <64> main() 
{
    !int <64> a = multiply(2, 3);
    !int <64> b = multiply(4, 5);
    ret add(a, b); 
}

: expected exit code : 26