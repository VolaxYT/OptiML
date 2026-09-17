#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vector.h"

int main(int argc, char** argv){
    vec_t a = { .size = 3, .data = (float[]){1, 2, 3} };
    vec_t b = { .size = 3, .data = (float[]){4, 5, 6} };
    vec_t* c = add(&a, &b);
    assert(c->data[0] == 5 && c->data[1] == 7 && c->data[2] == 9);
    
    vec_free(c);
    return 0;
}