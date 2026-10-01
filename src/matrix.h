#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

typedef struct mat_t mat_t; 

mat_t* mat_create(size_t rows, size_t cols, const float* values);
mat_t* mat_zeros(size_t rows, size_t cols);
mat_t* mat_ones(size_t rows, size_t cols);
mat_t* mat_add(const mat_t* mat1, const mat_t* mat2);
mat_t* mat_sub(const mat_t* mat1, const mat_t* mat2);

#endif