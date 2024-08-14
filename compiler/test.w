!fun digit_to_ascii(!digit digit) !ascii
{   
    ret digit + 48;
}

:Convert a digit to its corresponding ASCII code and print it.
!prc print_digit(!digit digit)
{
    :convert the number to its ASCII code
    !ascii asciicode = digit_to_ascii(digit);

    :print the ASCII character (using write syscall)
    @sysc(1, 1, &asciicode, 1, 0, 0, 0);

    :print a newline character (using write syscall)
    !ascii new_line = 10; 
    @sysc(1, 1, &new_line, 1, 0, 0, 0);

    ret; 
}

!fun add(!i64 a, !i64 b) !i64
{
    ret a + b;
}

!fun main()
{
    :define two !cst integers
    !i64 num1 = 3;
    !i64 num2 = 4;

    :calculate the sum of num1 and num2, then print the result
    !i64 sum = add(num1, num2);
    print_digit(sum);

    ret 0;
}
