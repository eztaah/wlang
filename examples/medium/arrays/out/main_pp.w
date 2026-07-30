 printuu()
{
     <64> bonjour = 2;

    ret ;

}

 <64> main()
{
     <64> arr[3] = [1, 2, 3];

     <64> i = 1;
    loop {
        print_number(^(arr - i*8));

        i = i + 1;
        if (i == 4) {
            break;
        }
    }

    printuu();

    ret 0;
}


