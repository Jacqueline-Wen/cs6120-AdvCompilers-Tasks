#include <stdio.h>
#include <stdlib.h>


// Usage: echo "1 2 3 4 5" | ./array_reverse 5
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <count>\n", argv[0]);
        return 1;
    }
    int n = (int)strtol(argv[1], NULL, 10);
    long a[10000];
    if (n < 0 || n > 10000) return 1;
    for (int i = 0; i < n; i++) {
        scanf("%ld", &a[i]);
    }
    for (int i = 0; i < n / 2; i++) {
        long tmp = a[i];
        a[i] = a[n - 1 - i];
        a[n - 1 - i] = tmp;
    }
    for (int i = 0; i < n; i++) {
        if (i) putchar(' ');
        printf("%ld", a[i]);
    }
    putchar('\n');
    return 0;
}