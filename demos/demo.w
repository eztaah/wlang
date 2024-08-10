# Convert a digit to its corresponding ASCII code and print it.
fun print_digit(cst digit: I64): Void
{
    # convert the number to its ASCII code
    cst asciicode: I64 = digit + 48;

    # print the ASCII character (using write syscall)
    @syscall(1, 1, &asciicode, 1, 0, 0, 0);

    # print a newline character (using write syscall)
    cst new_line: I64 = 10; 
    @syscall(1, 1, &new_line, 1, 0, 0, 0);

    return;
}

fun add(cst a: I64, cst b: I64): I64
{
    return a + b;
}

_start
{
    # define two cst integers
    cst num1: I64 = 3;
    cst num2: I64 = 5;

    # calculate the sum of num1 and num2, then print the result
    cst sum: I64 = add(num1, num2);
    print_digit(sum);

    # exit the program (using exit syscall)
    @syscall(60, 0, 0, 0, 0, 0, 0);
}
