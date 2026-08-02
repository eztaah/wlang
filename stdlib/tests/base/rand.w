!int <64> main()
{
    srand();

    !int <64> rand1 = rand();
    !int <64> rand2 = rand();
    !int <64> rand3 = rand();

    : On retourne 1 si au moins deux nombres sont différents, sinon 0.
    if (rand1 != rand2 || rand2 != rand3 || rand1 != rand3) {
        ret 1;
    } 
    else {
        ret 0;
    }
}

: expected exit code : 1