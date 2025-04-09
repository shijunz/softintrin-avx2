# softintrin-avx2
Fast SSE/AVX/AVX2 software intrinsics for use with VS2022 and Windows SDK for even easier porting to Windows on ARM!

Summary:

This project consists of 2 C/C++ header files which provide enhancements to the existing [ARM64EC](http://www.emulators.com/docs/abc_arm64ec_explained.htm) "soft intrinsics" provided in the Windows 24H2 SDK (build 10.0.26100) to provide this additional performance, functionality, and simplify porting Windows applications from Intel x86/x64 to ARM64EC, and even to native ARM64 for Windows on ARM:

- adds dozens (and eventually hundreds, this is a work-in-progress) of new 256-bit AVX2 soft intrinsics to support using Visual Studio's DVEC.H vector classes in ARM64/ARM64EC builds

- works around the "error C7302: AVX types (__m256) are not currently supported in ARM64EC code" compiler block in Visual Studio

- optimizes common SSE soft intrinsics for both ARM64 and ARM64EC builds which results in, for example, over 15x speedup for ARM64/ARM64EC builds of ToyPathTracer ray tracing demo, on par with emulation and alternate solutions such as sse2neon

- smaller binary size due to linking less static code from SOFTINTRIN.LIB library from the SDK, and then even additional binary size reductions when retargetting from ARM64EC to pure native ARM64

- native ARM64 binaries produced by this implementation are Windows 10 on ARM compatible and compatible with Snapdragon 835 based devices such as ASUS Novago, HP Envy X2, and Lenovo Miix (whereas ARM64EC has always required Windows 11 on ARM)


How to install:

Copy the two header files USE_SOFT_INTRIN.H and SOFTINTRIN_AVX2.H to a path in your project's INCLUDE path or to the directory where the existing SOFTINTRIN.H resides.  For example, if you have the latest official Windows SDK 10.0.26100 installed, that locations is likely:

    c:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\

Then, add this line of code to each source file in your project where there is either an #include <windows.h> or #include <intrin.h>.  This line should be placed right _before_ those existing includes, not after, in order to work around the compiler's _m256 block.

    #include <use_soft_intrinsics.h>

Optionally you can do an in-place replace of #include <intrin.h> with #include <use_soft_intrinsics.h>, this will not break existing x86/x64 builds.  You can skip source files that are not performance critical or do no contain any Intel-specific intrinsics since at worse they will simply use the existing soft intrinsics.  Again, this project is an overlay on top of the existing softintrin.h implementation, not a full replacement, so just layer it on in source files where it matters.

If your project is already building as ARM64EC, then nothing else is required other than to rebuild your project and experience faster performance.


How about a demo?!?!

Soft intrinsics make porting of existing Windows applications that contain SSE-specific intrinsics trivial, As a demo of ARM64EC and soft intrinsics, our [fork of ToyPathTracer](https://github.com/softmac/ToyPathTracer) demonstrates in a handful of additional commits (on top of their tip of tree) how to add an ARM64EC target to an existing x86/x64 Visual Studio project and shows the minor source code edits to make the code compile for ARM64EC.  The whole port from x64 to ARM64 only takes a few minutes and it is recommended to take a look at our fork to get an understanding of what porting to Windows on ARM entails.

Build all 4 Release targets - x86, x64, ARM64EC, and ARM64.  The x86 and x64 builds are exactly as-is as provided in the original [ToyPathTracer](https://github.com/aras-p/ToyPathTracer) project, the ARM64EC and ARM64 targets are what the fork branch adds.

On a Windows on ARM device - such as Surface Pro X, Surface Pro 9, Surface Pro 11 - run each of the 4 binaries produced (in theory you installed Visual Studio 2022 on your ARM device directly and can just press F5 on each).  What you should find, for example, on a Snapdragon X based Surface Pro 11, is that the 32-bit x86 binary renders at about 7 frames per second in CPU mode, the 64-bit x64 binary renders at about 9 frames per second in emulation (I'm testing on latest Windows Insider Canary build), and both the ARM64EC and ARM64 builds render at about... wait for it... 0.5 frames per second, or 2 seconds per frame.  That is the reference implementation of soft intrinsics in the SDK.

Thankfully, that reference implementation was specifically designed to be extensible, as most every soft intrinsic is implemented as a #define macro to re-map the intrinsic to a different function exported by SOFTINTRIN.LIB.  This is the key to how this new implementation layers on top, it simply uses an #undef to unmap each macro and replaces with a new faster implementation.

So go ahead, make 2 minor edits to the fork (or be brave and even just make the edits to the original ToyPathTracer branch), go to each of these three source files in that project and add the magic #include to the top of each of these files:

    Cpp/Windows/TestWin.cpp
    Cpp/Source/MathSimd.h

and do a full rebuild of each of the ARM64EC and ARM64 targets.  Presto, the new builds run at 8 frames per second, faster than the x86 emulation and just a bit slower than the x64 emulation, but a full 15x faster or more than the reference implementation in the SDK.



Implementation notes:

These header files extend the functionality of SOFTINTRIN.H in the 24H2 SDK, they are not a full replacement and you must have the 24H2 10.0.26100 (or higher) SDK installed as explained above.

This is _not_ an SSE/AVX-to-NEON mapping layer per se, although that technique is heavily used to replace the existing static library functions with inline functions that may contain NEON intrinscics or just operate directly on the _m128 and _m256 structures.  It is not stricly necessary to always try to map to NEON intrinsics, as in my testing I found that the Visual Studio C/C++ compiler vectorizer is quite good these days.

ARM64 auto-vectorization arrived in Visual C/C++ 2022 17.6, using 17.12 or higher is recommended since the compiler code quality been improving steadily since.

The file USE_SOFT_INTRINSICS.H implements the wrapper around intrin.h/windows.h to make life easier:

  - it plays some macro voodoo to work around that _m256 compiler block, otherwise this project would be dead in the water until Microsoft fixes bug: https://developercommunity.visualstudio.com/t/Arm64EC-Code-fails-to-compile/10838593

  - it enables native ARM64 target by defining the USE_SOFT_INTRINSICS macro and adds SOFTINTRIN.LIB to the list of libraries.  This is what ARM64EC does automatically, but you have to do this manually for ARM64.  You can add these in the Visual Studio project itself, but by doing this in the header it makes it trivial to build from the command line using CL.EXE

  - it works around another ARM64EC compiler bug which also causes the compiler to stop otherwise: https://developercommunity.visualstudio.com/t/VC-176-preview-1-x86-compiler-bad-cod/10291481

The file SOFTINTRIN_AVX2.H implements the actual soft intrisic overrides for SSE and the new soft intrinsics for AVX/AVX2.  It is #include-ed by the other header for convenience.  I'm keeping them as separate .H files since they serve entirely different purposes.  In theory once the Visual Studio compiler bugs are fixed most of USE_SOFT_INTRINSICS.H will just go away and then you will just need SOFTINTRIN_AVX2.H

There are some subdirectories in this project (m256_demo, dvec_demo) which demonstrate the above compiler bugs and how they go away once your add the #include <use_soft_intrinsics>

This initial commit today (April 9 2025) is the bare bones to get things started and unblock DVEC.H and speed up ToyPathTracer.  More demos and more new soft intrinsics will be added over time.



