#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>


int main() {
    printf("Calculating GCD, please wait...\n");

    int64_t a = 4342341223;
    int64_t b = 5342342234;
    int64_t result = 1;
    int64_t min = a < b ? a : b;

    for (int64_t i = 2; i <= min; i++) {
        if (a % i == 0 && b % i == 0) {
            result = i;
        }
    }

    printf("GCD of %" PRId64 " and %" PRId64 " is: %" PRId64 "\n", a, b, result);
    return 0;
}
