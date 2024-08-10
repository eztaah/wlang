# ------------------------ UTILS LIBRARY ------------------------
fun i32_to_asciicode(cst num: I64): I64
{
    cst ascii_code: I64 = num + 48;

    return ascii_code;
}

# ------------------------ IO library ------------------------
# follow this order when passing arguments to syscall
# @syscall(rax, rdi, rsi, rdx, r10, r8, r9)
fun exit(cst error_code: I64): Void
{
    @syscall(60, error_code, 0, 0, 0, 0, 0);

    # we don't have to add return because we exit the program
}

fun write(cst fd: I64, cst buf_addr: I64, cst count: I64): Void
{
    @syscall(1, fd, buf_addr, count, 0, 0, 0);

    return;
}

# works only for number between 0 and 9
fun print_num(cst num: I64): Void
{
    # print the number
    cst asciicode: I64 = i32_to_asciicode(num);
    write(1, &asciicode, 1);

    # print \n
    cst new_line_ascii_code: I64 = 10; 
    write(1, &new_line_ascii_code, 1);

    return;
}

# ------------------------ USER CODE ------------------------
fun test(): I64
{
    cst sum: I64 = 1;
    return sum;
}

_start
{
    cst res: I64 = test();

    cst sum: I64 = 7;

    print_num(sum);

    exit(0);
}