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
    __m256 F = foo();
    d256_f32("T.8s=0", F);

    __m256d D = doo();
    d256_f64("T.4d=0", D);

    __m256i I = goo();
    d256_u32("I.8l=0", I);
    d256_u64("I.4q=0", I);

    TestDvec();
    return 0;
}

