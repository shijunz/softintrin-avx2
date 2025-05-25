//
// AVX2_INTRIN_BUG.C
//
// Copyright (c) 2025 Darek Mihocka
//
// Demonstrates buggy -O2 code generation with -arch:AVX2 which may leak private data.
//
// This is a stripped down repro case of the AVX2 intrinsic test at:
//   https://github.com/softmac/softintrin-avx2/tree/main/test_intrins
//
// where it was observed that the X64 compiler is erroniously spilling XMM registers
// to stack locals and reloading them as YMM registers.  This loads bogus data into
// the top 128 bits of the YMM registers which leaks from other stack locals.
// It can be observed that the leaked data includes addresses and other private data.
//
// In this stripped down example, the 256-bit AVX2 shift left and shift right intrinsics
// are called, with inputs of all zeroes, meaning that the output should also be all
// zeroes (i.e. zero inputs shifted by zero == zero output).
//
// What is observed is that the lower 128 bits of each result are in fact zero, but the
// upper 128 bits contain non-zero data, some which are obvious memory pointers.
//
// Repros with latest AMD64 x64 compiler from Visual Studio 2022 17.14.2 (May 2025)
//
// To repro, open VCVARS64.BAT build environment and build with this command line (or run the BUILD-REPRO.BAT script):
//
// cl -FAsc -Zi -O2 -I.. -arch:AVX2 -Tc avx2_intrin_bug.c -link -out:avx2_intrin_bug.exe -debug -release -incremental:no
//
// Then repro by simply running the generated executable AVX2_INTRIN_BUG.EXE to observe the non-zero output.
//
// Recompiling with -O1 (instead of -O2) makes the bug go away, this appears to be a function inlining expansion bug maybe?
//
// Another workaround is to make the source inputs as 'volatile', uncomment the volatile type cast in this line (line 223 below):
//
//   { Vout[index]._ ## type_ret = op ( Vsrc[index + 0]._ ## type_a, /* *(volatile type_b *)& */ Vsrc[index + 1]._ ## type_b ); } \
//
// This fixes the problem by correctly loading the source as a 256-bit value, but why is this necessary?
//

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <intrin.h>
#include <windows.h>

#include "debug_vec.h"

//
// union of all possible scalar and vector types
// totalling 256 bits = 32 bytes in size
//

typedef void      * pvoid;
typedef float     * pfloat;
typedef double    * pdouble;
typedef __int8    * pint8;
typedef __int16   * pint16;
typedef __int32   * pint32;
typedef __int64   * pint64;

typedef __m128    * p__m128;
typedef __m128d   * p__m128d;
typedef __m128i   * p__m128i;

#if defined(__AVX2__) || (defined(USE_SOFT_INTRINSICS) && (USE_SOFT_INTRINSICS >= 2))

typedef __m256    * p__m256;
typedef __m256d   * p__m256d;
typedef __m256i   * p__m256i;

#endif

typedef struct BIGVEC
{
  // first 32 bytes is the data portion

  union
  {
    uint8_t    bytes[32];  // for access to raw bytes

    float      _float;
    double     _double;

    int        _int;

    __int8     ___int8;
    __int16    ___int16;
    __int32    ___int32;
    __int64    ___int64;

    __m128     ___m128;
    __m128d    ___m128d;
    __m128i    ___m128i;

    __m128     __am128[2];
    __m128d    __am128d[2];
    __m128i    __am128i[2];

#if defined(__AVX2__) || (defined(USE_SOFT_INTRINSICS) && (USE_SOFT_INTRINSICS >= 2))
    __m256     ___m256;
    __m256d    ___m256d;
    __m256i    ___m256i;
#endif
  };

  // followed by pointers to corresponding output vector
  union
  {
    pvoid      _pvoid;
    pfloat     _pfloat;
    pdouble    _pdouble;

    pint8      _p__int8;
    pint16     _p__int16;
    pint32     _p__int32;
    pint64     _p__int64;

    p__m128    _p__m128;
    p__m128d   _p__m128d;
    p__m128i   _p__m128i;

#if defined(__AVX2__) || (defined(USE_SOFT_INTRINSICS) && (USE_SOFT_INTRINSICS >= 2))
    p__m256    _p__m256;
    p__m256d   _p__m256d;
    p__m256i   _p__m256i;
#endif
  };

} BIGVEC;

C_ASSERT((sizeof(BIGVEC) >= 32) || (sizeof(BIGVEC) <= 64));

#define NUM_BIGVECS (16)

__declspec(align(32)) BIGVEC Vout[NUM_BIGVECS];
__declspec(align(32)) BIGVEC Vsrc[NUM_BIGVECS];

// const __m256i Zero256;  // a zero-ed 32-byte block

//
// Clear the output vectors and initialize the input vectors
// prior to running each unique intrinsics test, and also
// record the timestamp of each
//

__declspec(noinline)
bool init_vecs(char *OpName)
{
    for (unsigned i = 0; i < NUM_BIGVECS; i++)
    {
        // set the corresponding pointer to the output vector

        Vsrc[i]._pvoid   = (void *)&Vout[i];

        // clear the corresponding output vector in an SSE-friendly way

        Vsrc[i]._p__int64[0] =
        Vsrc[i]._p__int64[1] =
        Vsrc[i]._p__int64[2] =
        Vsrc[i]._p__int64[3] = 0;

        Vsrc[i].__am128i[0] = _mm_set1_epi64x(0);
        Vsrc[i].__am128i[1] = _mm_set1_epi64x(0);
    }

    return true;
}

//
// Run until a certain time has elapsed for micro-benchmarking
//

__forceinline
int Run(void)
{
    return 0;
}

//
// Dump out the output vectors after running a test
//

__declspec(noinline)
void dump_vecs(char *OpName)
{
    printf("Results of %s\n", OpName);

    for (unsigned i = 0; i < NUM_BIGVECS - 2; i++)
    {
#if defined(__AVX2__) || (defined(USE_SOFT_INTRINSICS) && (USE_SOFT_INTRINSICS >= 2))
        printf("I%4u.", i); d256_u32("8u: ", Vsrc[i].___m256i);
        printf("I%4u.", i); d256_u32("8u: ", Vsrc[i+1].___m256i);
        printf("O%4u.", i); d256_f64("4d: ", Vout[i].___m256d);
        printf("O%4u.", i); d256_f32("8s: ", Vout[i].___m256);
        printf("O%4u.", i); d256_u32("8u: ", Vout[i].___m256i);
#else
        printf("I%4u.", i); d128_f64("2d: ", Vsrc[i].___m128d);
        printf("O%4u.", i); d128_f64("2d: ", Vout[i].___m128d);
        printf("O%4u.", i); d128_f32("4s: ", Vout[i].___m128);
#endif
    }
}

//
// Macros to define the test functions
//

#define DEFINE_TEST_OP_R(op, type_ret) \
__forceinline void __cdecl test ## op         (unsigned index) { Vout[index]._ ## type_ret = op ( ); }

#define DEFINE_TEST_OP_RA(op, type_ret, type_a) \
__forceinline void __cdecl test ## op         (unsigned index) { Vout[index]._ ## type_ret = op ( Vsrc[index + 0]._ ## type_a ); }

// Visual Studio 2022 17.14 generates incorrect -O2 optimization for _mm256_sll_epi* _mm256_srl_epi* _mm256_sra_epi* intrinsics
// (due to the mix of m256 and m128 data types it seems?) causing unnecessary and incorrect vector register spills to the stack.
// This workaround using volatile temporarily works around the bugs.  The bug does not occur with -Od or -O1 but lowering the
// optimization level is not acceptable for micro-benchmarking purposes!

#define DEFINE_TEST_OP_RAB(op, type_ret, type_a, type_b) \
__forceinline void __cdecl test ## op         (unsigned index) { \
   if ((sizeof(type_a) == 32) && (sizeof(type_b) == 16)) \
       { Vout[index]._ ## type_ret = op ( Vsrc[index + 0]._ ## type_a, /* *(volatile type_b *)& */ Vsrc[index + 1]._ ## type_b ); } \
    else \
       { Vout[index]._ ## type_ret = op ( Vsrc[index + 0]._ ## type_a, Vsrc[index + 1]._ ## type_b ); } \
}

#define DEFINE_TEST_OP_RABC(op, type_ret, type_a, type_b, type_c) \
__forceinline void __cdecl test ## op         (unsigned index) { Vout[index]._ ## type_ret = op ( Vsrc[index + 0]._ ## type_a, Vsrc[index + 1]._ ## type_b, Vsrc[index + 2]._ ## type_c ); }

#define DEFINE_TEST_OP_RAI(op, type_ret, type_a, imm8) \
__forceinline void __cdecl test ## op ## imm8 (unsigned index) { Vout[index]._ ## type_ret = op ( Vsrc[index + 0]._ ## type_a,                              imm8 ); }

#define DEFINE_TEST_OP_RABI(op, type_ret, type_a, type_b, imm8) \
__forceinline void __cdecl test ## op ## imm8 (unsigned index) { Vout[index]._ ## type_ret = op ( Vsrc[index + 0]._ ## type_a, Vsrc[index + 1]._ ## type_b, imm8 ); }

#define DEFINE_TEST_OP_VAB(op, type_a, type_b) \
__forceinline void __cdecl test ## op         (unsigned index) {                             op ( Vsrc[index + 0]._ ## type_a, Vsrc[index + 1]._ ## type_b ); }

#include "intrin-list.h"

//
// Macros to define the test invocation
//

#undef  DEFINE_TEST_OP_R
#undef  DEFINE_TEST_OP_RA
#undef  DEFINE_TEST_OP_RAB
#undef  DEFINE_TEST_OP_RABC
#undef  DEFINE_TEST_OP_RAI
#undef  DEFINE_TEST_OP_RABI
#undef  DEFINE_TEST_OP_VAB

#define EXECUTE_TEST_OP(op)         do { if (init_vecs(# op)) { do { for (unsigned i = 0; i < (NUM_BIGVECS - 2); i++) { test ## op (i);         } } while (Run()); dump_vecs(# op); } } while(0);
#define EXECUTE_TEST_OP_I(op, imm8) do { if (init_vecs(# op)) { do { for (unsigned i = 0; i < (NUM_BIGVECS - 2); i++) { test ## op ## imm8 (i); } } while (Run()); dump_vecs(# op); } } while(0);

#define DEFINE_TEST_OP_R(   op, type_ret)                          EXECUTE_TEST_OP  (op)
#define DEFINE_TEST_OP_RA(  op, type_ret, type_a)                  EXECUTE_TEST_OP  (op)
#define DEFINE_TEST_OP_RAB( op, type_ret, type_a, type_b)          EXECUTE_TEST_OP  (op)
#define DEFINE_TEST_OP_RABC(op, type_ret, type_a, type_b, type_c)  EXECUTE_TEST_OP  (op)
#define DEFINE_TEST_OP_RAI( op, type_ret, type_a,         imm8)    EXECUTE_TEST_OP_I(op, imm8)
#define DEFINE_TEST_OP_RABI(op, type_ret, type_a, type_b, imm8)    EXECUTE_TEST_OP_I(op, imm8)
#define DEFINE_TEST_OP_VAB( op,           type_a, type_b)          EXECUTE_TEST_OP  (op)

void RunTests(void)
{
    #include "intrin-list.h"
}

int main (int argc, char **argv)
{
    RunTests();

    return 0;
}

