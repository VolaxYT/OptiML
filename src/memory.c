#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <string.h>


float* alloc_aligned_floats(size_t n) {
    float* ptr;
    if (posix_memalign((void**)&ptr, 32, n * sizeof(float)) != 0) {
        return NULL;
    }
    return ptr;
}

// TODO : arena allocator
// TODO : compteur/stats debug malloc, fuites, patterns d'allocation