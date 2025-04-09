
// DEMO2.CPP - 24H2 / VS 17.13 safe SSE-only intrinsics, with native ARM64 support

#define  USE_SOFT_INTRINSICS 1

#include <windows.h>
#include <intrin.h>   // for clarity. not really needed as is implicitly included by windows.h above
#include <stdio.h>
#include <stdint.h>


int X = 42;

__m128 Vec128;


// __m256 does not compile under -arm64EC due to unnecessary compiler block

__m256 Vec256;


#include <fvec.h>
#include <dvec.h>

M128    BaseVector128;
F64vec2 Vector128;
F64vec4 Vector256;

