!int <64> main()
{
    !ascii <64> str2[13] = "Hello World!\n"; 

    : Print the string
    print_str_array(str2, 13);

    ret 0;
}

: expected output : Hello World!\n