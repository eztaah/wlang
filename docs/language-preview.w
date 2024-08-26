: convert a digit to its corresponding ASCII code and print it
#def STDOUT 1
print_digit(!digt <64> digt)
{
    : validate digit is between 0 and 9
    if (digt < 0 || digt > 9) {
        !wstr& <64> err_msg[21] = "error: invalid digit\n";
        print_wstr(err_msg, 21);    : function from wlang stdlib
        exit(1);
    }

    : convert the number to its ASCII code
    !aiic <64> aiic = digt + 48;

    : print the ASCII character (using write syscall)
    %sysc(1, STDOUT, &aiic, 1, 0, 0, 0);
}

<64> add(<64> a, <64> b)
{
    ret a + b;
}

<64> main()
{
#ifdef DEV_MODE
    : print debug message if DEV_MODE is enabled
    !wstr& <64> debug_msg[27] = "Debug mode: Running in DEV_MODE\n";
    print_wstr(debug_msg, 27);
#endif

    : define and initialize two integers
    <64> num1 = 3;
    <64> num2 = 5;

    : compute the sum and print the result
    <64> sum = add(num1, num2);
    print_digit(sum);

    : create a string
    <64> str1[6] = "hello\n";
    ^(str1 - 2*8) = 'a';    : replace 'e' with 'a'

    : exit the program with code 0
    : alternatively, use the exit() function from libw
    %sysc(60, 0, 0, 0, 0, 0, 0);  
}



