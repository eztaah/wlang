!int <64> main()
{
    srand();

    !int <64> secret_number = rand() % 100 + 1;  : Générer un nombre aléatoire entre 1 et 100
    print_number(secret_number);

    !int <64> guess = 0;

    !ascii <64> prompt_msg[18] = "Enter your guess; ";
    !ascii <64> more_msg[11] = "It is more\n";
    !ascii <64> less_msg[11] = "It is less\n";
    !ascii <64> congrats_msg[48] = "Congratulations! You have found the right price\n";
    !ascii <64> range_msg[42] = "Guess the number (between 1 and 100);    \n";

    print_ascii_array(range_msg, 42);

    loop {
        print_ascii_array(prompt_msg, 18);

        : Lecture du nombre saisi par l utilisateur
        !ascii& <64> buffer[10];
        input(buffer, 10);

        guess = ascii_array_to_int(buffer, 10);

        if (guess < secret_number) {
            print_ascii_array(more_msg, 11);
        } 
        else {
            if (guess > secret_number) {
                print_ascii_array(less_msg, 11);
            } 
            else {
                print_ascii_array(congrats_msg, 48);
                break;
            }
        } 
    }

    ret 0;
}
