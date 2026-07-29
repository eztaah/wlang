!int <64> fibonacci(!int <64> n) {
    if (n <= 1) {
        ret n;
    } else {
        ret fibonacci(n - 1) + fibonacci(n - 2);
    }
}

!int <64> main() {
    ret fibonacci(10);
}

: expected exit code : 55