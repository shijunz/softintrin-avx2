//
// TEST_INTRINS.C
//
// Copyright (c) 2025 Darek Mihocka
//
// Validate and micro-benchmark various x64/SSE/AVX intrinsics.
//
// Validation steps:
// - build as native x64 and run on AMD/Intel hardware to capture reference output.
// - run that same x64 binary under emulation (Prism, Rosetta, etc.) to validate emulator.
// - re-build as ARM64EC to validate the Windows on ARM soft intrinsics
//
// Some intrinsics may have different behaviour between AMD, Intel, emulation, and soft intrinics.
// For example, reciprocals such as RCPSS and RSQRTSS, due to leeway in the x86/x64 spec.
//

#if !defined USE_SOFT_INTRINSICS && !defined _M_ARM64EC
#include <use_soft_intrinsics.h>  // always place first
#endif

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

uint64_t StartTick, StopTick;
uint64_t BenchmarkMinimum;
uint64_t Iterations;

char *SearchString;

__declspec(noinline)
bool init_vecs(char *OpName)
{
    // skip this test if there is a search string miss

    if ((SearchString != 0) && (0 == strstr(OpName, SearchString)))
        return false;

    for (unsigned i = 0; i < NUM_BIGVECS; i++)
    {
        // set the corresponding pointer to the output vector

        Vsrc[i]._pvoid   = (void *)&Vout[i];

        // clear the corresponding output vector in an SSE-friendly way

        Vsrc[i]._p__int64[0] =
        Vsrc[i]._p__int64[1] =
        Vsrc[i]._p__int64[2] =
        Vsrc[i]._p__int64[3] = 0;

        // initialize the input vector in an SSE-friendly way

        int swap = (i >> 1) & 1;

        switch((i >> 2) & 3)
        {
            case 0:     // focus on int32
                Vsrc[i].__am128i[0 ^ swap] = _mm_set_epi32(0xFFFFFFFF, i * 0x379BDF15, 0, i);
                Vsrc[i].__am128i[1 ^ swap] = _mm_set_epi32(i * 0x01020305, i * 0x88888888, i * 0xFFFFFFFF, 0xFFFFFFFF / (i + 1));
                break;

            case 1:     // focus on int64
                Vsrc[i].__am128i[0 ^ swap] = _mm_set_epi64x((long long)i * 0x88888888'88888888, i);
                Vsrc[i].__am128i[1 ^ swap] = _mm_set_epi64x((long long)i * 0x0102030507090B0Dll, 0xFFFFFFFF'FFFFFFFF);
                break;

            case 2:     // focus on float / f32
                Vsrc[i].__am128[0 ^ swap] = _mm_set_ps((float)i * -1.0f, (float)i * -1.4142136f, 0.0f, (float)i);
                Vsrc[i].__am128[1 ^ swap] = _mm_set_ps((float)i * 3.14159f, -1.0f, (float)i * -999.999f, 8.0f);
                break;

            case 3:     // focus on double / 64
                Vsrc[i].__am128d[0 ^ swap] = _mm_set_pd((double)i * -1.4142136, (double)i);
                Vsrc[i].__am128d[1 ^ swap] = _mm_set_pd((double)i * 3.14159, (double)i * -1.0);
                break;
        }
    }

    Iterations = 0;
    StartTick = GetTickCount64();

    // wait for the clock edge

    do
    {
        StopTick = GetTickCount64();
    } while (StartTick == StopTick);

    StartTick = StopTick;

    return true;
}

//
// Run until a certain time has elapsed for micro-benchmarking
//

__forceinline
int Run(void)
{
    StopTick = GetTickCount64();
    Iterations += NUM_BIGVECS - 2;

    if ((StopTick - StartTick) < BenchmarkMinimum)
        return 1;

    return 0;
}

//
// Dump out the output vectors after running a test
//

__declspec(noinline)
void dump_vecs(char *OpName)
{
    if (BenchmarkMinimum)
    {
        uint64_t OpsPerSec = Iterations * 1000ull / (StopTick - StartTick);
        double   PsecPerOp = 1000000000000.0  / (OpsPerSec + 1);

        printf("%-25s  %9.0f ps/op\n", OpName, PsecPerOp);

        return;
    }

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

#define DEFINE_TEST_OP_RAB(op, type_ret, type_a, type_b) \
__forceinline void __cdecl test ## op         (unsigned index) { Vout[index]._ ## type_ret = op ( Vsrc[index + 0]._ ## type_a, Vsrc[index + 1]._ ## type_b ); }

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

// for timeBeginPeriod / timeEndPeriod
#pragma comment(linker, "/defaultlib:winmm")

int main (int argc, char **argv)
{
    for (int argi = 1; argi < argc; argi++)
    {
        if (argv[argi][0] == '%')
            continue;

        if ((argv[argi][0] == '-') || (argv[argi][0] == '/'))
        {
            switch (argv[argi][1])
            {
              default:
                printf("Unknown option %s\n", argv[argi]);

              case '?':
Usage:
                printf("Usage: %s [-b|-B] [-f search_string] [-o output_file]\n", argv[0]);
                return 0;

              case 'b':
                BenchmarkMinimum = 150;  // 150 milliseconds per intrinsic
                break;

              case 'B':
                BenchmarkMinimum = 500;  // 500 milliseconds per intrinsic (more precision in the timing)
                break;

              case 'f':                  // filter by string
                if (argc <= ++argi)
                    goto Usage;

                SearchString = argv[argi];
                break;

              case 'o':                  // redirect output to a file, useful for TTD tracing
                if (argc <= ++argi)
                    goto Usage;

                freopen(argv[argi], "w+", stdout);
                break;
            }
        }
    }

    timeBeginPeriod(1);

    RunTests();

    timeEndPeriod(1);
    return 0;
}

