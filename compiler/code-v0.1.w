fun i32_to_char(cst num: I64): I64
{
    var ascii_code: I64 = num + 48;

    return ascii_code;
}

fun print_num(cst num: I64): Void
{
    var ascii_code: I64 = i32_to_char(num);

    @sysc_write(1, &ascii_code, 1);  # 1 correspond à stdout
}

fun add(cst a: I64, cst b: I64): I64
{
    cst sum: I64 = a + b;

    return sum;
}

fun _start(Void): I64
{
    var num1: I64 = (1 + -2) / 3;
    num1 = 3 * 4 / 3;

    cst num2: I64 = (num1 + 3) * 2 - 4 * (7 * -12);

    var num3: I64 = 7;
    num3 = num3 + add(num1, num2);

    print_num(num3);

    1 + 1;

    num3; 

    return 0;
}
