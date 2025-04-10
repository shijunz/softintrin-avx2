
@if not "%VSCMD_ARG_TGT_ARCH%" == "arm64" (
    echo Visual Studio ARM64 build environment not initialized.
    echo Make sure to run vcvarsarm64.bat or vcvarsamd64_arm64.bat
    goto end
    )

@echo This demo shows the steps to successfully compile AVX2 data types
@echo and include DVEC.H with ARM64 native and ARM64EC builds.
@echo ...
@echo This first attempt to compile as plain ARM64 should fail!
@pause
cl -c demo1.cpp
@echo ...
@echo This first attempt to compile as ARM64EC should fail too!
@pause
cl -c -arm64EC demo1.cpp
@echo ...
@echo This version sets USE_SOFT_INTRINSICS variable, should succeed as ARM64
@pause
cl -c demo2.cpp
@echo ...
@echo But this attempt to compile as ARM64EC should still fail   :-(
@pause
cl -c -arm64EC demo2.cpp
@echo ...
@echo This final attempt using the new .H should succeed as native ARM64
@pause
cl -c -I.. demo3.cpp
@echo ...
@echo And this final attempt to compile as ARM64EC should now succeed!  :-)
@pause
cl -c -I.. -arm64EC demo3.cpp
:end
