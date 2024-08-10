#include <stdio.h>

long w__add(long a, long b);
void w__hello_from_wlang();

int main() 
{
    w__hello_from_wlang();

    long a = 5;
    long b = 7;
    long result = w__add(a, b);

    printf("Result of %ld + %ld = %ld\n", a, b, result);

    return 0;
}
