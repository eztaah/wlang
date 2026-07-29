!int <64> factorial(!int <64> n) {
    if (n == 0) {
        ret 1;
    } else {
        ret n * factorial(n - 1);
    }
}

!int <64> main() {
    !int <64> result = factorial(5);
    ret result; 
}

: expected exit code : 120