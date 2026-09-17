#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct{
    size_t size;
    float* data;
} vec_t;

vec_t* add(const vec_t* vec1, const vec_t* vec2);
vec_t* sub(const vec_t* vec1, const vec_t* vec2);
vec_t* scale(const vec_t* vec, float scalar);
float dot(const vec_t* vec1, const vec_t* vec2);
float norm_l1(const vec_t* vec);
float norm_l2(const vec_t* vec);
void vec_free(vec_t* vec);

#endif