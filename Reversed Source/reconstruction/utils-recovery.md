# Utility module recovery

Target SHA-256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.
The target is the attached 32-bit x86 FreeBSD GCC ELF. Analysis is read-only; the original server was not executed.

## Scope and evidence

All 22 addressed functions in the utils.c DWARF compilation unit now have reconstructed definitions in libthecore/src/utils.c. Public declarations use target DWARF types, including qualifiers and argument order that Ghidra pseudocode sometimes loses.

utils-coverage.json maps all 22 addresses to evidence and definitions. tools/audit_utils_recovery.py rebuilds the coverage ledger and consolidated utils-ghidra.json. Coverage means addressed body/name coverage, not proof of semantic or binary equivalence, nor recovery of every original macro or un-emitted inline function.

Retained evidence:
- Timing/calendar: heartbeat-ghidra.json, millisecond-clock-ghidra.json, calendar-ghidra.json, timing-metadata.json.
- Core dumping: core-dump-ghidra.json contains complete decompilation and 26 instructions.
- Additional timing: utils-time-extra-ghidra.json contains complete decompilation and assembly; utils-root-metadata.json gives DWARF type chains and the raw float divisor bytes.
- Strings/files: utils-strings-ghidra.json and utils-strings-dwarf.json; detailed behavior in utils-strings-recovery.md.
- Numeric helpers: utils-numeric-ghidra.json and utils-numeric-dwarf.json; detailed behavior in utils-numeric-recovery.md.

## Newly resolved process and time behavior

core_dump_unix at 0x081b9400 takes const char* and WORD (unsigned short, two bytes). It logs with original diagnostic function name and line 371, flushes stdout then stderr, calls fork, and aborts only for a zero fork return. Positive parent results and negative failure results return without waiting or extra logging. Flush failures are ignored. The caller process is not deliberately aborted. FreeBSD fork, signal and core-file semantics remain runtime dependencies.

thecore_msleep at 0x081b93c0 keeps a static timeval. Seconds are milliseconds/1000, but microseconds are the FULL milliseconds*1000 with 32-bit wrapping, not the fractional remainder. This can yield invalid or negative tv_usec. It invokes the recovered sleep helper once. The select call may modify that static storage; each new invocation rewrites both fields.

rdtsc at 0x081b9460 is one bare RDTSC instruction returning unsigned long long in EDX:EAX. The implementation combines explicit low/high GNU assembly outputs so it also works with the test host compiler. It adds no serialization, fences or timing calibration. Ghidra's apparently recursive pseudocode is an intrinsic rendering, disproved as recursion by the five-instruction disassembly.

get_float_time at 0x081b9470 subtracts fixed epoch 1057699978 from the signed 32-bit seconds and adds microseconds/1000000.0. Assembly shows integer subtraction and x87 integer loads; bytes 00247449 at 0x08202920 establish float divisor 1000000.0. The source uses unsigned subtraction followed by the intended GCC signed conversion to preserve wrapping without signed-overflow UB, and double intermediates before returning float. Exact x87 excess-precision effects are not claimed identical to modern SSE/compiler behavior.

Previously recovered helpers retain static borrowed results, negative-difference clamping, positive-only time normalization, no sleep retry on EINTR, fixed DWORD epoch wrapping, and the unusual calendar leap/month arithmetic.

## Old string and numeric interfaces

The trim API is destination-first: trim_and_lower(char*,const char*,int). lower_string uses a signed byte limit and may write a terminator at index len. These differ from the newer reference APIs. Their original empty-input underflow, missing-separator overread and dump look-ahead behavior are preserved and documented in the detailed notes. Regression fixtures provide sufficient backing storage; they do not make unsafe original callers safe.

number_ex takes char* file, uses inclusive unsigned 32-bit span arithmetic, logs then swaps reversed endpoints, and consumes randomness even for equal endpoints. The full signed integer range wraps the span to zero; the original hardware divide fault is established statically and was not executed. The module calls BSD random rather than replacing it with Windows rand.

## Validation

make check check-abi passes all 16 integrated suites with MinGW GCC/G++ 13.2 (C99/C++03). The three added suites link the actual complete utils.c:
- utils_regression links real signal.c too and verifies the SIGUSR1 handler's call into core_dump_unix, diagnostic arguments, flush/fork ordering, parent/failure/child branches, ignored flush failure, raw sleep values and wrap, and fixed-epoch float cases.
- utils_numeric_regression controls random values and checks inclusive/wrapping intervals, signed limits, reversed logging and random-call counts.
- utils_strings_regression intercepts allocation, time formatting, file positioning and dump output, checking old limits, padding, high bytes, token rules and allocation failure.

The child branch uses mocked fork/abort and setjmp/longjmp; no real child is created or process aborted. File position and formatting calls are fixtures; no files are opened. Existing heartbeat/log fixtures gain unused-dependency stubs that fail if accidentally invoked.

The complete utils.c also passes gcc -m32 -std=c99 -Wall -Wextra -Werror -pedantic object compilation using test POSIX declarations. Its 22 global function symbols were checked in the object, and emitted RDTSC was inspected. This is a MinGW i386 object check, not a FreeBSD executable or runtime check. Existing two logger command-buffer warnings remain in the broad regression build. No full-server build, original-ABI runtime or byte-identical output claim is made.
