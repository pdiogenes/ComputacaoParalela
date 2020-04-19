/*
Pedro Diógenes
Alefe Lucas
Bernardo Morais

Foi alterado a entrada para N = 90000
para ter um tempo mais amplo e poder 
medir melhor o speedup.


Tempo sequencial:

real	0m21,744s
user	0m21,740s
sys	0m0,004s

Tempo paralelo com politica default:

real	0m12,236s
user	0m22,645s
sys	0m0,000s

Tempo paralelo com política dynamic com chunks de tamanho 300 (raiz quadrada de N):

real	0m11,271s
user	0m22,534s
sys	0m0,004s

Speedup: 21,744 / 11,271 = 1.92


*/


#include <stdio.h>
#include <stdlib.h>

int main() 
{
	int i, j, n = 90000; 

	// Allocate input, output and position arrays
	int *in = (int*) calloc(n, sizeof(int));
	int *pos = (int*) calloc(n, sizeof(int));   
	int *out = (int*) calloc(n, sizeof(int));   

	// Initialize input array in the reverse order
	for(i=0; i < n; i++)
		in[i] = n-i;  

	// Print input array
	//   for(i=0; i < n; i++) 
	//      printf("%d ",in[i]);

	// Silly sort (you have to make this code parallel) 
	#pragma omp parallel for private(i, j) schedule(dynamic, 300) num_threads(2)
	for(i=0; i < n; i++) 
		for(j=0; j < n; j++)
			if(in[i] > in[j]) pos[i]++;	

	// Move elements to final position
	for(i=0; i < n; i++) 
		out[pos[i]] = in[i];

	// print output array
	//   for(i=0; i < n; i++) 
	//      printf("%d ",out[i]);

	// Check if answer is correct
	for(i=0; i < n; i++)
		if(i+1 != out[i]) 
		{
			printf("test failed\n");
			exit(0);
		}

	printf("test passed\n"); 
}  
