#include <stdio.h>
#include <string.h>

int main(void) {
    char x[] = { 'x', 'y' };

    printf("1) %ld\n%s\n", sizeof(x), x);

    char *y = "Hello";
    printf("2) %ld\n", sizeof(*y));

    char z[128] = "Hello";
    printf("3) %ld\n", sizeof(z));

    char *zp = z;
    printf("4) %ld\n", sizeof(zp));

    strcat(z, zp);

    printf("5) %s\n", z);
}
