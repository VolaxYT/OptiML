# OptiML

## Optimisation vec_dot

Problème : gcc vectorise partiellement la boucle dans la fonction vec_dot de vector.h. 
La multiplication est vectorisée mais l'accumulateur result non.

Diagnostic : assembleur du fichier vector.c et benchmark 
```
vec_dot(n=64      ) :   0.0001 ms/iter |   1.36 GFLOPS
vec_dot(n=256     ) :   0.0006 ms/iter |   0.87 GFLOPS
vec_dot(n=1024    ) :   0.0028 ms/iter |   0.74 GFLOPS
vec_dot(n=4096    ) :   0.0115 ms/iter |   0.72 GFLOPS
vec_dot(n=16384   ) :   0.0462 ms/iter |   0.71 GFLOPS
vec_dot(n=65536   ) :   0.1835 ms/iter |   0.71 GFLOPS
vec_dot(n=262144  ) :   0.7728 ms/iter |   0.68 GFLOPS
vec_dot(n=1048576 ) :   3.1736 ms/iter |   0.66 GFLOPS
```

Cause : l'addition flottante n'est pas associative.

Solution : accumulation vectorielle manuelle (par intrinsics AVX2)

Résultat :

```
vec_dot(n=64       ) :   0.000016 ms/iter |   7.82 GFLOPS
vec_dot(n=256      ) :   0.000056 ms/iter |   9.09 GFLOPS
vec_dot(n=1024     ) :   0.000274 ms/iter |   7.46 GFLOPS
vec_dot(n=4096     ) :   0.001357 ms/iter |   6.04 GFLOPS
vec_dot(n=16384    ) :   0.005708 ms/iter |   5.74 GFLOPS
vec_dot(n=65536    ) :   0.023294 ms/iter |   5.63 GFLOPS
vec_dot(n=262144   ) :   0.108610 ms/iter |   4.83 GFLOPS
vec_dot(n=1048576  ) :   0.829620 ms/iter |   2.53 GFLOPS
vec_dot(n=4194304  ) :   4.249892 ms/iter |   1.97 GFLOPS
vec_dot(n=16777216 ) :  18.261078 ms/iter |   1.84 GFLOPS
vec_dot(n=67108864 ) :  66.421624 ms/iter |   2.02 GFLOPS
```

## Optimisation vec_norm_l1

Même problème que pour vec_dot, même solution.
Seule différence, besoin de créer un masque pour "retirer" le bit de signe des calculs. (comme ça pour garder la vectorisation)

## Optimisation mat_mul

Multiplication naïve dans l'ordre i,j,k :
```
mat_mul(n=64   ) :      0.743 ms |   0.71 GFLOPS
mat_mul(n=128  ) :      6.213 ms |   0.68 GFLOPS
mat_mul(n=256  ) :     47.824 ms |   0.70 GFLOPS
mat_mul(n=512  ) :    413.480 ms |   0.65 GFLOPS
mat_mul(n=1024 ) :   6937.906 ms |   0.31 GFLOPS
mat_mul(n=2048 ) : 147170.633 ms |   0.12 GFLOPS
```

Multiplication dans l'ordre i,k,j :
```
mat_mul(n=64   ) :      0.066 ms |   7.90 GFLOPS
mat_mul(n=128  ) :      0.731 ms |   5.74 GFLOPS
mat_mul(n=256  ) :      4.097 ms |   8.19 GFLOPS
mat_mul(n=512  ) :     33.128 ms |   8.10 GFLOPS
mat_mul(n=1024 ) :    347.889 ms |   6.17 GFLOPS
mat_mul(n=2048 ) :   3373.739 ms |   5.09 GFLOPS
```

Multiplication dans l'ordre i,k,j par bloc (BLOCK=32) :
```
mat_mul(n=64   ) :      0.078 ms |   6.68 GFLOPS
mat_mul(n=128  ) :      0.773 ms |   5.43 GFLOPS
mat_mul(n=256  ) :      5.231 ms |   6.41 GFLOPS
mat_mul(n=512  ) :     41.339 ms |   6.49 GFLOPS
mat_mul(n=1024 ) :    325.616 ms |   6.60 GFLOPS
mat_mul(n=2048 ) :   2896.998 ms |   5.93 GFLOPS
```

Multiplication dans l'ordre i,k,j par bloc (BLOCK=64) :
```
mat_mul(n=64   ) :      0.059 ms |   8.95 GFLOPS
mat_mul(n=128  ) :      0.411 ms |  10.21 GFLOPS
mat_mul(n=256  ) :      4.405 ms |   7.62 GFLOPS
mat_mul(n=512  ) :     34.871 ms |   7.70 GFLOPS
mat_mul(n=1024 ) :    256.525 ms |   8.37 GFLOPS
mat_mul(n=2048 ) :   2033.615 ms |   8.45 GFLOPS
```

Multiplication dans l'ordre i,k,j par bloc (BLOCK=128) :
```
mat_mul(n=64   ) :      0.063 ms |   8.28 GFLOPS
mat_mul(n=128  ) :      0.479 ms |   8.75 GFLOPS
mat_mul(n=256  ) :      4.203 ms |   7.98 GFLOPS
mat_mul(n=512  ) :     31.104 ms |   8.63 GFLOPS
mat_mul(n=1024 ) :    241.928 ms |   8.88 GFLOPS
mat_mul(n=2048 ) :   1833.867 ms |   9.37 GFLOPS
```

Multiplication dans l'ordre i,k,j par bloc (BLOCK=256) :
```
mat_mul(n=64   ) :      0.058 ms |   9.11 GFLOPS
mat_mul(n=128  ) :      0.479 ms |   8.76 GFLOPS
mat_mul(n=256  ) :      3.554 ms |   9.44 GFLOPS
mat_mul(n=512  ) :     30.276 ms |   8.87 GFLOPS
mat_mul(n=1024 ) :    219.195 ms |   9.80 GFLOPS
mat_mul(n=2048 ) :   2930.717 ms |   5.86 GFLOPS
```

Multiplication blocked (128) micro-kernel SIMD 4*16 :
```
mat_mul(n=64   ) :      0.016 ms |  33.24 GFLOPS
mat_mul(n=128  ) :      0.123 ms |  33.97 GFLOPS
mat_mul(n=256  ) :      1.053 ms |  31.87 GFLOPS
mat_mul(n=512  ) :      9.068 ms |  29.60 GFLOPS
mat_mul(n=1024 ) :     67.918 ms |  31.62 GFLOPS
mat_mul(n=2048 ) :    518.401 ms |  33.14 GFLOPS
```

Multiplication NumPy 1 thread :
```
numpy matmul(n=64   ) :      0.020 ms |   26.23 GFLOPS
numpy matmul(n=128  ) :      0.134 ms |   31.20 GFLOPS
numpy matmul(n=256  ) :      0.961 ms |   34.92 GFLOPS
numpy matmul(n=512  ) :      7.577 ms |   35.43 GFLOPS
numpy matmul(n=1024 ) :     54.469 ms |   39.43 GFLOPS
numpy matmul(n=2048 ) :    423.956 ms |   40.52 GFLOPS
```

Multiplication NumPy multi-thread :
```
numpy matmul(n=64   ) :      0.020 ms |   25.73 GFLOPS
numpy matmul(n=128  ) :     12.003 ms |    0.35 GFLOPS
numpy matmul(n=256  ) :     11.987 ms |    2.80 GFLOPS
numpy matmul(n=512  ) :     19.997 ms |   13.42 GFLOPS
numpy matmul(n=1024 ) :     24.804 ms |   86.58 GFLOPS
numpy matmul(n=2048 ) :    121.778 ms |  141.08 GFLOPS
```