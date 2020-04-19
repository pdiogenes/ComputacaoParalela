/*
Pedro Diógenes de Almeida Lara
605549

Adicionei um 0 a entrada para poder
medir de forma mais precisa o tempo
de execução, visto que o tempo com a
entrada original estava dando apenas
4 segundos.

A nova saída é: 50847534

Tempo sequencial:
50847534
real    0m50.676s
user    0m49.650s
sys     0m0.934s


Tempo paralelo:
50847534
real    0m28.803s
user    1m51.195s
sys     0m0.718s

Tempo paralelo com GPU:
50847534

real    0m13.928s
user    0m10.699s
sys     0m3.187s

Speedup em relação a versão sequencial:
50.676 / 13.928 = 3,63

Speedup em relação a versão paralela em CPU:
28.803 / 13.928 = 2,07

//

*/

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sieveOfEratosthenes(int n) {
    int primes = 0;
    bool *prime = (bool *)malloc((n + 1) * sizeof(bool));
    int sqrt_n = sqrt(n);

    memset(prime, true, (n + 1) * sizeof(bool));

    #pragma omp target map(tofrom : prime [0:n + 1])
    #pragma omp teams distribute
    for (int p = 2; p <= sqrt_n; p++) {
        if (prime[p] == true) {
            #pragma omp parallel for simd
            for (int i = p * 2; i <= n; i += p) prime[i] = false;
        }
    }

    #pragma omp target map(tofrom : primes) map(to : prime [0:n + 1])
    #pragma omp teams distribute parallel for simd reduction(+ : primes)
    for (int p = 2; p <= n; p++)
        if (prime[p]) primes++;

    return (primes);
}

int main() {
    int n = 1000000000;
    printf("%d\n", sieveOfEratosthenes(n));
    return 0;
}
