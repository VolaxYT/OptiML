#include "matrix.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <immintrin.h>
#include <string.h>

struct mat_t {
    float* data;   // row-major order : data[i*cols + j]
    size_t rows;
    size_t cols;
};

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