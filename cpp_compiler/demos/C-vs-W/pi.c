#include <stdio.h>
#include <stdint.h>

int main() {
    int64_t terms = 3000000000;
    double pi = 0.0;

    for (int64_t i = 0; i < terms; i++) {
        if (i % 2 == 0) {
            pi += 1.0 / (2 * i + 1);
        } else {
            pi -= 1.0 / (2 * i + 1);
        }
    }
    pi *= 4;

    printf("Approximated value of PI after %d terms is: %f\n", terms, pi);
    return 0;
}
