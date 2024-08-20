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
    ret add(multiply(2, 3), multiply(4, 5)); 
}

: expected exit code : 26