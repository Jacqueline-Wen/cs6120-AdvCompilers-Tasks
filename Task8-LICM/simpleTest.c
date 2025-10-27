#include <stdio.h>

void test() {
    int a = 20;
    int b = 40;
    int c;
    int d = 0;
    for (int i = 0; i < 10; i++) {
        c = a * b;      // loop-invariant
        d += c;         // accumulates in loop
    }
    printf("%d\n", d);
}

int main() {
    test();
    return 0;
}
