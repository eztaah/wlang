fun print_num(cst num: I32): Void
{
    # print with printf function
}

fun add(cst a: I32, cst b: I32): I32
{
    cst sum: I32 = a + b;

    return sum;
}

var num1: I32 = (1 + -2) / 3;
num1 = 3 * 4 / 3;

cst num2: I32 = (num1 + 3) * 2 - 4 * (7 * -12);

var num3: I32 = 7;
num3 = num3 + add(num1, num2);

print_num(num3);

1 + 1;

num3;

