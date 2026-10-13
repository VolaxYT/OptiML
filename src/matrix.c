#include "matrix.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <immintrin.h>
#include <string.h>

#ifndef BLOCK
#define BLOCK 128
#endif

struct mat_t {
    float* data;   // row-major order : data[i*cols + j]
    size_t rows;
    size_t cols;
};

static size_t min_size(size_t a, size_t b){ return a < b ? a : b; }

static mat_t* mat_alloc(size_t rows, size_t cols){
    mat_t* mat = malloc(sizeof(mat_t));
    if(!mat) {
        fprintf(stderr, "Error : malloc failed.\n");
        return NULL;
    }
    mat->rows = rows;
    mat->cols = cols;
    mat->data = alloc_aligned_floats(rows*cols);
    if(!mat->data) {
        fprintf(stderr, "Error : malloc failed.\n");
        free(mat);
        return NULL;
    }
    return mat;
}

mat_t* mat_random(size_t rows, size_t cols){
    mat_t* mat = mat_alloc(rows, cols);
    if(!mat) return NULL;

    size_t n = rows * cols;
    for(size_t i = 0; i < n; i++){
        mat->data[i] = (float)rand() / (float)RAND_MAX;
    }
    return mat;
}

mat_t* mat_create(size_t rows, size_t cols, const float* values){
    mat_t* mat = mat_alloc(rows, cols);
    if(!mat) return NULL;

    memcpy(mat->data, values, rows*cols * sizeof(float));
    return mat;
}

mat_t* mat_zeros(size_t rows, size_t cols){
    mat_t* mat = mat_alloc(rows, cols);
    if(!mat) return NULL;

    memset(mat->data, 0, rows * cols * sizeof(float));
    return mat;
}

mat_t* mat_ones(size_t rows, size_t cols){
    mat_t* mat = mat_alloc(rows, cols);
    if(!mat) return NULL;

    size_t n = rows * cols;
    for(size_t i = 0; i < n; i++) {
        mat->data[i] = 1.0f;
    }
    return mat;
}

mat_t* mat_add(const mat_t* mat1, const mat_t* mat2){
    if(!mat1 || !mat2){
        fprintf(stderr, "Error : mat1 or mat2 is NULL pointer.\n");
        return NULL;
    }

    if((mat1->rows != mat2->rows) || (mat1->cols != mat2->cols) ){
        fprintf(stderr, "Error : mat1 and mat2 not the same size.\n");
        return NULL;
    }

    mat_t* mat = mat_alloc(mat1->rows, mat1->cols);
    if(!mat) return NULL;

    for(size_t i = 0; i < mat1->rows * mat1->cols;i++){
        mat->data[i] = mat1->data[i] + mat2->data[i];
    }

    return mat;
}

mat_t* mat_sub(const mat_t* mat1, const mat_t* mat2){
    if(!mat1 || !mat2){
        fprintf(stderr, "Error : mat1 or mat2 is NULL pointer.\n");
        return NULL;
    }
    
    if((mat1->rows != mat2->rows) || (mat1->cols != mat2->cols) ){
        fprintf(stderr, "Error : mat1 and mat2 not the same size.\n");
        return NULL;
    }

    mat_t* mat = mat_alloc(mat1->rows, mat1->cols);
    if(!mat) return NULL;

    for(size_t i = 0; i < mat1->rows * mat1->cols;i++){
        mat->data[i] = mat1->data[i] - mat2->data[i];
    }


    return mat;
}

mat_t* mat_scale(const mat_t* mat, float scalar){
    if(!mat){
        fprintf(stderr, "Error : mat is NULL pointer.\n");
        return NULL;
    }

    mat_t* result = mat_alloc(mat->rows, mat->cols);
    if(!result) return NULL;

    for(size_t i = 0; i < mat->rows*mat->cols; i++){
        result->data[i] = mat->data[i] * scalar;
    }
    return result;
}

float mat_get(const mat_t* mat, size_t row, size_t col){
     if(!mat){
        fprintf(stderr, "Error : mat is NULL pointer.\n");
        return NAN;
    }

    if (row >= mat->rows || col >= mat->cols){
        fprintf(stderr, "Error: index out of bounds.\n");
        return NAN;
    }
    return mat->data[row*mat->cols + col];
}

void mat_set(mat_t* mat, size_t row, size_t col, float value){
    if(!mat){
        fprintf(stderr, "Error : mat is NULL pointer.\n");
        return;
    }

    if (row >= mat->rows || col >= mat->cols){
        fprintf(stderr, "Error: index out of bounds.\n");
        return;
    }
    mat->data[row*mat->cols + col] = value;
}

size_t mat_rows(const mat_t* mat){
    if(!mat){
        fprintf(stderr, "Error : mat is NULL pointer.\n");
        return 0;
    }

    return mat->rows;
}

size_t mat_cols(const mat_t* mat){
    if(!mat){
        fprintf(stderr, "Error : mat is NULL pointer.\n");
        return 0;
    }

    return mat->cols;
}

mat_t* mat_transpose(const mat_t* mat){
    if(!mat){
        fprintf(stderr, "Error : mat is NULL pointer.\n");
        return NULL;
    }

    mat_t* result = mat_alloc(mat->cols, mat->rows);
    if(!result) return NULL;

    for(size_t i = 0; i < mat->cols; i++){
        for(size_t j=0; j < mat->rows; j++){
            result->data[i*mat->rows + j] = mat->data[j*mat->cols + i];
        }
    }
    return result;
}

mat_t* mat_mul_naive(const mat_t* mat1, const mat_t* mat2){
    if(!mat1 || !mat2){
        fprintf(stderr, "Error : mat1 or mat2 is NULL pointer.\n");
        return NULL;
    }
    
    if((mat1->cols != mat2->rows)){
        fprintf(stderr, "Error : incompatible dimensions for multiplication.\n");
        return NULL;
    }

    mat_t* result = mat_alloc(mat1->rows, mat2->cols);
    if(!result) return NULL;

    for(size_t i = 0; i < mat1->rows; i++){
        for(size_t j = 0; j < mat2->cols; j++){
            float sum = 0.0f;
            for(size_t k = 0; k < mat1->cols; k++){
                sum += mat1->data[i*mat1->cols + k]*mat2->data[k*mat2->cols + j];
            }
            result->data[i*result->cols + j] = sum;
        }
    }
    return result;
}

mat_t* mat_mul_ikj(const mat_t* mat1, const mat_t* mat2){
    if(!mat1 || !mat2){
        fprintf(stderr, "Error : mat1 or mat2 is NULL pointer.\n");
        return NULL;
    }
    
    if((mat1->cols != mat2->rows)){
        fprintf(stderr, "Error : incompatible dimensions for multiplication.\n");
        return NULL;
    }

    const size_t M = mat1->rows, K = mat1->cols, N = mat2->cols;
    mat_t* result = mat_zeros(M, N);
    if(!result) return NULL;

    const float* A = mat1->data;
    const float* B = mat2->data;
    float* C = result->data;

    for(size_t i = 0; i < M; i++){
        for(size_t k = 0; k < K; k++){
            const float a = A[i * K + k];
            const float* b_row = &B[k * N];
            float* c_row = &C[i * N];
            for(size_t j = 0; j < N; j++){
                c_row[j] += a * b_row[j];
            }
        }
    }
    return result;
}

mat_t* mat_mul_blocked(const mat_t* mat1, const mat_t* mat2){
    if(!mat1 || !mat2){
        fprintf(stderr, "Error : mat1 or mat2 is NULL pointer.\n");
        return NULL;
    }
    if(mat1->cols != mat2->rows){
        fprintf(stderr, "Error : incompatible dimensions for multiplication.\n");
        return NULL;
    }

    const size_t M = mat1->rows, K = mat1->cols, N = mat2->cols;

    mat_t* result = mat_zeros(M, N);
    if(!result) return NULL;

    const float* A = mat1->data;
    const float* B = mat2->data;
    float* C = result->data;

    for(size_t ii = 0; ii < M; ii += BLOCK){
        size_t i_end = min_size(ii + BLOCK, M);
        for(size_t kk = 0; kk < K; kk += BLOCK){
            size_t k_end = min_size(kk + BLOCK, K);
            for(size_t jj = 0; jj < N; jj += BLOCK){
                size_t j_end = min_size(jj + BLOCK, N);

                for(size_t i = ii; i < i_end; i++){
                    for(size_t k = kk; k < k_end; k++){
                        float a = A[i * K + k];
                        const float* b_row = &B[k * N];
                        float* c_row = &C[i * N];
                        for(size_t j = jj; j < j_end; j++){
                            c_row[j] += a * b_row[j];
                        }
                    }
                }
            }
        }
    }
    return result;
}

// Micro-kernel : C[i..i+3][j..j+15] += A[i..i+3][kk..k_end) * B[kk..k_end)[j..j+15]
static void kernel_4x16(const float* A, const float* B, float* C, size_t K, size_t N, size_t i, size_t j, size_t kk, size_t k_end){
    float* c0 = &C[(i + 0) * N + j];
    float* c1 = &C[(i + 1) * N + j];
    float* c2 = &C[(i + 2) * N + j];
    float* c3 = &C[(i + 3) * N + j];

    // on charge C car on accumule sur plusieurs blocs kk
    __m256 c00 = _mm256_loadu_ps(c0), c01 = _mm256_loadu_ps(c0 + 8);
    __m256 c10 = _mm256_loadu_ps(c1), c11 = _mm256_loadu_ps(c1 + 8);
    __m256 c20 = _mm256_loadu_ps(c2), c21 = _mm256_loadu_ps(c2 + 8);
    __m256 c30 = _mm256_loadu_ps(c3), c31 = _mm256_loadu_ps(c3 + 8);

    const float* a0 = &A[(i + 0) * K];
    const float* a1 = &A[(i + 1) * K];
    const float* a2 = &A[(i + 2) * K];
    const float* a3 = &A[(i + 3) * K];

    for(size_t k = kk; k < k_end; k++){
        const float* b = &B[k * N + j];
        __m256 b0 = _mm256_loadu_ps(b);
        __m256 b1 = _mm256_loadu_ps(b + 8);

        __m256 a = _mm256_broadcast_ss(&a0[k]);
        c00 = _mm256_fmadd_ps(a, b0, c00);
        c01 = _mm256_fmadd_ps(a, b1, c01);

        a = _mm256_broadcast_ss(&a1[k]);
        c10 = _mm256_fmadd_ps(a, b0, c10);
        c11 = _mm256_fmadd_ps(a, b1, c11);

        a = _mm256_broadcast_ss(&a2[k]);
        c20 = _mm256_fmadd_ps(a, b0, c20);
        c21 = _mm256_fmadd_ps(a, b1, c21);

        a = _mm256_broadcast_ss(&a3[k]);
        c30 = _mm256_fmadd_ps(a, b0, c30);
        c31 = _mm256_fmadd_ps(a, b1, c31);
    }

    // une seule écriture de C
    _mm256_storeu_ps(c0, c00); _mm256_storeu_ps(c0 + 8, c01);
    _mm256_storeu_ps(c1, c10); _mm256_storeu_ps(c1 + 8, c11);
    _mm256_storeu_ps(c2, c20); _mm256_storeu_ps(c2 + 8, c21);
    _mm256_storeu_ps(c3, c30); _mm256_storeu_ps(c3 + 8, c31);
}

// Version scalaire pour les bords : C[i0..i1)[j0..j1) += A[i0..i1)[kk..k_end) * B[kk..k_end)[j0..j1)
static void kernel_edge(const float* A, const float* B, float* C, size_t K, size_t N, size_t i0, size_t i1, size_t j0, size_t j1, size_t kk, size_t k_end){
    for(size_t i = i0; i < i1; i++){
        for(size_t k = kk; k < k_end; k++){
            const float a = A[i * K + k];
            for(size_t j = j0; j < j1; j++){
                C[i * N + j] += a * B[k * N + j];
            }
        }
    }
}

mat_t* mat_mul_simd(const mat_t* mat1, const mat_t* mat2){
    if(!mat1 || !mat2){
        fprintf(stderr, "Error : mat1 or mat2 is NULL pointer.\n");
        return NULL;
    }
    if(mat1->cols != mat2->rows){
        fprintf(stderr, "Error : incompatible dimensions for multiplication.\n");
        return NULL;
    }

    const size_t M = mat1->rows, K = mat1->cols, N = mat2->cols;
    mat_t* result = mat_zeros(M, N);
    if(!result) return NULL;

    const float* A = mat1->data;
    const float* B = mat2->data;
    float* C = result->data;

    for(size_t ii = 0; ii < M; ii += BLOCK){
        const size_t i_end = min_size(ii + BLOCK, M);
        for(size_t kk = 0; kk < K; kk += BLOCK){
            const size_t k_end = min_size(kk + BLOCK, K);
            for(size_t jj = 0; jj < N; jj += BLOCK){
                const size_t j_end = min_size(jj + BLOCK, N);

                size_t i = ii;
                for(; i + 4 <= i_end; i += 4){
                    size_t j = jj;
                    for(; j + 16 <= j_end; j += 16){
                        kernel_4x16(A, B, C, K, N, i, j, kk, k_end);
                    }
                    // colonnes restantes (< 16) pour ces 4 lignes
                    if(j < j_end){
                        kernel_edge(A, B, C, K, N, i, i + 4, j, j_end, kk, k_end);
                    }
                }
                // lignes restantes (< 4) sur toute la largeur du bloc
                if(i < i_end){
                    kernel_edge(A, B, C, K, N, i, i_end, jj, j_end, kk, k_end);
                }
            }
        }
    }
    return result;
}

mat_t* mat_mul(const mat_t* mat1, const mat_t* mat2){
    return mat_mul_simd(mat1, mat2);
}

void mat_free(mat_t* mat){
    if(!mat){
        fprintf(stderr, "Error : mat is NULL pointer.\n");
        return;
    }

    free(mat->data);
    free(mat);
}