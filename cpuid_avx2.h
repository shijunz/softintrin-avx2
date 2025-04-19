//
// CPUID_AVX2.H
//
// Experimental __cpuid() / __cpuidex() override for ARM64EC builds.
//
// #include this code _after_ any #include <windows.h> or <intrin.h>
//

#if _M_ARM64EC

//
// Override __cpuid() and __cpuidex() to return AVX and AVX2 feature bits enabled.
// WARNING: use this only to guard C/C++ code using AVX/AVX2 intrinsics. Do not use
// to guard against function calls to ASM code that contains AVX or AVX2 opcodes.
// For the ASM case either execute a CPUID check in the ASM code itself, or perform
// the check in a source file that is not using these new intrinsic unless you are
// running on a recent Windows Insider Canary build (278xx) where AVX2 support exists.
//

#undef __cpuid
#undef __cpuidex

__forceinline
void __cpuidex(int cpuInfo[4], int function_id, int subfunction_id)
{
    typedef enum CPUID_REGS
    {
        CPUID_EAX = 0,
        CPUID_EBX = 1,
        CPUID_ECX = 2,
        CPUID_EDX = 3,
    } CPUID_REGS;

    // call the SDK's __cpuidex() wrapper first (see softintrin.h)

    CPUIDEX64(cpuInfo, function_id, subfunction_id);

    // then overlay AVX and AVX2 capabilities

    if (function_id == 1)
    {
        cpuInfo[CPUID_ECX] |= (1 << 28);    // AVX capability bit
    }

    if (function_id == 7)
    {
        cpuInfo[CPUID_EBX] |= (1 << 5);     // AVX2 capability bit
    }

    // TODO: overlay other capability bits as intrinsics are implemented
    // e.g. RDRAND

}

__forceinline
void __cpuid(int cpuInfo[4], int function_id)
{
    __cpuidex(cpuInfo, function_id, 0);
}

#endif // _M_ARM64EC

