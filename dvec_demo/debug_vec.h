
#ifndef DEBUG_SOFT_INTRINSICS
#define DEBUG_SOFT_INTRINSICS

//
// Helpful debug functions (C and C++ compatible)
//

#include <stdio.h>

#if defined(__AVX2__) || (defined(USE_SOFT_INTRINSICS) && (USE_SOFT_INTRINSICS >= 2))

static inline __forceinline
void d256_u32(const char *psz, __m256i T)
{
    printf("%-8s %14X %14X %14X %14X %14X %14X %14X %14X\n", psz, T.m256i_u32[7], T.m256i_u32[6], T.m256i_u32[5], T.m256i_u32[4], T.m256i_u32[3], T.m256i_u32[2], T.m256i_u32[1], T.m256i_u32[0]);
}

static inline __forceinline
void d256_u64(const char *psz, __m256i T)
{
    printf("%-8s %29llX %29llX %29llX %29llX\n", psz, T.m256i_u64[3], T.m256i_u64[2], T.m256i_u64[1], T.m256i_u64[0]);
}

static inline __forceinline
void d256_f32(const char *psz, __m256 T)
{
    printf("%-8s %14g %14g %14g %14g %14g %14g %14g %14g\n", psz, T.m256_f32[7], T.m256_f32[6], T.m256_f32[5], T.m256_f32[4], T.m256_f32[3], T.m256_f32[2], T.m256_f32[1], T.m256_f32[0]);
}

static inline __forceinline
void d256_f64(const char *psz, __m256d T)
{
    printf("%-8s %29g %29g %29g %29g\n", psz, T.m256d_f64[3], T.m256d_f64[2], T.m256d_f64[1], T.m256d_f64[0]);
}

#endif // AVX2

static inline __forceinline
void d128_f32(const char *psz, __m128 T)
{
    printf("%-8s %74g %14g %14g %14g\n", psz, T.m128_f32[3], T.m128_f32[2], T.m128_f32[1], T.m128_f32[0]);
}

static inline __forceinline
void d128_f64(const char *psz, __m128d T)
{
    printf("%-8s %89g %29g\n", psz, T.m128d_f64[1], T.m128d_f64[0]);
}

#endif  // DEBUG_SOFT_INTRINSICS


