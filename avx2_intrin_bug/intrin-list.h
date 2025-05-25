#if defined(__AVX2__) || (defined(USE_SOFT_INTRINSICS) && (USE_SOFT_INTRINSICS >= 2))

DEFINE_TEST_OP_RAB (_mm256_sll_epi32,       __m256i,    __m256i,    __m128i)
DEFINE_TEST_OP_RAB (_mm256_srl_epi32,       __m256i,    __m256i,    __m128i)

#endif // AVX2 tests

