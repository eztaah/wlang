<64> main()
{
    srand();

    <64> secret_number = rand() % 100 + 1;  : generate a random number between 1 and 100
    : print_num(secret_number);
    : print_aiic('\n');

    <64> guess = 0;

    !wstr& <64> prompt_msg[18] = "Enter your guess: ";
    !wstr& <64> more_msg[11] = "It is more\n";
    !wstr& <64> less_msg[11] = "It is less\n";
    !wstr& <64> congrats_msg[43] = "Congratulations! You have found the number\n";
    !wstr& <64> range_msg[42] = "Guess the number (between 1 and 100):    \n";

    print_wstr(range_msg, 42);

    loop {
        print_wstr(prompt_msg, 18);

        !wstr& <64> buffer[10];
        input(buffer, 10);

        guess = wstr_to_int(buffer, 10);  : convert the input string to an integer

        if (guess < secret_number) {
            print_wstr(more_msg, 11);
        } 
        else {
            if (guess > secret_number) {
                print_wstr(less_msg, 11);
            } 
            else {
                print_wstr(congrats_msg, 43);
                break;
            }
        } 
    }

    ret 0;
}
