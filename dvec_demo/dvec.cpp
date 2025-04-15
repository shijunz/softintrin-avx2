//
// DVEC.CPP
//
// trivial C++ demo of the 128-bit and 256-bit vector class usage from DVEC.CPP
//

//
// first include header to enable soft intrinsics for ARM64 and ARM64EC
//

#include <use_soft_intrinsics.h>

//
// then include the usual headers
//

#include <windows.h>
#include <intrin.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ostream>

//
// then include the new softintrins, this will append to the softintrin.h already included
// but added here for clarity.
//

#include <softintrin_avx2.h>

//
// some useful vector output functions
//

#include "debug_vec.h"

//
// now include DVEC.H which normally would fail to complile for ARM64 and ARM64EC
//

#include <dvec.h>
#include <fvec.h>
#include <ivec.h>

__m64 MM;
__m128 F;
__m128d D;
__m128i I;

EXTERN_C
int __vectorcall TestDvec()
{
    F32vec8 W1(0.0f, -1.12345f, 2.23456f, -3.34567f, 4.45678f, -5.56789f, 6.61234f, -7.72345f);
    F32vec8 W2(8.8f);

    d256_f32("W1.8s  ", (W1));
    d256_f32("abs W1 ", abs(W1));
    d256_f32("W2.8s  ", (W2));
    d256_f32("min    ", simd_min(W1, W2));
    d256_f32("max    ", simd_max(W1, W2));
    d256_f32("W1 + W2", (W1 + W2));
    d256_f32("W2 - W1", (W2 - W1));
    d256_f32("W1 * W2", (W1 * W2));
    d256_f32("W1 / W2", (W1 / W2));
    d256_f32("W1 | W2", (W1 | W2));
    d256_f32("W1 ^ W2", (W1 ^ W2));
    d256_f32("W1 & W2", (W1 & W2));
    d256_f32("~W1& W2", andnot(W1, W2));
    d256_f32("W1h+ W2", (F32vec8)add_horizontal(W1));
    d256_f32("sqrt W1", sqrt(W1));
    d256_f32("W1 perm", _mm256_permute_ps(W1, 0xEE));
    d256_f32("W1 perm", _mm256_permute_ps(W1, 0x36));
    d256_f32("W1+perm", _mm256_add_ps(W1, _mm256_permute_ps(W1, 0xEE)));
    d256_f32("W1+perm", _mm256_add_ps(W1, _mm256_permute_ps(W1, 0x36)));
    d256_f32("W1 hdup", _mm256_movehdup_ps(W1));
    d256_f32("W1 ldup", _mm256_moveldup_ps(W1));
    d128_f32("W1 extr", _mm256_extractf128_ps(W1, 0));
    d128_f32("W1 extr", _mm256_extractf128_ps(W1, 1));
    d128_f32("W1 sqss", _mm_sqrt_ss(_mm256_extractf128_ps(W1, 0)));
    d128_f32("W2 sqss", _mm_sqrt_ss(_mm256_extractf128_ps(W2, 0)));
    d256_f64("W1 pspd", _mm256_cvtps_pd(_mm256_extractf128_ps(W1, 0)));

    F64vec4 W3(0.0, -1.12345678, 2.23456789, -3.34567891);
    F64vec4 W4(9.87654321);

    d256_f64("W3.4d  ", (W3));
    d256_f64("abs W3 ", abs(W3));
    d256_f64("W4.4d  ", (W4));
    d256_f64("min    ", simd_min(W3, W4));
    d256_f64("max    ", simd_max(W3, W4));
    d256_f64("W3 + W4", (W3 + W4));
    d256_f64("W4 - W3", (W4 - W3));
    d256_f64("W3 * W4", (W3 * W4));
    d256_f64("W3 / W4", (W3 / W4));
    d256_f64("W3 | W4", (W3 | W4));
    d256_f64("W3 ^ W4", (W3 ^ W4));
    d256_f64("W3 & W4", (W3 & W4));
    d256_f64("~W3& W4", andnot(W3, W4));
    d256_f64("W3h+ W4", (F64vec4)add_horizontal(W3));
    d256_f64("sqrt W3", sqrt(W3));
    d256_f64("W3 perm", _mm256_permute_pd(W3, 0x05));
    d256_f64("W3+perm", _mm256_add_pd(W3, _mm256_permute_pd(W3, 0x05)));
    d128_f64("W3 extr", _mm256_extractf128_pd(W3, 0));
    d128_f64("W3 extr", _mm256_extractf128_pd(W3, 1));
    d128_f64("W3 sqsd", _mm_sqrt_sd(_mm256_extractf128_pd(W3, 0), _mm256_extractf128_pd(W3, 0)));
    d128_f64("W4 sqsd", _mm_sqrt_sd(_mm256_extractf128_pd(W4, 0), _mm256_extractf128_pd(W4, 0)));
    d256_f64("W3 hadd", _mm256_hadd_pd(W3, W3));
    d128_f32("W3 pdps", _mm256_cvtpd_ps(W3));

    F64vec2 V(1.1, 2.2);
    d128_f64("V.2d   ", V);

    return 0;
}

