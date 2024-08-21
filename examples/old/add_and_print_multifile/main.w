! def int <64> add(int <64> a, int <64> b)       from ./add.w
! def void print_digit(digit <64> num)  from ./io.w

#def HELLO 10

glb !int <64> main()
{
    :define two !cst integers
    !int <64> num1;
    num1 = HELLO;
    !int <64> num2 = 2;

    :calculate the sum of num1 and num2, then print the result
    !int <64> sum = add(num1, num2);
    print_digit(sum);

    ret 0;
}
