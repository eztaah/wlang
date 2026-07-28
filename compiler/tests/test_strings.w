!int <64> main()
{
    !ascii <64> str[13] = "Hello World!\n";
    %sysc(1, 1, str, 13, 0, 0, 0);
    ret 0;
}
