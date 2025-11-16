#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv){
    if(argc!=2){
        fprintf(stderr, "usage: %s N\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    long long sum=0;
    for(int i=1;i<=n;i++) sum += i;
    printf("sum(1..%d) = %lld\n", n, sum);
    return 0;
}