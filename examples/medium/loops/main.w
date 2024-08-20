!int <64> main()
{
    !int <64> i = 0;

    loop {
        print_number(i);
        i = i + 1;

        if (i == 5) {
            break;
        }
    }

    ret 0;
}
