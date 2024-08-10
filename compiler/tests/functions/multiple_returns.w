!int <64> max(!int <64> a, !int <64> b) {
    if (a > b) {
        ret a;
    } else {
        ret b;
    }
}

!int <64> main() {
    ret max(7, 3); 
}

: expected exit code : 7