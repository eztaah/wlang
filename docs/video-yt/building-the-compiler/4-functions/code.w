: on ajoute les fonctions
: mise en place de la compilation multifichiers
: on peut appeler des fonctions de la libc ou de tout autre language
: introduction des annotations
: ajout du & pour avoir l'adresse

<64> add(<64> a, <64> b) 
{
    ret a + b;
}

<64> main() 
{
    : use function 
    <64> a = 1;
    <64> b = give_number();     : give_number() is defined in another source file
    <64> sum = add(a, b);

    : use libc functions
    !c_str& <64> ptr = @malloc(16);
    @puts(ptr);
    @free(ptr, 16);

    : give the adress of a variable 
    &a

    ret sum; 
}
