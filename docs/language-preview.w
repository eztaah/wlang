
# convert a number to its ascii code and print it
#  ! works only for number between 0 and 9
fun print_num(cst num: I64): Void
{
    # convert number to his corresponding ascii code
    cst asciicode: I64 = num + 48;

    # print the number with a write syscall
    @syscall(1, 1, &ascii_code, 1, 0, 0, 0);

    return;
}

fun add(cst a: I64, cst b: I64): I64
{
    return a + b;
}

_start
{
    # create 2 variables
    cst num1: I64 = 3;
    cst num2: I64 = 5;

    # get the sum of these 2 numbers and print it
    cst sum: I64 = add(num1, num2);
    print_num(sum);

    # exit the program with an exit syscall
    @syscall(60, error_code, 0, 0, 0, 0, 0);
}


