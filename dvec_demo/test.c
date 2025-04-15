//
// TEST.C
//
// trivial C demo of 256-bit AVX2 vector types
// and intrinsics with ARM64 and ARM64EC
//

#include <use_soft_intrinsics.h>

#include <windows.h>
#include <intrin.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <intrin.h>     // intentional redundant include
#include <windows.h>    // intentional redundant include

#include "debug_vec.h"

static __m256  f, g;
static __m256d d, e;
static __m256i i, j;

__m256 __vectorcall foo()
{
    __m256 T = _mm256_andnot_ps(f, g);

    T = _mm256_and_ps(T, g);
    T = _mm256_or_ps(f, T);
    T = _mm256_xor_ps(T, g);

    return T;
}

__m256d __vectorcall doo()
{
    __m256d T = _mm256_andnot_pd(d, e);

    T = _mm256_and_pd(T, e);
    T = _mm256_or_pd(d, T);
    T = _mm256_xor_pd(T, e);

    return T;
}

__m256i __vectorcall goo()
{
    __m256i T = _mm256_andnot_si256(i, j);

    T = _mm256_and_si256(T, j);
    T = _mm256_or_si256(i, T);
    T = _mm256_xor_si256(T, j);

    return T;
}

extern int __vectorcall TestDvec();

int __cdecl main()
{
    __m256d D = _mm256_set_pd(3.1415926535897932, 1.618033988749894848, 2.0, -1.0);
    __m256d E = _mm256_set1_pd(2.82842712474619f);
    _mm256_storeu_pd((double *)&d, D);
    _mm256_storeu_pd((double *)&e, E);
    d256_f64("D.4d=0", D);
    d256_f64("E.4d=0", E);

    __m256 F = _mm256_set1_ps(2.82842712474619f);
    __m256 G = _mm256_set_ps(1.1111f, -2.2222f, 3.3333f, -4.4444f, 5.5555f, -6.6666f, 7.7777f, -8.8888f);
    _mm256_storeu_ps((float *)&f, F);
    _mm256_storeu_ps((float *)&g, G);
    d256_f32("F.8s=0", F);
    d256_f32("G.8s=0", G);

    F = foo();
    d256_f32("T.8s=0", F);

    D = doo();
    d256_f64("T.4d=0", D);

    __m256i I = goo();
    d256_u32("I.8l=0", I);
    d256_u64("I.4q=0", I);

    D = _mm256_loadu_pd((double *)&e);
    E = _mm256_loadu_pd((double *)&d);
    F = _mm256_loadu_ps((float *)&g);
    G = _mm256_loadu_ps((float *)&f);
    d256_f64("d.4d=0", E);
    d256_f64("e.4d=0", D);
    d256_f32("f.8s=0", G);
    d256_f32("g.8s=0", F);

    TestDvec();
    return 0;
}

