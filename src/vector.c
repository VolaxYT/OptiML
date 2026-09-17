#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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


vec_t* add(const vec_t* vec1, const vec_t* vec2){
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

vec_t* sub(const vec_t* vec1, const vec_t* vec2){
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

vec_t* scale(const vec_t* vec, float scalar){
    vec_t* result = vec_alloc(vec->size);
    if(!vec) return NULL;

    for(size_t i = 0; i < vec->size; i++){
        result->data[i] = vec->data[i] * scalar;
    }
    return result;
}

float dot(const vec_t* vec1, const vec_t* vec2){
    if(vec1-> size != vec2->size){
        fprintf(stderr, "Error : vec1 and vec2 not the same size.\n");
        return NAN;
    }

    float result = 0;
    for(size_t i = 0; i < vec1->size;i++){
        result += vec1->data[i] * vec2->data[i];
    }
    return result;
}

float norm_l1(const vec_t* vec){
    float sum = 0;
    for(size_t i = 0; i < vec->size;i++){
        sum += fabsf(vec->data[i]);
    }
    return sum;
}

float norm_l2(const vec_t* vec){
    return sqrtf(dot(vec,vec));
}

void vec_free(vec_t* vec) {
    if (vec) {
        free(vec->data);
        free(vec);
    }
}