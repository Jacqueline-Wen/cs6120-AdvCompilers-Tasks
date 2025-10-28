#include <stdio.h>
#include <stdlib.h>


// Usage: ./fibonacci <terms>
// Prints the first <terms> Fibonacci numbers
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <terms>\n", argv[0]);
        return 1;
    }
    long n = strtol(argv[1], NULL, 10);
    long long a = 0, b = 1;
    for (long i = 0; i < n; i++) {
        printf("%lld\n", a);
        long long next = a + b;
        a = b; b = next;
    }
    return 0;
}