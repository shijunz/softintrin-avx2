@if "%1" == "-b" goto bench

@if not "%VSCMD_ARG_TGT_ARCH%" == "x86" (
    echo Make sure to run vcvars32.bat or vcvarsarm64_x86.bat first!
    goto done
    )

@rem SSE4-only native 32-bit x86 build
cl -FAsc -Zi -O2 -I../dvec_demo -I.. -arch:SSE2   -Tc test-intrins.c -link -out:test-intrins-x86-sse4.exe -debug -release -incremental:no

@rem SSE4+AVX2 native 32-bit x86 build
cl -FAsc -Zi -O2 -I../dvec_demo -I.. -arch:AVX2   -Tc test-intrins.c -link -out:test-intrins-x86-avx2.exe -debug -release -incremental:no

@rem Run both the correctness tests and micro-benchmarks.
@rem Optionally define LOADER with a debugger command line (e.g. "cdb -o -g -G") or TTD command line (e.g. "sudo ttd")

:bench

if exist test-intrins-x86-sse4.exe (%LOADER% test-intrins-x86-sse4.exe    -o test-x86-sse4.txt)
if exist test-intrins-x86-avx2.exe (%LOADER% test-intrins-x86-avx2.exe    -o test-x86-avx2.txt)

if exist test-intrins-x86-sse4.exe (%LOADER% test-intrins-x86-sse4.exe -b -o bench-x86-sse4.txt)
if exist test-intrins-x86-avx2.exe (%LOADER% test-intrins-x86-avx2.exe -b -o bench-x86-avx2.txt)

@rem Next steps:
@rem
@rem Capture reference output on an AMD or Intel CPU, then re-run the above tests
@rem under Prism, QEMU, Rosetta 2, etc. to compare per-instruction performance and
@rem and correctness of the respective emulators.  Some per-implementation differences
@rem will exist between AMD and Intel on intrinsics such as reciprocals and
@rem reciprocal square roots.  Emulators may have correctness errors with NaN outputs.
@rem
@rem Then, build on an ARM64 device using make-arm64.bat to compare soft intrinsics.

:done

