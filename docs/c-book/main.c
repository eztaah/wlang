// original.c
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159
#define SQUARE(x) ((x) * (x))

int main() {
    #ifdef PI
    printf("The value of PI is: %f\n", PI);
    #endif

    int num = 5;
    printf("The square of %d is: %d\n", num, SQUARE(num));

    return 0;
}
