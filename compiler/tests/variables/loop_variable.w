!int <64> main()
{
    srand();

    !int <64> secret_number = rand() % 100 + 1;  : Générer un nombre aléatoire entre 1 et 100

    print_number(secret_number);
    !int <64> guess = 0;
    !int <64> attempts = 0;


    !ascii <64> prompt_msg[18] = "Enter your guess; ";
    !ascii <64> more_msg[11] = "It is more\n";
    !ascii <64> less_msg[11] = "It is less\n";
    !ascii <64> congrats_msg[47] = "Congratulations! You have found the right price\n";
    !ascii <64> range_msg[32] = "Guess the number (between 1 and\n";

    loop {
        : Lecture du nombre saisi par l utilisateur
        !ascii& <64> buffer[10];
        input(buffer, 10);

        : convertir str_array en entier
        guess = ascii_array_to_int(buffer);

        attempts = attempts + 1;

        print_number(guess);

        : if (guess < secret_number) {
        :     print_ascii_array(more_msg, 11);
        : } 
        : else {
        :     if (guess > secret_number) {
        :         print_ascii_array(less_msg, 11);
        :     } 
        :     else {
        :         print_ascii_array(congrats_msg, 48);
        :         : break;
        :     }
        : } 
    }

    ret 0;
}

: input : 3 
: expected output : 1234