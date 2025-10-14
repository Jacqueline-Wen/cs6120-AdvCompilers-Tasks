#include <stdio.h>

int divide_int(int a, int b) {
    return a / b;
}

int main() {
    int x = 10;
    int y = 0;
    int z = 5;

    printf("divide_int(x, y) = %d\n", divide_int(x, y));
    printf("divide_int(x, z) = %d\n", divide_int(x, z));

    return 0;
}