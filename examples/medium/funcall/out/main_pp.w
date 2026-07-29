<64> add(<64> a, <64> b)
{
    ret a + b;
}

<64> main()
{
    <64> result = add(2, 3);
    print_number(result);

    ret 0;
}

