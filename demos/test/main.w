glb !void print_digit(!digit num)
{
    :convert the number to its ASCII code
    !ascii asciicode = num + 48;

    :print the ASCII character (using write syscall)
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);

    :print a newline character (using write syscall)
    !ascii new_line = 10; 
    %sysc(1, 1, &new_line, 1, 0, 0, 0);
}

!int main()
{
    !int a = 3;
    print_digit(a); 

    !int& ptr = &a;
    !int b = 2 + ^ptr;  :b = 2 + 3 = 5
    print_digit(b);

    ^ptr = 4;    :a = 4
    print_digit(a);

    ret 0;
}
