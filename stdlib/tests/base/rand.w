<64> main()
{
    srand();

    <64> rand1 = rand();
    <64> rand2 = rand();
    <64> rand3 = rand();

    if (rand1 != rand2 || rand2 != rand3 || rand1 != rand3) {
        ret 1;
    } 
    else {
        ret 0;
    }
}

: expected exit code : 1