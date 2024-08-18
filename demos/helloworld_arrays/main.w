!int <64> main()
{
    : Define the string "Hello World"
    !ascii <64> str[15] = ['H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\n', '!', ':', '\0'];

    !ascii <64> str2[15] = "Hello World:!\n\0"; 

    : Print the string
    print_array(str2, 15);

    ret 0;
}