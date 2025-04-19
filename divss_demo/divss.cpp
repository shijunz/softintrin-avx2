//
// DIVSS.CPP
//
//  Copyright (c) 2025 Darek Mihocka
//
//  Checks correctness of SSE2/AVX2 vector divide involving zero inputs.
//  Build with MAKE64.BAT with the following optional arguments:
//
//  on x86/x64 build the AVX2 tests as well:
//
//    - make64.bat -arch:AVX2
//
//  on ARM64 to build the stock SDK-based SSE2-only soft intrinsics tests:
//
//    - make64.bat -arm64EC
//
//  on ARM64 to build ARM64EC test binary using new SSE2+AVX2 intrinsics:
//
//    - make64.bat -arm64EC -FI ../use_soft_intrinsics.h
//
//  on ARM64 to build pure native ARM64 test binary with new SSE+AVX intrinsics:
//
//    - make64.bat -FI ../use_soft_intrinsics.h
//
//  DIVSS.TXT is the expected correct output of SSE2-only tests.
//  DIVSS-AVX2.TXT is the expected correct output of SSE2+AVX2 tests.
//

#include <intrin.h>
#include <stdio.h>

__m128  fZero;
__m128  fNegZero;
__m128  fOne;
__m128  fNegOne;
__m128  fInf;
__m128  fNegInf;

__m128d dZero;
__m128d dNegZero;
__m128d dOne;
__m128d dNegOne;
__m128d dInf;
__m128d dNegInf;

#if defined __AVX2__ || (defined(USE_SOFT_INTRINSICS) && (USE_SOFT_INTRINSICS >= 2))

__m256  ffZero;
__m256  ffNegZero;
__m256  ffOne;
__m256  ffNegOne;
__m256  ffInf;
__m256  ffNegInf;

__m256d ddZero;
__m256d ddNegZero;
__m256d ddOne;
__m256d ddNegOne;
__m256d ddInf;
__m256d ddNegInf;

#endif // AVX2

// handy macro to perfom the division and dump out the result (as hexadecimal and float)

#define TEST(Numerator, Denominator, CvtFunc, DivFunc) do { \
    auto T = DivFunc(Numerator, Denominator); \
    __int64 RawBits = *(__int64 *)&T; \
    printf("0x%016llx ", RawBits); \
    printf("%11.1e = %11.1e / %11.1e\n", CvtFunc(DivFunc(Numerator, Denominator)), CvtFunc(Numerator), CvtFunc(Denominator)); \
  } while (0)

int main()
{
    fZero   = _mm_set1_ps(0.0f);
    fNegZero= _mm_set1_ps(-0.0f);
    fOne    = _mm_set1_ps(1.0f);
    fNegOne = _mm_set1_ps(-1.0f);
    fInf    = _mm_div_ss(fOne, fZero);
    fNegInf = _mm_div_ss(fNegOne, fZero);

    printf("_mm_div_ss:\n");
    TEST(fZero,    fZero,    _mm_cvtss_f32,    _mm_div_ss);
    TEST(fZero,    fNegZero, _mm_cvtss_f32,    _mm_div_ss);
    TEST(fOne,     fZero,    _mm_cvtss_f32,    _mm_div_ss);
    TEST(fOne,     fNegZero, _mm_cvtss_f32,    _mm_div_ss);
    TEST(fNegOne,  fZero,    _mm_cvtss_f32,    _mm_div_ss);
    TEST(fZero,    fOne,     _mm_cvtss_f32,    _mm_div_ss);
    TEST(fZero,    fNegOne,  _mm_cvtss_f32,    _mm_div_ss);
    TEST(fZero,    fNegOne,  _mm_cvtss_f32,    _mm_div_ss);
    TEST(fInf,     fNegInf,  _mm_cvtss_f32,    _mm_div_ss);
    TEST(fNegInf,  fInf,     _mm_cvtss_f32,    _mm_div_ss);
    TEST(fNegInf,  fOne,     _mm_cvtss_f32,    _mm_div_ss);
    TEST(fNegInf,  fZero,    _mm_cvtss_f32,    _mm_div_ss);
    TEST(fNegInf,  fNegZero, _mm_cvtss_f32,    _mm_div_ss);

    printf("_mm_div_ps:\n");
    TEST(fZero,    fZero,    _mm_cvtss_f32,    _mm_div_ps);
    TEST(fZero,    fNegZero, _mm_cvtss_f32,    _mm_div_ps);
    TEST(fOne,     fZero,    _mm_cvtss_f32,    _mm_div_ps);
    TEST(fOne,     fNegZero, _mm_cvtss_f32,    _mm_div_ps);
    TEST(fNegOne,  fZero,    _mm_cvtss_f32,    _mm_div_ps);
    TEST(fZero,    fOne,     _mm_cvtss_f32,    _mm_div_ps);
    TEST(fZero,    fNegOne,  _mm_cvtss_f32,    _mm_div_ps);
    TEST(fOne,     fNegOne,  _mm_cvtss_f32,    _mm_div_ps);
    TEST(fInf,     fNegInf,  _mm_cvtss_f32,    _mm_div_ps);
    TEST(fNegInf,  fInf,     _mm_cvtss_f32,    _mm_div_ps);
    TEST(fNegInf,  fOne,     _mm_cvtss_f32,    _mm_div_ps);
    TEST(fNegInf,  fZero,    _mm_cvtss_f32,    _mm_div_ps);
    TEST(fNegInf,  fNegZero, _mm_cvtss_f32,    _mm_div_ps);

    dZero   = _mm_set1_pd(0.0);
    dNegZero= _mm_set1_pd(-0.0);
    dOne    = _mm_set1_pd(1.0);
    dNegOne = _mm_set1_pd(-1.0);
    dInf    = _mm_div_sd(dOne, dZero);
    dNegInf = _mm_div_sd(dNegOne, dZero);

    printf("_mm_div_sd:\n");
    TEST(dZero,    dZero,    _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dZero,    dNegZero, _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dOne,     dZero,    _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dOne,     dNegZero, _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dNegOne,  dZero,    _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dZero,    dOne,     _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dZero,    dNegOne,  _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dOne,     dNegOne,  _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dInf,     dNegInf,  _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dNegInf,  dInf,     _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dNegInf,  dOne,     _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dNegInf,  dZero,    _mm_cvtsd_f64,    _mm_div_sd);
    TEST(dNegInf,  dNegZero, _mm_cvtsd_f64,    _mm_div_sd);

    printf("_mm_div_pd:\n");
    TEST(dZero,    dZero,    _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dZero,    dNegZero, _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dOne,     dZero,    _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dOne,     dNegZero, _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dNegOne,  dZero,    _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dZero,    dOne,     _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dZero,    dNegOne,  _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dOne,     dNegOne,  _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dInf,     dNegInf,  _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dNegInf,  dInf,     _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dNegInf,  dOne,     _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dNegInf,  dZero,    _mm_cvtsd_f64,    _mm_div_pd);
    TEST(dNegInf,  dNegZero, _mm_cvtsd_f64,    _mm_div_pd);

#if defined __AVX2__ || (defined(USE_SOFT_INTRINSICS) && (USE_SOFT_INTRINSICS >= 2))

    ffZero   = _mm256_set1_ps(0.0f);
    ffNegZero= _mm256_set1_ps(-0.0f);
    ffOne    = _mm256_set1_ps(1.0f);
    ffNegOne = _mm256_set1_ps(-1.0f);
    ffInf    = _mm256_div_ps(ffOne, ffZero);
    ffNegInf = _mm256_div_ps(ffNegOne, ffZero);

    printf("_mm256_div_ps:\n");
    TEST(ffZero,   ffZero,   _mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffZero,   ffNegZero,_mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffOne,    ffZero,   _mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffOne,    ffNegZero,_mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffNegOne, ffZero,   _mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffZero,   ffOne,    _mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffZero,   ffNegOne, _mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffOne,    ffNegOne, _mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffInf,    ffNegInf, _mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffNegInf, ffInf,    _mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffNegInf, ffOne,    _mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffNegInf, ffZero,   _mm256_cvtss_f32, _mm256_div_ps);
    TEST(ffNegInf, ffNegZero,_mm256_cvtss_f32, _mm256_div_ps);

    ddZero   = _mm256_set1_pd(0.0);
    ddNegZero= _mm256_set1_pd(-0.0);
    ddOne    = _mm256_set1_pd(1.0);
    ddNegOne = _mm256_set1_pd(-1.0);
    ddInf    = _mm256_div_pd(ddOne, ddZero);
    ddNegInf = _mm256_div_pd(ddNegOne, ddZero);

    printf("_mm256_div_pd:\n");
    TEST(ddZero,   ddZero,   _mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddZero,   ddNegZero,_mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddOne,    ddZero,   _mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddOne,    ddNegZero,_mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddNegOne, ddZero,   _mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddZero,   ddOne,    _mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddZero,   ddNegOne, _mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddOne,    ddNegOne, _mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddInf,    ddNegInf, _mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddNegInf, ddInf,    _mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddNegInf, ddOne,    _mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddNegInf, ddZero,   _mm256_cvtsd_f64, _mm256_div_pd);
    TEST(ddNegInf, ddNegZero,_mm256_cvtsd_f64, _mm256_div_pd);

#endif // AVX2

    return 0;
}

