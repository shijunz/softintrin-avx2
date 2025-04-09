//
// USE_SOFT_INTRINSICS.H
//
// Enable SSE and AVX soft intrinsics for native ARM64 and
// work around various ARM64EC compiler bugs that break porting
// AVX/AVX2 intrinsics from existing x86 x64 codebases.
//
// - works around Visual C/C++ ARM64EC compiler bug with __vectorcall
//
// - works around Visual C/C++ ARM64EC compiler bug with __m256
//
// - enables command line and IDE builds to native ARM64 (i.e. non-ARM64EC)
//
// - enables enhanced soft intrinsics for SSE and AVX/AVX2
//
// Native ARM64 builds support soft intrinsics on older discontinued
// Snapdragon 835 and Windows 10 on ARM devices, e.g. Lenovo Miix, HP Envy X2
//
// ARM64EC builds require Windows 11 on ARM and newer devices (Surface Pro X or later)
//
// It is recommended placing this header file and SOFTINTRIN_AVX2.H
// next to the existing SOFTINTRIN.H header in the SDK, which by default
// will usually be found at this directory on your boot disk:
//
// c:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\
//

#if defined _M_ARM64EC || defined _M_ARM64

// This check is to prevent re-entrancy and works with existing projects
// which have defined the USE_SOFT_INTRINSICS macro for native ARM64 builds

#if !defined USE_SOFT_INTRINSICS || (USE_SOFT_INTRINSICS < 2)

// TODO: enable the correct version check once the MSVC compiler fixes the two bugs

#if 1 || (_MSC_FULL_VER <= 194434918)

// Not a full workaround for MSVC compiler bug as it will not generate correct thunks
// but as per this bug report this will work around the compiler error with __vectorcall
// https://developercommunity.visualstudio.com/t/VC-176-preview-1-x86-compiler-bad-cod/10291481

#define __vectorcall

// workaround to ARM64EC MSVC compiler _m256 restriction which prevents DVEC.h from compiling
// https://developercommunity.visualstudio.com/t/Arm64EC-Code-fails-to-compile/10838593

#undef  USE_SOFT_INTRINSICS
#define _DISABLE_SOFTINTRIN_

#include <intrin.h>  // this will pull in everything but immintrin.h ammintrin.h and softintrin.h

#undef _DISABLE_SOFTINTRIN_
#define USE_SOFT_INTRINSICS 2

#define intrin_type  noalias

#include <immintrin.h>  // this pulls in AVX intrinsics with the _m256 compiler error neutered
#include <ammintrin.h>

#undef intrin_type

#endif // needed workaround

#include <softintrin.h>       // now pull in legacy softintrin.h as usual
#include <softintrin_avx2.h>  // append with enhanced AVX2 and optimized SSE intrinsics

// make sure NTDLL.LIB is linked (for some Rtl functions) and SOFTINTRIN.LIB for command line builds
// these can also be specified in the Visual Studio .vcxproj file for IDE based builds

#pragma comment(linker, "/defaultlib:ntdll")
#pragma comment(linker, "/defaultlib:softintrin")

#endif  // USE_SOFT_INTRINSICS
#endif  // ARM64/ARM64EC


