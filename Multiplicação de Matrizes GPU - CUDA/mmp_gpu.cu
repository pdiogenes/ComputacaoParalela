/*
Tempo sequencial:
    real    1m6.463s
    user    1m6.338s
    sys     0m0.068s

Tempo paralelo CPU:
    real    0m36.188s
    user    2m21.357s
    sys     0m3.156s

Tempo paralelo GPU:
    real    0m21.687s
    user    0m17.254s
    sys     0m4.460s

Tempo paralelo GPU CUDA:
    real    0m1.498s
    user    0m0.515s
    sys     0m0.890s



--

Métricas do nvprof:


    distribute parallel for simd:

    Invocations                                Event Name         Min Max Avg
Total Device "GeForce GT 1030 (0)" Kernel: mm$_omp_fn$0 1 warps_launched 72 72
72          72

    ==28418== Metric result:
    Invocations                               Metric Name Metric Description Min
Max         Avg Device "GeForce GT 1030 (0)" Kernel: mm$_omp_fn$0 1
warp_execution_efficiency                 Warp Execution Efficiency      90.31%
90.31%      90.31%


    CUDA gpu:
    Invocations                                Event Name         Min         Max         Avg       Total
    Device "GeForce GT 1030 (0)"
        Kernel: mm(double*, double*, double*, int)
            1                            warps_launched      127008      127008      127008      127008

    ==26255== Metric result:
    Invocations                               Metric Name                        Metric Description         Min         Max         Avg
    Device "GeForce GT 1030 (0)"
        Kernel: mm(double*, double*, double*, int)
            1                 warp_execution_efficiency                 Warp Execution Efficiency      99.21%      99.21%      99.21%




*/

#include <stdio.h>
#include <stdlib.h>

__global__ void mm(double* a, double* b, double* c, int width) {
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;

    if(i < width && j < width){
        double sum = 0;
        for (int k = 0; k < width; k++) {
            double x = a[i * width + k];
            double y = b[k * width + j];
            sum += x * y;
        }
        c[i * width + j] = sum;
    }
}

int main() {
    int width = 2000;
    int size = width * width * sizeof(double);
    double* a = (double*)malloc(size);
    double* b = (double*)malloc(size);
    double* c = (double*)malloc(size);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            a[i * width + j] = i;
            b[i * width + j] = j;
            c[i * width + j] = 0;
        }
    }

    double *d_a, *d_b, *d_c;

    cudaMalloc((void **) &d_a, size);
    cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);

    cudaMalloc((void **) &d_b, size);
    cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);

    cudaMalloc((void **) &d_c, size);

    int block_size = 32;
    int blocks = (width-1)/block_size + 1;
    dim3 dimGrid(blocks, blocks, 1);
    dim3 dimBlock(block_size, block_size, 1);

    mm<<<dimGrid,dimBlock>>>(d_a, d_b, d_c, width);

    cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);

    
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            printf("\n c[%d][%d] = %f", i, j, c[i * width + j]);
        }
    }
    
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
}