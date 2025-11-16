#include <stdio.h>
#include <stdlib.h>


// Usage: ./factorial <n>
// Computes n! in 64-bit (valid up to 20!)
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }
    long n = strtol(argv[1], NULL, 10);
    if (n < 0 || n > 20) {
        fprintf(stderr, "n must be between 0 and 20 for 64-bit factorial.\n");
        return 1;
    }
    unsigned long long f = 1ULL;
    for (long i = 2; i <= n; i++) f *= (unsigned long long)i;
    printf("%ld! = %llu\n", n, f);
    return 0;
}