! def int  add(int a, int b)       from ./add.w
! def void print_digit(digit num)  from ./io.w

glb !int main()
{
    :define two !cst integers
    !int num1 = 3;
    !int num2 = 2;

    :calculate the sum of num1 and num2, then print the result
    !int sum = add(num1, num2);
    print_digit(sum);

    ret 0;
}
