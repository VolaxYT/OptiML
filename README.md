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