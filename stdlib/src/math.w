: returns the absolute value of a number
:
: this function calculates the absolute value of `x`. if `x` is negative, 
: it returns `-x`; otherwise, it returns `x`.
glb <64> abs(<64> x)
{
    if (x < 0) {
        ret -x;
    } 
    else {
        ret x;
    }
}

: returns the maximum of two numbers
:
: this function compares two numbers `a` and `b`, and returns the larger of the two.
glb <64> max(<64> a, <64> b)
{
    if (a > b) {
        ret a;
    } 
    else {
        ret b;
    }
}

: returns the minimum of two numbers
:
: this function compares two numbers `a` and `b`, and returns the smaller of the two.
glb <64> min(<64> a, <64> b)
{
    if (a < b) {
        ret a;
    } 
    else {
        ret b;
    }
}

: calculates the power of a base raised to an exponent
:
: this function calculates `base` raised to the power of `exponent` using 
: a simple loop. it assumes that `exponent` is non-negative; if `exponent` 
: is negative, the function exits with an error.
glb <64> pow(<64> base, <64> exponent)
{
    if (exponent < 0) {
        !wstr& <64> err_msg[64] = "error: libw/math/pow: negative exponent is not supported\n";
        print_wstr(err_msg, 64);
        exit(1);
    }

    <64> result = 1;
    loop {
        if (exponent == 0) {
            break;
        }

        result = result * base;
        exponent = exponent - 1;
    }

    ret result;
}

: calculates the square root of a number
:
: this function computes the integer square root of `x` using binary search.
: if `x` is negative, the function exits with an error since square roots of 
: negative numbers are not supported. the result is the largest integer `n` 
: such that `n * n <= x`.
glb <64> sqrt(<64> x)
{
    if (x < 0) {
        !wstr& <64> err_msg[64] = "error: libw/math/sqrt: square root of a negative number is not supported\n";
        print_wstr(err_msg, 64);
        exit(1);
    }

    <64> low = 0;
    <64> high = x;
    <64> result = 0;

    loop {
        if (low > high) {
            break;
        }

        <64> mid = (low + high) / 2;
        <64> square = mid * mid;

        if (square == x) {
            ret mid;
        }
        else {
            if (square < x) {
                low = mid + 1;
                result = mid;
            }
            else {
                high = mid - 1;
            }
        }
    }

    ret result;
}

