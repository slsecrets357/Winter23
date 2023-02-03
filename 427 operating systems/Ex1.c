#include <stdio.h>

int add(int x, int y);

int main(void) {
    int a = 40;
    int b = 1283;

    int c = add(a, b);
    printf("%d\n", c);

    return 0;
}

int add(int x, int y) {
    return x + y;
}
