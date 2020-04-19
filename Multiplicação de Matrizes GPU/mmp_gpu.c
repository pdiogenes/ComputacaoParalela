/*
Tempo sequencial:
    real    1m6.463s
    user    1m6.338s
    sys     0m0.068s

Tempo paralelo CPU:
    real    0m36.188s
    user    2m21.357s
    sys     0m3.156s

Tempos paralelos GPU:
    apenas distribute:
    real    3m34.373s
    user    2m50.083s
    sys     0m44.316s

    distribute parallel for:
    real    0m49.469s
    user    0m39.581s
    sys     0m9.893s

    distribute parallel for simd:
    real    0m21.687s
    user    0m17.254s
    sys     0m4.460s

--

Métricas do nvprof:

    apenas distribute:

    Invocations                                Event Name         Min         Max         Avg       Total
    Device "GeForce GT 1030 (0)"
        Kernel: mm$_omp_fn$0
            1                            warps_launched          72          72          72          72

    ==28582== Metric result:
    Invocations                               Metric Name                        Metric Description         Min         Max         Avg
    Device "GeForce GT 1030 (0)"
        Kernel: mm$_omp_fn$0
            1                 warp_execution_efficiency                 Warp Execution Efficiency     100.00%     100.00%     100.00%


    
    distribute parallel for:

    Invocations                                Event Name         Min         Max         Avg       Total
    Device "GeForce GT 1030 (0)"
        Kernel: mm$_omp_fn$0
            1                            warps_launched          72          72          72          72

    ==28672== Metric result:
    Invocations                               Metric Name                        Metric Description         Min         Max         Avg
    Device "GeForce GT 1030 (0)"
        Kernel: mm$_omp_fn$0
            1                 warp_execution_efficiency                 Warp Execution Efficiency      98.41%      98.41%      98.41%


    distribute parallel for simd:

    Invocations                                Event Name         Min         Max         Avg       Total
    Device "GeForce GT 1030 (0)"
        Kernel: mm$_omp_fn$0
            1                            warps_launched          72          72          72          72

    ==28418== Metric result:
    Invocations                               Metric Name                        Metric Description         Min         Max         Avg
    Device "GeForce GT 1030 (0)"
        Kernel: mm$_omp_fn$0
            1                 warp_execution_efficiency                 Warp Execution Efficiency      90.31%      90.31%      90.31%



*/

#include <stdio.h>
#include <stdlib.h>

void mm(double* a, double* b, double* c, int width) {
    int array_size = width * width;
    double sum = 0;
    #pragma omp target map (tofrom:c[0:array_size], sum) map (to:a[0:array_size], b[0:array_size])
    #pragma omp teams distribute
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            sum = 0;
            #pragma omp parallel for simd reduction(+:sum)
            // #pragma omp parallel for reduction(+:sum) <--- diretiva paralela em CPU
            for (int k = 0; k < width; k++) {
                double x = a[i * width + k];
                double y = b[k * width + j];
                sum += x * y;
            }
            c[i * width + j] = sum;
        }
    }
}

int main() {
    int width = 2000;
    double* a = (double*)malloc(width * width * sizeof(double));
    double* b = (double*)malloc(width * width * sizeof(double));
    double* c = (double*)malloc(width * width * sizeof(double));

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            a[i * width + j] = i;
            b[i * width + j] = j;
            c[i * width + j] = 0;
        }
    }

    mm(a, b, c, width);

    /*
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            printf("\n c[%d][%d] = %f", i, j, c[i * width + j]);
        }
    }
    */
    
    
}