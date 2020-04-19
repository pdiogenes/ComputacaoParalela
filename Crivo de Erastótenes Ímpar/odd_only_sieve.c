// original code made by geeks by geeks, modified for the parallel computing course @ PUC-MG

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

int sieveOfEratosthenes(int n){
    /*
        initializing the counter with 1 so it counts the number "2" as a prime
        since this code only verifies odd numbers
    */

    int primes = 1;
    bool *prime = (bool*) malloc((n+1)*sizeof(bool));
    int sqrt_n = sqrt(n);

    memset(prime, true,(n+1)*sizeof(bool));

    /*
        starting p as 3 since its the first odd prime
        p is incremented by 2 as we're skipping even numbers
        starting i as p squared so its certainly an odd number
        i is incremented by 2*p so the next iteration certainly works on an odd number
            which is obviously not a prime
    */

    #pragma omp parallel for schedule(dynamic)
    for(int p=3; p <= sqrt_n; p+=2){
        if(prime[p] == true){
            for(int i=p*p; i<=n; i += 2*p){
                prime[i] = false;
            }
        }
    }

    /*
        counting prime numbers
    */
    
    #pragma omp parallel for reduction(+:primes)
    for(int p=3; p<=n; p+=2){
        if(prime[p]) primes++;
    }

    return(primes);
}

int main(){
    int n = 1000000000;
    printf("%d\n",sieveOfEratosthenes(n));
    return 0;
}
