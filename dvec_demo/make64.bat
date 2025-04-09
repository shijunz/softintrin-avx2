
@rem Builds 32-bit x86 version and 64-bit versions for x64 ARM64EC and pure native ARM64.
@rem Run the Visual Studio vcvars32.bat _or_ vcvars64.bat / vcvarsarm.bat scripts ahead of time.

@if "%VSCMD_ARG_TGT_ARCH%" == "" (
    echo Visual Studio build environment not initialized.
    echo Make sure to run vcvars32.bat vcvars64.bat or vcvarsamd64_arm64.bat
    goto end
    )

@if "%VSCMD_ARG_TGT_ARCH%" == "x64" (
  @ml64.exe 1>nul 2>nul
  echo %errorlevel%

  @if not "%errorlevel%" == "0" (
    echo 64-bit assembler not found.
    echo Make sure to run vcvars64.bat
    goto end
    )

  @rem assemble the ASM file
  ml64 -c -Zd -Zi avx2.asm

  @if not exist avx2.obj (
    echo ASM failed.
    @if "%VSCMD_ARG_TGT_ARCH%" == "arm64" (
      echo Make sure to run the x64 build first!
      )
    goto end
    )
  )

@setlocal

@rem maximum warnings, debug symbols, compile for size, generate listings.
@rem for better debugging emit frame pointers and spill homeparams.
@rem link against the static C runtime library for better backward compat.
@rem ARM64 pure native build is the default unless -arm64EC is passed as an argument.
set CL_ARGS=-W4 -Zi -FAsc -O2 -Oy- -MT

@if "%VSCMD_ARG_TGT_ARCH%" == "x64" (
    set CL_ARGS=%CL_ARGS% -homeparams
    )

@if "%VSCMD_ARG_TGT_ARCH%" == "arm64" (
    set CL_ARGS=%CL_ARGS% -arch:armv8.2 -feature:rcpc -volatile:ms -I.. %1
    )

cl -c %CL_ARGS% test.c
cl -c %CL_ARGS% dvec.cpp

@rem maximum debug information, remove dead code
set LINK_ARGS=-debug -release -opt:ref -incremental:no

@rem x64 supports emitting unwind information for all functions.
@rem This is useful for code discovery for offline x64-to-ARM64 binary rewriting.:w
@if "%VSCMD_ARG_TGT_ARCH%" == "x64" (
    set LINK_ARGS=%LINK_ARGS% -allpdata
    )

@rem force an ARM64X binary format
@if "%VSCMD_ARG_TGT_ARCH%" == "arm64" (
    if "%1" == "-arm64EC" set LINK_ARGS=%LINK_ARGS% -machine:arm64ec
    )

@rem the libraries that are generally needed for command line (using UCRT not MSVCRT)
set LINK_LIBS=libucrt.lib onecore.lib kernel32.lib user32.lib ntdll.lib softintrin.lib

@if "%VSCMD_ARG_TGT_ARCH%" == "x64" (
set LINK_LIBS=%LINK_LIBS% avx2.obj
    )

link %LINK_ARGS% test.obj dvec.obj %LINK_LIBS%

@endlocal

:end

