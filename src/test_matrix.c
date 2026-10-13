#include "matrix.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

#define EPS 1e-5f

// Vérifie dimensions + toutes les valeurs (row-major)
static void check_mat(const mat_t* m, size_t rows, size_t cols, const float* expected){
    assert(m != NULL);
    assert(mat_rows(m) == rows);
    assert(mat_cols(m) == cols);
    for(size_t i = 0; i < rows; i++){
        for(size_t j = 0; j < cols; j++){
            assert(fabsf(mat_get(m, i, j) - expected[i*cols + j]) < EPS);
        }
    }
}

static void test_create_get(void){
    float v[] = {1,2,3,4,5,6};
    mat_t* a = mat_create(2, 3, v);
    check_mat(a, 2, 3, v);
    assert(mat_get(a, 1, 2) == 6.0f);
    mat_free(a);
    printf("test_create_get OK\n");
}

static void test_zeros_ones(void){
    float z[6] = {0}, o[6] = {1,1,1,1,1,1};
    mat_t* a = mat_zeros(2, 3);
    mat_t* b = mat_ones(2, 3);
    check_mat(a, 2, 3, z);
    check_mat(b, 2, 3, o);
    mat_free(a); mat_free(b);
    printf("test_zeros_ones OK\n");
}

static void test_add_sub(void){
    float va[] = {1,2,3,4,5,6}, vb[] = {6,5,4,3,2,1};
    float sum[] = {7,7,7,7,7,7}, diff[] = {-5,-3,-1,1,3,5};
    mat_t* a = mat_create(2, 3, va);
    mat_t* b = mat_create(2, 3, vb);
    mat_t* s = mat_add(a, b);
    mat_t* d = mat_sub(a, b);
    check_mat(s, 2, 3, sum);
    check_mat(d, 2, 3, diff);

    // dimensions incompatibles -> NULL
    mat_t* c = mat_ones(3, 2);
    assert(mat_add(a, c) == NULL);
    assert(mat_sub(a, c) == NULL);
    // pointeur NULL -> NULL
    assert(mat_add(NULL, b) == NULL);
    assert(mat_sub(a, NULL) == NULL);

    mat_free(a); mat_free(b); mat_free(c); mat_free(s); mat_free(d);
    printf("test_add_sub OK\n");
}

static void test_scale(void){
    float v[] = {1,-2,3,4}, expected[] = {2.5f,-5,7.5f,10};
    mat_t* a = mat_create(2, 2, v);
    mat_t* r = mat_scale(a, 2.5f);
    check_mat(r, 2, 2, expected);
    assert(mat_scale(NULL, 2.0f) == NULL);
    mat_free(a); mat_free(r);
    printf("test_scale OK\n");
}

static void test_get_set(void){
    mat_t* a = mat_zeros(3, 4);
    mat_set(a, 2, 3, 42.0f);              // dernier élément valide
    assert(mat_get(a, 2, 3) == 42.0f);
    assert(mat_get(a, 0, 0) == 0.0f);     // pas d'effet de bord ailleurs

    // hors limites : le cas piégeux (col >= cols mais index linéaire valide)
    assert(isnan(mat_get(a, 0, 5)));
    assert(isnan(mat_get(a, 3, 0)));
    mat_set(a, 0, 5, 1.0f);               // ne doit pas écrire ni crasher
    assert(mat_get(a, 1, 1) == 0.0f);     // (1,1) n'a pas été touché par (0,5)

    assert(isnan(mat_get(NULL, 0, 0)));
    mat_set(NULL, 0, 0, 1.0f);            // ne doit pas crasher
    mat_free(a);
    printf("test_get_set OK\n");
}

static void test_transpose(void){
    // rectangulaire : une carrée ne détecterait pas une confusion rows/cols
    float v[] = {1,2,3,4,5,6};
    float expected[] = {1,4, 2,5, 3,6};
    mat_t* a = mat_create(2, 3, v);
    mat_t* t = mat_transpose(a);
    check_mat(t, 3, 2, expected);

    // transposer deux fois = identité
    mat_t* tt = mat_transpose(t);
    check_mat(tt, 2, 3, v);

    assert(mat_transpose(NULL) == NULL);
    mat_free(a); mat_free(t); mat_free(tt);
    printf("test_transpose OK\n");
}

static void test_mul(void){
    float va[] = {1,2,3,4,5,6};            // 2x3
    float vb[] = {7,8,9,10,11,12};         // 3x2
    float expected[] = {58,64,139,154};    // 2x2
    mat_t* a = mat_create(2, 3, va);
    mat_t* b = mat_create(3, 2, vb);
    mat_t* c = mat_mul(a, b);
    check_mat(c, 2, 2, expected);

    // B x A -> 3x3 (autre forme, détecte les erreurs de pas)
    float expected2[] = {39,54,69, 49,68,87, 59,82,105};
    mat_t* d = mat_mul(b, a);
    check_mat(d, 3, 3, expected2);

    // dimensions incompatibles : (2x3) x (2x3)
    assert(mat_mul(a, a) == NULL);
    assert(mat_mul(NULL, b) == NULL);

    mat_free(a); mat_free(b); mat_free(c); mat_free(d);
    printf("test_mul OK\n");
}

static void test_mul_identity(void){
    float vi[] = {1,0,0, 0,1,0, 0,0,1};
    float va[] = {1,2,3,4,5,6,7,8,9};
    mat_t* i3 = mat_create(3, 3, vi);
    mat_t* a  = mat_create(3, 3, va);
    mat_t* r  = mat_mul(a, i3);
    check_mat(r, 3, 3, va);
    mat_free(i3); mat_free(a); mat_free(r);
    printf("test_mul_identity OK\n");
}

// Référence naïve indépendante : servira à valider toutes les futures versions optimisées
static void test_mul_vs_reference(void){
    size_t m = 13, k = 17, n = 11;   // tailles volontairement non multiples de 8
    mat_t* a = mat_zeros(m, k);
    mat_t* b = mat_zeros(k, n);
    for(size_t i = 0; i < m; i++) for(size_t j = 0; j < k; j++) mat_set(a, i, j, (float)((i*7 + j*3) % 10) - 4.0f);
    for(size_t i = 0; i < k; i++) for(size_t j = 0; j < n; j++) mat_set(b, i, j, (float)((i*5 + j*2) % 9) - 3.0f);

    mat_t* c = mat_mul(a, b);
    assert(mat_rows(c) == m && mat_cols(c) == n);
    for(size_t i = 0; i < m; i++){
        for(size_t j = 0; j < n; j++){
            float ref = 0.0f;
            for(size_t p = 0; p < k; p++) ref += mat_get(a, i, p) * mat_get(b, p, j);
            assert(fabsf(mat_get(c, i, j) - ref) < 1e-3f);
        }
    }
    mat_free(a); mat_free(b); mat_free(c);
    printf("test_mul_vs_reference OK\n");
}

int main(void){
    test_create_get();
    test_zeros_ones();
    test_add_sub();
    test_scale();
    test_get_set();
    test_transpose();
    test_mul();
    test_mul_identity();
    test_mul_vs_reference();
    printf("\nAll matrix tests passed.\n");
    return 0;
}