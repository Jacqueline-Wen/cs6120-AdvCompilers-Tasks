#include <stdio.h>

void test() {
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        int a = 2 + 3;        // constant addition
        int b = a * 4;        // multiply by constant
        int c = b - 7;        // subtract constant
        sum += c;             // will NOT move: accesses memory
    }
    printf("%d\n", sum);
}

int main() {
    test();
    return 0;
}
