//
//  SOFTINTRIN_AVX2.H
//
//  Enhancements to the existing "soft intrinsics" provided in the Windows 24H2 SDK (build 10.0.26100):
//
//  - this header file extends the functionality of SOFTINTRIN.H in the 24H2 SDK, it is not a replacement
//    and you must still have the 24H2 10.0.26100 (or higher) SDK installed
//
//  - optimizes common SSE soft intrinsics for both ARM64 and ARM64EC builds which results in a
//    15x speedup for ARM64/ARM64EC builds of ToyPathTracer ray tracing demo, on par with emulation
//
//  - adds dozens of new 256-bit AVX2 soft intrinsics to support using DVEC.H in ARM64/ARM64EC builds
//
//  - smaller binary size due to linking less static code from SOFTINTRIN.LIB library from the SDK
//
//  - native ARM64 binaries produced by this implementation _are_ Windows 10 on ARM compatible and
//    compatible with Snapdragon 835 based devices such as ASUS Novago, HP Envy X2, and Lenovo Miix
//    (whereas ARM64EC has always required Windows 11 on ARM)
//
//  Implementation notes:
//
//  - this is _not_ an SSE/AVX-to-NEON mapping layer per se, although that technique is used
//
//  - Visual C++ ARM64 toolset does not recognize m128 _m256i etc. as native vector types so in some
//    cases the implementation relies of compiler auto-vectorization to generate NEON instructions
//    resulting in faster code sequences than explicitly casting to and from _n128 and NEON intrinsics
//
//  - ARM64 auto-vectorization arrived in Visual C/C++ 2022 17.6, using 17.12 or higher is recommended!
//

#ifndef SOFT_INTRINSICS_AVX2
#define SOFT_INTRINSICS_AVX2

#if defined _M_ARM64EC || defined _M_ARM64

#if !defined(USE_SOFT_INTRINSICS) || (USE_SOFT_INTRINSICS < 2)
#error Need to #include <use_soft_intrinsics.h> before including windows.h or intrin.h
#endif

#ifdef __cplusplus

#define SIMD_REINTERPRET_CAST(func, type_out, type_in) \
__forceinline type_out func(type_in a) { \
    return *reinterpret_cast<type_out *>(&a); } \
__forceinline type_out type_out ## _from_ ## type_in (type_in a) { \
    return *reinterpret_cast<type_out *>(&a); }

#else

#define SIMD_REINTERPRET_CAST(func, type_out, type_in) \
__forceinline type_out func(type_in a) { \
    return *(type_out *)(&a); } \
__forceinline type_out type_out ## _from_ ## type_in (type_in a) { \
    return *(type_out *)(&a); }

#endif

//
// Faster 128-bit SSE soft intrinsics (replacing existing softintrin.h implementations)
//

#undef _mm_cvtss_f32

__forceinline
float _mm_cvtss_f32(__m128 a)
{
    return a.m128_f32[0];
}


#undef _mm_castpd_ps
#undef _mm_castps_pd
#undef _mm_castpd_si128
#undef _mm_castps_si128
#undef _mm_castsi128_pd
#undef _mm_castsi128_ps

//                    conversion-func         type_out  type_in

SIMD_REINTERPRET_CAST(_mm_castpd_ps,          __m128,   __m128d)
SIMD_REINTERPRET_CAST(_mm_castps_pd,          __m128d,  __m128)
SIMD_REINTERPRET_CAST(_mm_castpd_si128,       __m128i,  __m128d)
SIMD_REINTERPRET_CAST(_mm_castps_si128,       __m128i,  __m128)
SIMD_REINTERPRET_CAST(_mm_castsi128_pd,       __m128d,  __m128i)
SIMD_REINTERPRET_CAST(_mm_castsi128_ps,       __m128,   __m128i)

SIMD_REINTERPRET_CAST(_nn128_castn128_si128,  __m128i,  __n128)
SIMD_REINTERPRET_CAST(_nn128_castn128_pd,     __m128d,  __n128)
SIMD_REINTERPRET_CAST(_nn128_castn128_ps,     __m128,   __n128)

SIMD_REINTERPRET_CAST(_nn128_castsi128_n128,  __n128,   __m128i)
SIMD_REINTERPRET_CAST(_nn128_castpd_n128,     __n128,   __m128d)
SIMD_REINTERPRET_CAST(_nn128_castps_n128,     __n128,   __m128)

#undef _mm_set_epi32
#undef _mm_set_epix64
#undef _mm_set_pd
#undef _mm_set_ps

__forceinline
__m128i _mm_set_epi32(int e3, int e2, int e1, int e0)
{
    __m128i T;

    T.m128i_i32[0] = e0;
    T.m128i_i32[1] = e1;
    T.m128i_i32[2] = e2;
    T.m128i_i32[3] = e3;

    return T;
}

__forceinline
__m128i _mm_set_epix64(__int64 e1, __int64 e0)
{
    __m128i T;

    T.m128i_i64[0] = e0;
    T.m128i_i64[1] = e1;

    return T;
}

__forceinline
__m128 _mm_set_ps(float _F3, float _F2, float _F1, float _F0)
{
    __m128 T = { _F0, _F1, _F2, _F3 };

//  T.m128_f32[0] = _F0;
//  T.m128_f32[1] = _F1;
//  T.m128_f32[2] = _F2;
//  T.m128_f32[3] = _F3;

    return T;
}

__forceinline
__m128d _mm_set_pd(double _D1, double _D0)
{
    __m128d T = { _D0, _D1 };

//  T.m128d_f64[0] = _D0;
//  T.m128d_f64[1] = _D1;

    return T;
}

#undef _mm_setzero_epi32
#undef _mm_setzero_epix64
#undef _mm_setzero_ps
#undef _mm_setzero_pd

__forceinline
__m128i _mm_setzero_epi32(void)
{
    return _mm_set_epi32(0, 0, 0, 0);
}

__forceinline
__m128i _mm_setzero_epix64(void)
{
    return _mm_set_epix64(0, 0);
}

__forceinline
__m128 _mm_setzero_ps(void)
{
    return _mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f);
}

__forceinline
__m128d _mm_setzero_pd(void)
{
    return _mm_set_pd(0.0, 0.0);
}

#undef _mm_set1_epi32
#undef _mm_set1_epix64
#undef _mm_set1_ps
#undef _mm_set_ps1
#undef _mm_set1_pd
#undef _mm_set_pd1

__forceinline
__m128i _mm_set1_epi32(const int a)
{
//  return _nn128_castn128_si128(neon_dupqr32(a));
    return _mm_set_epi32(a, a, a, a);
}

__forceinline
__m128i _mm_set1_epix64(const int a)
{
    return _mm_set_epix64(a, a);
}

__forceinline
__m128 _mm_set1_ps(const float a)
{
    return _nn128_castn128_ps(neon_dupqrf32(a));
//  return _mm_set_ps(a, a, a, a);
}

__forceinline
__m128d _mm_set1_pd(double _D0)
{
    return _mm_set_pd(_D0, _D0);
}

#define _mm_set_ps1 _mm_set1_ps
#define _mm_set_pd1 _mm_set1_pd

//
// Flags to indicate special handling for certain operations
//

typedef enum  INTRIN_FLAGS
{
    _IF_NONE = 0,
    _IF_SQRT_F32 = (1 << 0),
    _IF_SQRT_F64 = (1 << 1),
    _IF_SCALAR_INSERT_F32 = (1 << 2),
    _IF_SCALAR_INSERT_F64 = (1 << 3),
} INTRIN_FLAGS;

__forceinline
__n128 _nn_postprocess(__n128 T, __n128 a, const __n128 b, int flags)
{
    if (flags & _IF_SQRT_F32)
    {
        const __n128 SignMask = neon_dupqr32(0x80000000);
        T = neon_bitq(T, a, SignMask);
    }

    if (flags & _IF_SQRT_F64)
    {
        const __n128 SignMask = neon_dupqr64(0x8000000000000000ull);
        T = neon_bitq(T, a, SignMask);
    }

    if (flags & _IF_SCALAR_INSERT_F32)
    {
        // scalar 32-bit SS/SI operations where result is inserted into low float/dword lane

        T = neon_insqe32q(a, 0, T, 0);
    }

    if (flags & _IF_SCALAR_INSERT_F64)
    {
        // scalar 64-bit SD/SQ operations where result is inserted into low double/qword lane

        T = neon_insqe64q(a, 0, T, 0);
    }

    return T;
}

//
// Template for common 128-bit dest,source1,source2 vector instructions
//

#define DEFINE_N128_OP_N128_N128(rettype, name, intrin, arg1type, arg1, arg2type, arg2, flags ) \
\
__forceinline __n128 _nn_ ## name (__n128 a, const __n128 b) \
{ \
    __n128 T = intrin (a, b); \
    T = _nn_postprocess(T, a, b, flags); \
    return T; \
} \
\
__forceinline rettype _mm_ ## name (arg1type arg1, arg2type arg2) \
{ \
    return rettype ## _from___n128 ( _nn_ ## name ( __n128_from_ ## arg1type (a), __n128_from_ ## arg2type (b) ) ); \
}

// PADD PSUB PMUL PDIV

#undef _mm_add_epi8
#undef _mm_add_epi16
#undef _mm_add_epi32
#undef _mm_add_epi64

DEFINE_N128_OP_N128_N128(__m128i, add_epi8,     neon_addq8,     __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, add_epi16,    neon_addq16,    __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, add_epi32,    neon_addq32,    __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, add_epi64,    neon_addq64,    __m128i, a, __m128i, b, 0)

#undef _mm_sub_epi8
#undef _mm_sub_epi16
#undef _mm_sub_epi32
#undef _mm_sub_epi64

DEFINE_N128_OP_N128_N128(__m128i, sub_epi8,     neon_subq8,     __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, sub_epi16,    neon_subq16,    __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, sub_epi32,    neon_subq32,    __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, sub_epi64,    neon_subq64,    __m128i, a, __m128i, b, 0)

#undef _mm_mul_epi8
#undef _mm_mul_epi16
#undef _mm_mul_epi32
// #undef _mm_mul_epi64

DEFINE_N128_OP_N128_N128(__m128i, mul_epi8,     neon_mulq8,     __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, mul_epi16,    neon_mulq16,    __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, mul_epi32,    neon_mulq32,    __m128i, a, __m128i, b, 0)
// DEFINE_N128_OP_N128_N128(__m128i, mul_epi64,    neon_mulq64,    __m128i, a, __m128i, b, 0)

#if 0
#undef _mm_div_epi8
#undef _mm_div_epi16
#undef _mm_div_epi32
#undef _mm_div_epi64

DEFINE_N128_OP_N128_N128(__m128i, div_epi8,     neon_divq8,     __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, div_epi16,    neon_divq16,    __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, div_epi32,    neon_divq32,    __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, div_epi64,    neon_divq64,    __m128i, a, __m128i, b, 0)
#endif

// PAND PANDN POR PXOR

#undef _mm_and_si128
#undef _mm_andnot_si128
#undef _mm_or_si128
#undef _mm_xor_si128

DEFINE_N128_OP_N128_N128(__m128i, and_si128,    neon_andq,      __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, andnot_si128, neon_bicq,      __m128i, b, __m128i, a, 0) // swap arg order
DEFINE_N128_OP_N128_N128(__m128i,  or_si128,    neon_orrq,      __m128i, a, __m128i, b, 0)
DEFINE_N128_OP_N128_N128(__m128i, xor_si128,    neon_eorq,      __m128i, a, __m128i, b, 0)

// MINPD MAXPD
// MINPS MAXPS

#undef _mm_min_pd
#undef _mm_max_pd
#undef _mm_min_ps
#undef _mm_max_ps

DEFINE_N128_OP_N128_N128(__m128d, min_pd,       neon_fminq64,   __m128d, a, __m128d, b, _IF_SCALAR_INSERT_F32)
DEFINE_N128_OP_N128_N128(__m128d, max_pd,       neon_fmaxq64,   __m128d, a, __m128d, b, _IF_SCALAR_INSERT_F32)
DEFINE_N128_OP_N128_N128(__m128 , min_ps,       neon_fminq32,   __m128 , a, __m128 , b, _IF_SCALAR_INSERT_F32)
DEFINE_N128_OP_N128_N128(__m128 , max_ps,       neon_fmaxq32,   __m128 , a, __m128 , b, _IF_SCALAR_INSERT_F32)

// ADDPD SUBPD MULPD DIVPD

#undef _mm_add_pd
#undef _mm_sub_pd
#undef _mm_mul_pd
#undef _mm_div_pd

DEFINE_N128_OP_N128_N128(__m128d, add_pd,       neon_faddq64,   __m128d, a, __m128d, b, _IF_SCALAR_INSERT_F64)
DEFINE_N128_OP_N128_N128(__m128d, sub_pd,       neon_fsubq64,   __m128d, a, __m128d, b, _IF_SCALAR_INSERT_F64)
DEFINE_N128_OP_N128_N128(__m128d, mul_pd,       neon_fmulq64,   __m128d, a, __m128d, b, _IF_SCALAR_INSERT_F64)
DEFINE_N128_OP_N128_N128(__m128d, div_pd,       neon_fdivq64,   __m128d, a, __m128d, b, _IF_SCALAR_INSERT_F64)

// ADDSS SUBSS MULSS DIVSS

#undef _mm_add_ss
#undef _mm_sub_ss
#undef _mm_mul_ss
#undef _mm_div_ss

DEFINE_N128_OP_N128_N128(__m128 , add_ss,       neon_faddq32,   __m128 , a, __m128 , b, 0)
DEFINE_N128_OP_N128_N128(__m128 , sub_ss,       neon_fsubq32,   __m128 , a, __m128 , b, 0)
DEFINE_N128_OP_N128_N128(__m128 , mul_ss,       neon_fmulq32,   __m128 , a, __m128 , b, 0)
DEFINE_N128_OP_N128_N128(__m128 , div_ss,       neon_fdivq32,   __m128 , a, __m128 , b, 0)

// ADDSD SUBSD MULSD DIVSD

#undef _mm_add_sd
#undef _mm_sub_sd
#undef _mm_mul_sd
#undef _mm_div_sd

DEFINE_N128_OP_N128_N128(__m128d, add_sd,       neon_faddq64,   __m128d, a, __m128d, b, 0)
DEFINE_N128_OP_N128_N128(__m128d, sub_sd,       neon_fsubq64,   __m128d, a, __m128d, b, 0)
DEFINE_N128_OP_N128_N128(__m128d, mul_sd,       neon_fmulq64,   __m128d, a, __m128d, b, 0)
DEFINE_N128_OP_N128_N128(__m128d, div_sd,       neon_fdivq64,   __m128d, a, __m128d, b, 0)

// ADDPS SUBPS MULPS DIVPS

#undef _mm_add_ps
#undef _mm_sub_ps
#undef _mm_mul_ps
#undef _mm_div_ps

DEFINE_N128_OP_N128_N128(__m128 , add_ps,       neon_faddq32,   __m128 , a, __m128 , b, 0)
DEFINE_N128_OP_N128_N128(__m128 , sub_ps,       neon_fsubq32,   __m128 , a, __m128 , b, 0)
DEFINE_N128_OP_N128_N128(__m128 , mul_ps,       neon_fmulq32,   __m128 , a, __m128 , b, 0)
DEFINE_N128_OP_N128_N128(__m128 , div_ps,       neon_fdivq32,   __m128 , a, __m128 , b, 0)


// SQRTPD SQRTSD
// SQRTPS SQRTSS

#undef _mm_sqrt_pd
#undef _mm_sqrt_ps
#undef _mm_sqrt_sd
#undef _mm_sqrt_ss

__forceinline
__n128 _nn_sqrt_pd(__n128 a)
{
    __n128 T = neon_fsqrtq64(a);
    T = _nn_postprocess(T, a, a, _IF_SQRT_F64);

    return T;
}

__forceinline
__n128 _nn_sqrt_ps(__n128 a)
{
    __n128 T = neon_fsqrtq32(a);
    T = _nn_postprocess(T, a, a, _IF_SQRT_F32);

    return T;
}

__forceinline
__m128d _mm_sqrt_pd(__m128d a)
{
    return _nn128_castn128_pd( _nn_sqrt_pd(_nn128_castpd_n128(a)) );
}

__forceinline
__m128 _mm_sqrt_ps(__m128 a)
{
    return _nn128_castn128_ps( _nn_sqrt_ps(_nn128_castps_n128(a)) );
}

__forceinline
__n128 _nn_sqrt_sd(__n128 a, __n128 b) // not like the others!
{
    // SQRT(b) gets merged in to lower lane of a

    __n128 T = neon_fsqrtq64(b);
    T = _nn_postprocess(T, b, b, _IF_SQRT_F64); \

    return T;
}

__forceinline
__n128 _nn_sqrt_ss(__n128 a)
{
    // SQRT(a) gets merged in to lower lane of a

    __n128 T = neon_fsqrtq32(a);
    T = _nn_postprocess(T, a, a, _IF_SQRT_F32); \

    return T;
}

__forceinline
__m128d _mm_sqrt_sd(__m128d a, __m128d b)
{
    return _nn128_castn128_pd( _nn_sqrt_sd(_nn128_castpd_n128(a),  _nn128_castpd_n128(b)) );
}

__forceinline
__m128 _mm_sqrt_ss(__m128 a)
{
    return _nn128_castn128_ps( _nn_sqrt_ss(_nn128_castps_n128(a)) );
}

// PSHUFD

#undef _mm_shuffle_epi32

__forceinline
__m128i _mm_shuffle_epi32(__m128i a, const int imm8)
{
    __m128i T;

    // check for common cases of broadcasting a single lane

    if (imm8 == 0x00)
        return _mm_set1_epi32(a.m128i_i32[0]);

    if (imm8 == 0x55)
        return _mm_set1_epi32(a.m128i_i32[1]);

    if (imm8 == 0xAA)
        return _mm_set1_epi32(a.m128i_i32[2]);

    if (imm8 == 0xFF)
        return _mm_set1_epi32(a.m128i_i32[3]);

    // all other cases slow shuffle

    T.m128i_u32[0] = a.m128i_u32[(imm8 >> 0) & 0x03];
    T.m128i_u32[1] = a.m128i_u32[(imm8 >> 2) & 0x03];
    T.m128i_u32[2] = a.m128i_u32[(imm8 >> 4) & 0x03];
    T.m128i_u32[3] = a.m128i_u32[(imm8 >> 6) & 0x03];

    return T;
}

// SHUFPS

#undef _mm_shuffle_ps

__forceinline
__n128 _nn128_shuffle_ps(__n128 a, const __n128 b, const unsigned int imm8)
{
    __n128 T;

    T.n128_f32[0] = a.n128_f32[(imm8 >> 0) & 0x03];
    T.n128_f32[1] = a.n128_f32[(imm8 >> 2) & 0x03];
    T.n128_f32[2] = b.n128_f32[(imm8 >> 4) & 0x03];
    T.n128_f32[3] = b.n128_f32[(imm8 >> 6) & 0x03];

    return T;
}

__forceinline
__m128 _mm_shuffle_ps(__m128 a, __m128 b, const unsigned int imm8)
{
//  the NEON path is slower due to memory spill for the casts :-(
//  return _nn128_castn128_ps( _nn128_shuffle_ps(_nn128_castps_n128(a), _nn128_castps_n128(b), imm8) );

    __m128 T;

    // all other cases slow shuffle

    T.m128_f32[0] = a.m128_f32[(imm8 >> 0) & 0x03];
    T.m128_f32[1] = a.m128_f32[(imm8 >> 2) & 0x03];
    T.m128_f32[2] = b.m128_f32[(imm8 >> 4) & 0x03];
    T.m128_f32[3] = b.m128_f32[(imm8 >> 6) & 0x03];

    return T;
}

// CMPPS

#undef _mm_cmp_ps

__forceinline
__n128 _nn128_cmpord_ps(__n128 a, __n128 b)
{
    __n128 T1 = neon_fcmeqq32(a, a);
    __n128 T2 = neon_fcmeqq32(b, b);
    __n128 T3 = neon_andq(T1, T2);

    return T3;
}

__forceinline
__n128 _nn128_cmp_ps(__n128 a, __n128 b, const int imm8)
{
    __n128 T;

    switch (imm8 & 0x1F)
    {
        case _CMP_EQ_OQ:     T = neon_fcmeqq32(a, b); break;
        case _CMP_LT_OS:     T = neon_fcmgtq32(b, a); break;  // LT is GT swapped
        case _CMP_LE_OS:     T = neon_fcmgeq32(b, a); break;  // LE is GE swapped
        case _CMP_UNORD_Q:   T = neon_notq(_nn128_cmpord_ps(a, b)); break;
        case _CMP_NEQ_UQ:    T = neon_notq(neon_fcmeqq32(a, b)); break;
        case _CMP_NLT_US:    T = neon_notq(neon_fcmgtq32(b, a)); break;
        case _CMP_NLE_US:    T = neon_notq(neon_fcmgeq32(b, a)); break;
        case _CMP_ORD_Q:     T = _nn128_cmpord_ps(a, b); break;

        case _CMP_EQ_UQ:     T = neon_fcmeqq32(a, b); break;
        case _CMP_NGE_US:    T = neon_notq(neon_fcmgeq32(a, b)); break;
        case _CMP_NGT_US:    T = neon_notq(neon_fcmgtq32(a, b)); break;
        case _CMP_FALSE_OQ:  T = neon_moviqw(0); break;
        case _CMP_NEQ_OQ:    T = neon_notq(neon_fcmeqq32(b, a)); break;
        case _CMP_GE_OS:     T = neon_fcmgeq32(a, b); break;
        case _CMP_GT_OS:     T = neon_fcmgtq32(a, b); break;
        case _CMP_TRUE_UQ:   T = neon_mvniqw(0); break;

        case _CMP_EQ_OS:     T = neon_fcmeqq32(a, b); break;
        case _CMP_LT_OQ:     T = neon_fcmgtq32(b, a); break;
        case _CMP_LE_OQ:     T = neon_fcmgeq32(b, a); break;
        case _CMP_UNORD_S:   T = neon_notq(_nn128_cmpord_ps(a, b)); break;
        case _CMP_NEQ_US:    T = neon_notq(neon_fcmeqq32(a, b)); break;
        case _CMP_NLT_UQ:    T = neon_notq(neon_fcmgtq32(b, a)); break;
        case _CMP_NLE_UQ:    T = neon_notq(neon_fcmgeq32(b, a)); break;
        case _CMP_ORD_S:     T = _nn128_cmpord_ps(a, b); break;

        case _CMP_EQ_US:     T = neon_fcmeqq32(a, b); break;
        case _CMP_NGE_UQ:    T = neon_notq(neon_fcmgeq32(a, b)); break;
        case _CMP_NGT_UQ:    T = neon_notq(neon_fcmgtq32(a, b)); break;
        case _CMP_FALSE_OS:  T = neon_moviqw(0); break;
        case _CMP_NEQ_OS:    T = neon_notq(neon_fcmeqq32(b, a)); break;
        case _CMP_GE_OQ:     T = neon_fcmgeq32(a, b); break;
        case _CMP_GT_OQ:     T = neon_fcmgtq32(a, b); break;
        case _CMP_TRUE_US:   T = neon_mvniqw(0); break;
            break;

        default:
            __assume(0);
            break;
    }

    return T;
}

__forceinline
__m128 _mm_cmp_ps(__m128 a, __m128 b, const int imm8)
{
    __m128 T = _nn128_castn128_ps( _nn128_cmp_ps(_nn128_castps_n128(a), _nn128_castps_n128(b), imm8 & 0x1F) );

    return T;
}


// PBLENDVB

#undef _mm_blendv_epi8

__forceinline
__m128i _mm_blendv_epi8(__m128i a, __m128i b, __m128i mask)
{
    __m128i T = _nn128_castn128_si128(neon_bitq(_nn128_castsi128_n128(a), _nn128_castsi128_n128(b), neon_sshriq8 (_nn128_castsi128_n128(mask), 7)));

    return T;
}


#undef _mm_blendv_ps

__forceinline
__m128 _mm_blendv_ps(__m128 a, __m128 b, __m128 mask)
{
    __m128 T = _nn128_castn128_ps(neon_bitq(_nn128_castps_n128(a), _nn128_castps_n128(b), neon_sshriq32 (_nn128_castps_n128(mask), 31)));

    return T;
}


#undef _mm_movemask_ps

__forceinline
int _mm_movemask_ps(__m128 a)
{
    __m128i T = _mm_castps_si128(a);

    int mask = 0;

    mask |= (T.m128i_u32[3] >> 31) << 3;
    mask |= (T.m128i_u32[2] >> 31) << 2;
    mask |= (T.m128i_u32[1] >> 31) << 1;
    mask |= (T.m128i_u32[0] >> 31) << 0;

    return mask;
}



//
// New 256-bit AVX2 soft intrinsics (not provided in softintrin.h)
//
// naming conventions as documented at https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
//

//                    conversion-func         type_out  type_in

SIMD_REINTERPRET_CAST(_mm256_castsi256_pd,    __m256d,  __m256i)
SIMD_REINTERPRET_CAST(_mm256_castsi256_ps,    __m256,   __m256i)
SIMD_REINTERPRET_CAST(_mm256_castpd_si256,    __m256i,  __m256d)
SIMD_REINTERPRET_CAST(_mm256_castps_si256,    __m256i,  __m256)

SIMD_REINTERPRET_CAST(_mm256_castpd256_pd128, __m128d,  __m256d)
SIMD_REINTERPRET_CAST(_mm256_castps256_ps128, __m128,   __m256)

SIMD_REINTERPRET_CAST(_nn256_castn256_pd,     __m256d,  __n128x2)
SIMD_REINTERPRET_CAST(_nn256_castn256_ps,     __m256,   __n128x2)
SIMD_REINTERPRET_CAST(_nn256_castn256_si256,  __m256i,  __n128x2)

SIMD_REINTERPRET_CAST(_nn256_castpd_n256,     __n128x2, __m256d)
SIMD_REINTERPRET_CAST(_nn256_castps_n256,     __n128x2, __m256)
SIMD_REINTERPRET_CAST(_nn256_castsi256_n256,  __n128x2, __m256i)

__forceinline
__m256d _mm256_set_pd(double _D3, double _D2, double _D1, double _D0)
{
    __m256d T;

    T.m256d_f64[0] = _D0;
    T.m256d_f64[1] = _D1;
    T.m256d_f64[2] = _D2;
    T.m256d_f64[3] = _D3;

    return T;
}

__forceinline
__m256d _mm256_setzero_pd(void)
{
    return _mm256_set_pd(0.0, 0.0, 0.0, 0.0);
}

__forceinline
__m256d _mm256_set1_pd(double _D0)
{
    return _mm256_set_pd(_D0, _D0, _D0, _D0);
}

__forceinline
__m256 _mm256_set_ps(float _F7, float _F6, float _F5, float _F4, float _F3, float _F2, float _F1, float _F0)
{
    __m256 T;

    T.m256_f32[0] = _F0;
    T.m256_f32[1] = _F1;
    T.m256_f32[2] = _F2;
    T.m256_f32[3] = _F3;
    T.m256_f32[4] = _F4;
    T.m256_f32[5] = _F5;
    T.m256_f32[6] = _F6;
    T.m256_f32[7] = _F7;

    return T;
}

__forceinline
__m256 _mm256_setzero_ps(void)
{
    return _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

__forceinline
__m256 _mm256_set1_ps(float _F0)
{
    return _mm256_set_ps(_F0, _F0, _F0, _F0, _F0, _F0, _F0, _F0);
}

#define DEFINE_N256_OP_N256_N256(rettype, name, intrin, arg1type, arg1, arg2type, arg2, flags ) \
\
__forceinline __n128x2 _nn256_ ## name (__n128x2 a, const __n128x2 b) \
{ \
    __n128x2 T; \
    T.val[0] = intrin (a.val[0], b.val[0]); \
    T.val[0] = _nn_postprocess(T.val[0], a.val[0], b.val[0], flags); \
    T.val[1] = intrin (a.val[1], b.val[1]); \
    T.val[1] = _nn_postprocess(T.val[1], a.val[1], b.val[1], flags); \
    return T; \
} \
\
__forceinline rettype _mm256_ ## name (arg1type arg1, arg2type arg2) \
{ \
    return rettype ## _from___n128x2 ( _nn256_ ## name ( __n128x2_from_ ## arg1type (a), __n128x2_from_ ## arg2type (b) ) ); \
}

// VPAND  VPANDN  VPOR  VPXOR

DEFINE_N256_OP_N256_N256(__m256i, and_si256,    neon_andq,      __m256i, a, __m256i, b, 0)
DEFINE_N256_OP_N256_N256(__m256i, andnot_si256, neon_bicq,      __m256i, b, __m256i, a, 0) // swap arg order
DEFINE_N256_OP_N256_N256(__m256i, or_si256,     neon_orrq,      __m256i, a, __m256i, b, 0)
DEFINE_N256_OP_N256_N256(__m256i, xor_si256,    neon_eorq,      __m256i, a, __m256i, b, 0)

// VANDPP VORPD VXORPD

DEFINE_N256_OP_N256_N256(__m256d, and_pd,       vandq_s64,      __m256d, a, __m256d, b, 0)
DEFINE_N256_OP_N256_N256(__m256d, andnot_pd,    vbicq_s64,      __m256d, b, __m256d, a, 0) // swap arg order
DEFINE_N256_OP_N256_N256(__m256d, or_pd,        vorrq_s64,      __m256d, a, __m256d, b, 0)
DEFINE_N256_OP_N256_N256(__m256d, xor_pd,       veorq_s64,      __m256d, a, __m256d, b, 0)

// VANDPS VORPS VXORPS

DEFINE_N256_OP_N256_N256(__m256 , and_ps,       vandq_s32,      __m256 , a, __m256 , b, 0)
DEFINE_N256_OP_N256_N256(__m256 , andnot_ps,    vbicq_s32,      __m256 , b, __m256 , a, 0) // swap arg order
DEFINE_N256_OP_N256_N256(__m256 , or_ps,        vorrq_s32,      __m256 , a, __m256 , b, 0)
DEFINE_N256_OP_N256_N256(__m256 , xor_ps,       veorq_s32,      __m256 , a, __m256 , b, 0)

// VADDPD VSUBPD VMULPD VDIVPD

DEFINE_N256_OP_N256_N256(__m256d, add_pd,       neon_faddq64,   __m256d, a, __m256d, b, 0)
DEFINE_N256_OP_N256_N256(__m256d, sub_pd,       neon_fsubq64,   __m256d, a, __m256d, b, 0)
DEFINE_N256_OP_N256_N256(__m256d, mul_pd,       neon_fmulq64,   __m256d, a, __m256d, b, 0)
DEFINE_N256_OP_N256_N256(__m256d, div_pd,       neon_fdivq64,   __m256d, a, __m256d, b, 0)

// VADDPD VSUBPD VMULPD VDIVPD

DEFINE_N256_OP_N256_N256(__m256 , add_ps,       neon_faddq32,   __m256 , a, __m256 , b, 0)
DEFINE_N256_OP_N256_N256(__m256 , sub_ps,       neon_fsubq32,   __m256 , a, __m256 , b, 0)
DEFINE_N256_OP_N256_N256(__m256 , mul_ps,       neon_fmulq32,   __m256 , a, __m256 , b, 0)
DEFINE_N256_OP_N256_N256(__m256 , div_ps,       neon_fdivq32,   __m256 , a, __m256 , b, 0)

// VMIN VMAX

DEFINE_N256_OP_N256_N256(__m256d, min_pd,       neon_fminq64,   __m256d, a, __m256d, b, 0)
DEFINE_N256_OP_N256_N256(__m256d, max_pd,       neon_fmaxq64,   __m256d, a, __m256d, b, 0)

DEFINE_N256_OP_N256_N256(__m256 , min_ps,       neon_fminq32,   __m256 , a, __m256 , b, 0)
DEFINE_N256_OP_N256_N256(__m256 , max_ps,       neon_fmaxq32,   __m256 , a, __m256 , b, 0)

// VSQRTPD VSQRTPS

__forceinline
__n128x2 _nn256_sqrt_pd(__n128x2 a)
{
    __n128x2 T;

    T.val[0] = neon_fsqrtq64(a.val[0]);
    T.val[1] = neon_fsqrtq64(a.val[1]);

    T.val[0] = _nn_postprocess(T.val[0], a.val[0], a.val[0], _IF_SQRT_F64);
    T.val[1] = _nn_postprocess(T.val[1], a.val[1], a.val[1], _IF_SQRT_F64);

    return T;
}

__forceinline
__n128x2 _nn256_sqrt_ps(__n128x2 a)
{
    __n128x2 T;

    T.val[0] = neon_fsqrtq32(a.val[0]);
    T.val[1] = neon_fsqrtq32(a.val[1]);

    T.val[0] = _nn_postprocess(T.val[0], a.val[0], a.val[0], _IF_SQRT_F32);
    T.val[1] = _nn_postprocess(T.val[1], a.val[1], a.val[1], _IF_SQRT_F32);

    return T;
}

__forceinline
__m256d _mm256_sqrt_pd(__m256d a)
{
    return _nn256_castn256_pd( _nn256_sqrt_pd(_nn256_castpd_n256(a)) );
}

__forceinline
__m256 _mm256_sqrt_ps(__m256 a)
{
    return _nn256_castn256_ps( _nn256_sqrt_ps(_nn256_castps_n256(a)) );
}

// VEXTRACTF128

__forceinline
__m128d _mm256_extractf128_pd(__m256d a, const int imm8)
{
    __m128d T = *(__m128d *)&a.m256d_f64[(imm8 & 1) << 1];
    return T;
}

__forceinline
__m128 _mm256_extractf128_ps(__m256 a, const int imm8)
{
    __m128 T = *(__m128 *)&a.m256_f32[(imm8 & 1) << 2];
    return T;
}

// VPERMILPD VPERMILPS

__forceinline
__m256d _mm256_permute_pd(__m256d a, const int imm8)
{
    __m256d T =
    {
        a.m256d_f64[0 + ((imm8 >> 0) & 1)],
        a.m256d_f64[0 + ((imm8 >> 1) & 1)],
        a.m256d_f64[2 + ((imm8 >> 2) & 1)],
        a.m256d_f64[2 + ((imm8 >> 3) & 1)],
    };

    return T;
}

__forceinline
__m256 _mm256_permute_ps(__m256 a, const int imm8)
{
    __m256 T =
    {
        a.m256_f32[0 + ((imm8 >> 0) & 3)],
        a.m256_f32[0 + ((imm8 >> 2) & 3)],
        a.m256_f32[0 + ((imm8 >> 4) & 3)],
        a.m256_f32[0 + ((imm8 >> 6) & 3)],
        a.m256_f32[4 + ((imm8 >> 0) & 3)],
        a.m256_f32[4 + ((imm8 >> 2) & 3)],
        a.m256_f32[4 + ((imm8 >> 4) & 3)],
        a.m256_f32[4 + ((imm8 >> 6) & 3)],
    };

    return T;
}

// VMOVSHDUP VMOVSLDUP

__forceinline
__m256 _mm256_moveldup_ps(__m256 a)
{
    return _mm256_permute_ps(a, 0xA0); // A0 = 10'10'00'00
}

__forceinline
__m256 _mm256_movehdup_ps(__m256 a)
{
    return _mm256_permute_ps(a, 0xF5); // F5 = 11'11'01'01
}

#endif  // ARM64/ARM64EC

#endif  // SOFT_INTRINSICS_AVX2

