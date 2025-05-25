@rem using -O1 generates a correct binary which emits all zero output as expected
cl -FAsc -Zi -O1 -I.. -arch:AVX2 -Tc avx2_intrin_bug.c -link -out:avx2_intrin_no_bug.exe -debug -release -incremental:no 

@rem using -O2 generates a corrupted binary which leaks non-zero stack locals BUG!!!!
cl -FAsc -Zi -O2 -I.. -arch:AVX2 -Tc avx2_intrin_bug.c -link -out:avx2_intrin_bug.exe -debug -release -incremental:no 
