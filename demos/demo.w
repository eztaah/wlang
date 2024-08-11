!fun digit_to_ascii(!cst !digit digit) !ascii
{
    ret digit + 48;
}

# Convert a digit to its corresponding ASCII code and print it.
!prc print_digit(!cst !digit digit)
{
    # convert the number to its ASCII code
    asciicode = digit_to_ascii(digit);

    # print the ASCII character (using write syscall)
    @sysc(1, 1, &asciicode, 1, 0, 0, 0);

    # print a newline character (using write syscall)
    new_line = 10; 
    @sysc(1, 1, &new_line, 1, 0, 0, 0);
}

:fun add(:cst !i64 a, :cst :i64 b) !i64
{
    ret a + b;
}

main()
{
    !cst !i64 num1;
    
    # define two !cst integers
    num1 = 3;
    !cst !i64 num2 = 5;

    # calculate the sum of num1 and num2, then print the result
    !cst !i64 sum = add(num1, num2);
    print_digit(sum);

    ret 0;
}
