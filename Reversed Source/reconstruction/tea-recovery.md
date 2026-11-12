# TEA recovery

Target: `PROJECT_ROOT/Original Binaries/game`, SHA256 `61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c`; ELF x86 little endian, 32-bit, GCC. The attached Ghidra program identity was verified before analysis. No target execution or shared Ghidra changes were performed.

## Evidence and conclusions

`tea-ghidra.json` retains complete, untruncated Ghidra decompilation and disassembly, including instruction bytes, for `tea_code` at `0x081b9960`, `tea_decode` at `0x081b9fe0`, `TEA_Encrypt` at `0x081ba660`, and `TEA_Decrypt` at `0x081ba6f0`. `tea-dwarf.json` retains independently extracted ELF DWARF types, const qualifiers, and the `tea_nilbuf` location and bounds. The latter is external `char[8]` at `0x0842d440` in `.bss`, hence initially zero.

DWARF proves DWORD is `unsigned long`, four bytes (typedef DIE `0x2dbd99f`, base DIE `0x2dbba42`), not the supplied newer tree's `unsigned int`. Pointer-to-const DWORD is DIE `0x2dbdd7c`, pointer-to-DWORD `0x2dbdd82`, and signed four-byte int `0x2dbb9fa`. Verified signatures:

```c
void tea_code(const DWORD sz, const DWORD sy, const DWORD *key, DWORD *dest);
void tea_decode(const DWORD sz, const DWORD sy, const DWORD *key, DWORD *dest);
int TEA_Encrypt(DWORD *dest, const DWORD *src, const DWORD *key, int size);
int TEA_Decrypt(DWORD *dest, const DWORD *src, const DWORD *key, int size);
extern char tea_nilbuf[8];
```

The helpers are externally emitted symbols. Their straight-line operations implement 32 XTEA cycles: encryption updates y at the old sum, increments sum by `0x9e3779b9`, then updates z; decryption reverses this from `0xc6ef3720`. Source reconstruction folds the 64 verified half-rounds into a loop. This is a readable reconstruction, not an assertion of identical historical source spelling or machine code.

Wrappers traverse blocks forward, load both input words before the helper writes output, and call the helper anew with the original key pointer on every block. This preserves in-place and overlapping buffers and changes to key material aliased with output. No key snapshot may be hoisted across blocks. Encryption rounds size and writes zero padding directly into the source despite its const qualifier. Decryption rounds the readable length but does not pad or strip bytes. Aligned lengths get no extra block; zero length touches no buffers. There is no validation or capacity check.

The disassembly implements signed remainder rounding, including negative values: on a nonmultiple of eight it adds seven for negative size, clears three low bits, then adds eight. The reconstruction expresses this in 32-bit unsigned arithmetic to preserve target wrap behavior without signed overflow. For example -1 rounds to 8 and encryption writes before the supplied source; -9 rounds to 0; -8 returns -8 without work. These historical behaviors are retained, not presented as safe API usage. Tests provide valid surrounding memory for negative-length cases. Large encryption inputs and invalid pointer/capacity cases are not executed.

The supplied `REFERENCE_PROJECT/Source/srv/server/libthecore/src/tea.c` agrees with the verified rounds, word ordering, padding mutation, and length calculation. It served as a comparison reference; its includes and typedef choice were not copied. The width guard deliberately rejects LP64 unsigned long.

## Validation

`reconstruction/tests/tea_oracle.inc` mechanically normalizes the retained helper pseudocode: rename the helpers, replace Ghidra's `uint` with the verified DWORD, and restore key constness. Its fully unrolled expressions are a static-evidence oracle, not original-binary execution. `tea_regression.c` tests an independent zero-key/zero-input fixed XTEA vector (`dee9d4d8 f7131ed9` as words), 2000 deterministic direct helper comparisons including independent decryption inputs, 812 complete-buffer wrapper comparisons covering lengths -17 through 40, disjoint/in-place/forward-overlap/backward-overlap and key/output/source aliases, zero buffer access, and signed size boundaries for decryption. Full-arena comparisons verify write extent and padding as well as output.

Passed with host MinGW GCC at O0 and O2, and a 32-bit source syntax check. Host MinGW has the required four-byte unsigned long; these checks do not claim FreeBSD ABI runtime validation or a full server link. Commands from the reconstruction root (substitute compiler path as needed):

```text
C:/Strawberry/c/bin/gcc.exe -std=c99 -Wall -Wextra -Werror -Wno-parentheses -pedantic -O2 reconstruction/tests/tea_regression.c libthecore/src/tea.c -o reconstruction/tests/tea_regression.exe
reconstruction/tests/tea_regression.exe
C:/Strawberry/c/bin/gcc.exe -std=c99 -Wall -Wextra -Werror -pedantic -m32 -fsyntax-only libthecore/src/tea.c
```

`-Wno-parentheses` applies to the test build because the retained decompiler oracle uses minimal C precedence parentheses; the reconstructed source passes without it. Shared headers and the shared test Makefile are integrated by the coordinating worker.
