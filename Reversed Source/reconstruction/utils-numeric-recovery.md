# Numeric utils recovery

Observed target: game, x86 LE 32-bit GCC, SHA256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Read-only attached Ghidra results retained in utils-numeric-ghidra.json; no original execution.

Five emitted bodies recovered: MIN 081b9260, MAX 081b9280, MINMAX 081b92a0, thecore_random 081b92c0, number_ex 081b92d0. DWARF prototypes retained in utils-numeric-dwarf.json. In particular number_ex takes char *file, not the newer const char *; DWORD is unsigned long of size 4. All other numeric inputs/results are signed 4-byte int.

MINMAX first imposes its lower bound, then upper bound, so inverted bounds always produce max. thecore_random directly calls FreeBSD random(), with no Windows rand branch in this target.

number_ex logs original reversed endpoints before swapping, using _sys_err function name number_ex and original source line 339. It then always draws randomness, including equal endpoints. DIV ECX at 081b92fd is unsigned. SUB and LEA operate with 32-bit wrapping; the fragment uses DWORD arithmetic to preserve this without signed subtraction overflow. The returned bit pattern is converted to signed int using the intended GCC target semantics.

Unlike the supplied newer source, this binary has no zero-span guard. INT_MIN through INT_MAX produces zero divisor and a hardware divide fault in the original. That fault is established statically and deliberately not executed in the regression. Modulo bias remains unchanged. FreeBSD random generator sequence and ABI behavior remain target-runtime dependencies; tests mock the randomness provider and logger and execute only recovered code.

The independent regression covers signed limits, inverted bounds, inclusive/wrapping ranges, reversed endpoint logging, random-call counts and equal endpoints. It requires 32-bit int and DWORD (including the MinGW x64 host, where long is 32-bit). Standalone build: gcc -DNUMERIC_STANDALONE -std=c99 -Wall -Wextra -O2 regression.c -o regression.exe. Without NUMERIC_STANDALONE it includes the destination public header and expects the full utils.c to be linked with the common platform fixtures.

Integrated validation: utils_numeric_regression now links the actual full utils.c, its public header and isolated support fixtures. The combined 16-suite run passes; see utils-recovery.md.

