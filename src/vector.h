#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct{
    size_t size;
    float* data;
} vec_t;

vec_t* vec_create(const size_t size, const float* values);
vec_t* vec_zeros(const size_t size);
vec_t* vec_add(const vec_t* vec1, const vec_t* vec2);
vec_t* vec_sub(const vec_t* vec1, const vec_t* vec2);
vec_t* vec_scale(const vec_t* vec, float scalar);
float vec_dot(const vec_t* vec1, const vec_t* vec2);
float vec_norm_l1(const vec_t* vec);
float vec_norm_l2(const vec_t* vec);
void vec_free(vec_t* vec);

#endif