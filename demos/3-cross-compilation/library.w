glb hello_from_wlang()
{
    !wstr& <64> str[17] = "Hello from wlang\n"; 
    print_wstr(str, 17);
}

glb <64> add(<64> a, <64> b)
{
    ret a + b;
}

