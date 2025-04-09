
// DEMO3.CPP - 24H2 / VS 17.13 safe SSE-only intrinsics, with support for __m256 for both ARM64 and ARM64EC

#include <use_soft_intrinsics.h>

#include <windows.h>
#include <intrin.h>   // for clarity. not really needed as is implicitly included by windows.h above
#include <stdio.h>
#include <stdint.h>


int X = 42;

__m128 Vec128;


// __m256 does now compile under -arm64EC thanks to temporary workaround

__m256 Vec256;


#include <fvec.h>
#include <dvec.h>

M128    BaseVector128;
F64vec2 Vector128;
F64vec4 Vector256;

