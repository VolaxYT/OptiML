#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <immintrin.h>
#include <string.h>

static float* alloc_aligned_floats(size_t n) {
    float* ptr;
    if (posix_memalign((void**)&ptr, 32, n * sizeof(float)) != 0) {
        return NULL;
    }
    return ptr;
}

static vec_t* vec_alloc(size_t size) {
    vec_t* vec = malloc(sizeof(vec_t));
    if (!vec) {
        fprintf(stderr, "Error : malloc failed.\n");
        return NULL;
    }
    vec->size = size;
    vec->data = alloc_aligned_floats(size);
    if (!vec->data) {
        fprintf(stderr, "Error : malloc failed.\n");
        free(vec);
        return NULL;
    }
    return vec;
}

vec_t* vec_random(size_t size) {
    vec_t* vec = vec_alloc(size);
    for (size_t i = 0; i < size; i++) {
        vec->data[i] = (float)rand() / RAND_MAX;
    }
    return vec;
}

vec_t* vec_create(const size_t size, const float* values){
    vec_t* vec = vec_alloc(size);
    if (!vec) return NULL;

    memcpy(vec->data, values, size * sizeof(float));
    return vec;
}

vec_t* vec_zeros(const size_t size){
    vec_t* vec = vec_alloc(size);
    if (!vec) return NULL;

    memset(vec->data, 0, size * sizeof(float));
    return vec;
}

vec_t* vec_ones(const size_t size){
    vec_t* vec = vec_alloc(size);
    if (!vec) return NULL;

    memset(vec->data, 1, size * sizeof(float));
    return vec;
}

vec_t* vec_add(const vec_t* vec1, const vec_t* vec2){
    if(vec1-> size != vec2->size){
        fprintf(stderr, "Error : vec1 and vec2 not the same size.\n");
        return NULL;
    }

    vec_t* vec = vec_alloc(vec1->size);
    if(!vec) return NULL;

    for(size_t i = 0; i < vec->size; i++){
        vec->data[i] = vec1->data[i] + vec2->data[i];
    }
    return vec;
}

vec_t* vec_sub(const vec_t* vec1, const vec_t* vec2){
    if(vec1-> size != vec2->size){
        fprintf(stderr, "Error : vec1 and vec2 not the same size.\n");
        return NULL;
    }

    vec_t* vec = vec_alloc(vec1->size);
    if(!vec) return NULL;

    for(size_t i = 0; i < vec->size; i++){
        vec->data[i] = vec1->data[i] - vec2->data[i];
    }
    return vec;
}

vec_t* vec_scale(const vec_t* vec, float scalar){
    vec_t* result = vec_alloc(vec->size);
    if(!vec) return NULL;

    for(size_t i = 0; i < vec->size; i++){
        result->data[i] = vec->data[i] * scalar;
    }
    return result;
}

float vec_dot(const vec_t* vec1, const vec_t* vec2){
    __m256 acc = _mm256_setzero_ps(); // 8 acc, tous à 0
    size_t i = 0;

    for(; i + 8 <= vec1->size;i += 8) {
        __m256 v1 = _mm256_loadu_ps(&vec1->data[i]);
        __m256 v2 = _mm256_loadu_ps(&vec2->data[i]);
        acc = _mm256_fmadd_ps(v1, v2, acc); // acc += va * vb
    }

    // optimisation ici
    float tmp[8];
    _mm256_storeu_ps(tmp, acc);
    float result = tmp[0]+tmp[1]+tmp[2]+tmp[3]+tmp[4]+tmp[5]+tmp[6]+tmp[7];

    // éléments restants (a->size % 8)
    for (; i < vec1->size; i++) {
        result += vec2->data[i] * vec2->data[i];
    }

    return result;
}

float vec_norm_l1(const vec_t* vec){
    __m256 acc = _mm256_setzero_ps();
    __m256 abs_mask = _mm256_set1_ps(-0.0f);
    size_t i = 0;

    for (; i + 8 <= vec->size; i += 8) {
        __m256 v = _mm256_loadu_ps(&vec->data[i]);
        __m256 abs_v = _mm256_andnot_ps(abs_mask, v); // (NOT 10000000[..]000) AND v 
        acc = _mm256_add_ps(acc, abs_v);
    }

    float tmp[8];
    _mm256_storeu_ps(tmp, acc);
    float result = tmp[0]+tmp[1]+tmp[2]+tmp[3]+tmp[4]+tmp[5]+tmp[6]+tmp[7];

    for (; i < vec->size; i++) {
        result += fabsf(vec->data[i]);
    }

    return result;
}

float vec_norm_l2(const vec_t* vec){
    return sqrtf(vec_dot(vec,vec));
}

void vec_free(vec_t* vec) {
    if (vec) {
        free(vec->data);
        free(vec);
    }
}