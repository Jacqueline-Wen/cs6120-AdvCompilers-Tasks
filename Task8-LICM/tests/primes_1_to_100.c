#include <stdio.h>
int main(void){
    for(int n=2;n<=100;n++){
        int prime = 1;
        for(int d=2; d*d<=n; d++){
            if(n%d==0){ prime=0; break; }
        }
        if(prime) printf("%d\n", n);
    }
    return 0;
}