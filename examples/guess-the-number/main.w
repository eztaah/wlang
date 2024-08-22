!int <64> main()
{
        !ascii <64> hello[4] = "1234\n";

        : print_ascii_array(hello, 4);

        !int <64> guess = 0;
        guess = ascii_array_to_int(hello, 4);
        print_number(guess);

        : print_ascii_array(hello, 4);

        !int <64> test = ascii_array_to_int(hello, 4);
        print_number(test);

    ret 0;
}

: !int <64> multiply(!int <64> a, !int <64> b)
: {
:     ret a * b;
: }

: !int <64> add(!int <64> a, !int <64> b)
: {
:     !int <64> test = 0;
:     test = multiply(2 * 3);

:     ret a + b;
: }

: !int <64> main()
: {

:     !int <64> guess = 7;
:     guess = add(1, 2);

:     : print_number(guess);

:     ret guess;
: }
