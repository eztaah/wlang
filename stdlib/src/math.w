glb !int <64> abs(!int <64> x)
{
    if (x < 0) {
        ret -x;
    } 
    else {
        ret x;
    }
}

glb !int <64> max(!int <64> a, !int <64> b)
{
    if (a > b) {
        ret a;
    } 
    else {
        ret b;
    }
}

glb !int <64> min(!int <64> a, !int <64> b)
{
    if (a < b) {
        ret a;
    } 
    else {
        ret b;
    }
}

glb !int <64> pow(!int <64> base, !int <64> exponent)
{
    if (exponent < 0) {
        @exit(-1);  : negative exponents are not supported
    }

    !int <64> result = 1;
    loop {
        if (exponent == 0) {
            break;
        }

        result = result * base;
        exponent = exponent - 1;
    }

    ret result;
}

glb !int <64> sqrt(!int <64> x)
{
    if (x < 0) {
        @exit(-1);  : square root of a negative number is not supported
    }

    !int <64> low = 0;
    !int <64> high = x;
    !int <64> result = 0;

    loop {
        if (low > high) {
            break;
        }

        !int <64> mid = (low + high) / 2;
        !int <64> square = mid * mid;

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
