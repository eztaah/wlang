!int <64> main() {
    ret (4 || 0) + (0 || 3) + (5 || 5);
}

: expected exit code : 3