! include print_string() from std

!int <64> main()
{
    : Define the string "Hello"
    !ascii <8> str[7] = [72, 101, 108, 108, 111, 10, 0];

    : Print the string
    %sysc(1, 1, &str, 7, 0, 0, 0);

    ret 0;
}
