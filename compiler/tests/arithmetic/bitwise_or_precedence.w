!int <64> main() {
    : | has lower precedence than !=
    ret 5 | 7 != 5;
}

: expected exit code : 5