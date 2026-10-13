#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

typedef struct mat_t mat_t; 

mat_t* mat_random(size_t rows, size_t cols);
mat_t* mat_create(size_t rows, size_t cols, const float* values);
mat_t* mat_zeros(size_t rows, size_t cols);
mat_t* mat_ones(size_t rows, size_t cols);
mat_t* mat_add(const mat_t* mat1, const mat_t* mat2);
mat_t* mat_sub(const mat_t* mat1, const mat_t* mat2);
mat_t* mat_scale(const mat_t* mat, float scalar);
float mat_get(const mat_t* mat, size_t row, size_t col);
void mat_set(mat_t* mat, size_t row, size_t col, float value);
size_t mat_rows(const mat_t* mat);
size_t mat_cols(const mat_t* mat);
mat_t* mat_transpose(const mat_t* mat);
mat_t* mat_mul_naive(const mat_t* mat1, const mat_t* mat2);
mat_t* mat_mul_ikj(const mat_t* mat1, const mat_t* mat2);
mat_t* mat_mul_blocked(const mat_t* mat1, const mat_t* mat2);
mat_t* mat_mul_simd(const mat_t* mat1, const mat_t* mat2);
mat_t* mat_mul(const mat_t* mat1, const mat_t* mat2);
void mat_free(mat_t* mat);

#endif