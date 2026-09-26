#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../vector.h"
#define N_RUNS 7

static double now_seconds(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double) ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

static int compare_doubles(const void* a, const void* b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    return (da > db) - (da < db);
}

static double median(double* values, int n) {
    qsort(values, n, sizeof(double), compare_doubles);
    return values[n / 2];
}

static void bench_dot(size_t n, int iterations) {
    vec_t* a = vec_random(n);
    vec_t* b = vec_random(n);

    volatile float sink = vec_dot(a, b); 

    double times[N_RUNS];
    for (int run = 0; run < N_RUNS; run++) {
        double start = now_seconds();
        for (int i = 0; i < iterations; i++) {
            sink = vec_dot(a, b);
        }
        times[run] = (now_seconds() - start) / iterations;
    }

    double med = median(times, N_RUNS);
    double flops = 2.0 * (double)n;
    double gflops = flops / med / 1e9;

    printf("vec_dot(n=%-9zu) : %10.6f ms/iter | %6.2f GFLOPS\n", n, med * 1000.0, gflops);

    (void)sink;
    vec_free(a);
    vec_free(b);
}

int main(void) {
    size_t sizes[] = {64, 256, 1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216, 67108864};
    int n_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < n_sizes; i++) {
        int iterations = 100000000 / (int)sizes[i];
        if (iterations < 10) iterations = 10;

        bench_dot(sizes[i], iterations);
    }

    return 0;
}