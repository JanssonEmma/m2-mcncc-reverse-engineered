# Recovered string/file utility portion

Observed target: game, ELF x86 32-bit GCC, SHA256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Attached Ghidra identity confirmed before read-only decompilation; original binary was never executed. `utils-strings-ghidra.json` retains complete pseudocode and trim assembly. `utils-strings-dwarf.json` was extracted directly from target DWARF with `tools/utils_strings_metadata.py` (pyelftools). The staged `fragment.c` contains source reconstructions, not literal original source.

## Exact public declarations from target DWARF

```c
void printdata(const unsigned char *data, int bytes);
char *time_str(time_t ct);
void trim_and_lower(char *dest, const char *src, int max_len);
void lower_string(const char *src, char *dest, int len);
char *str_dup(const char *source);
int is_abbrev(char *arg1, char *arg2);
int filesize(FILE *fp);
void parse_token(char *src, char *token, char *value);
```

`time_t` is signed int32 in this binary; `FILE` is a target libc type of 88 bytes. Use target system headers rather than embedding host libc layouts. Ghidra incorrectly labels printdata bytes bool and omits several const qualifiers; DWARF resolves these.

## Evidence and behavior

* printdata 0x081b8a30 agrees with supplied reference logic. Always emits at least one row, including bytes <= 0. Hex columns total 16, text columns equal remaining length capped at 16. A byte is displayed only when it AND the following byte pass `((b & 0xe0)>0x90) || isprint(b)`. Thus even the last displayed byte may read one following byte beyond the declared length. Caller backing memory requirements are unchanged. The fragment preserves short-circuit evaluation.
* time_str 0x081b8d00 returns asctime(localtime(&ct))+4 after truncating the shared asctime buffer at strlen-6. No null/error guards. Not reentrant. The reference agrees.
* trim_and_lower 0x081b8d50 has the OLD destination-first API with signed int max_len (reference has changed API and safety behavior). Null src returns leaving dest untouched. Leading whitespace uses the non-high-byte guard plus ctype whitespace; copying lowercases ASCII A-Z only. Nonempty input copies a first byte even when max_len<=0; after each copied byte, signed count>=limit stops. It then NUL-terminates and decrements before whitespace checking. Empty or all-whitespace source reads dest[-1], and if that previous byte is whitespace can erase preceding memory. Assembly confirms offsets at +8 dest,+12 src,+16 int; 0x081b8dc0 NUL-writes then DEC EBX then reads byte. This behavior was preserved rather than silently hardened. Tests provide backing storage and canaries to exercise it without invalid access.
* lower_string 0x081b8e60 has signed int len counting source bytes, not total capacity. It writes a terminator at index len when truncated, requiring len+1 space, and when source is shorter it zero fills indices through len-1 only. Even len<=0 writes dest[0]=0. Source is dereferenced without null guard. ASCII lowercase only. Newer reference changed semantics.
* str_dup 0x081b8ed0 uses calloc(strlen+1,1), strcpy, and perror("malloc failure") followed by abort on allocation failure. Test intercepts failure without ending process.
* is_abbrev 0x081b8f20 rejects empty first arg and performs ASCII case-insensitive prefix comparison. No null guards. Reference agrees.
* filesize 0x081b8fa0 saves ftell in signed int, seeks end, takes signed int size, restores original position. Errors are not handled. Tests fake all file calls; no files opened.
* parse_token 0x081b9000 removes ALL non-high whitespace from name and ASCII-lowercases it, then skips colon PLUS EXACTLY ONE following byte. It removes CR/LF throughout value but preserves other whitespace and case. Missing colon still advances two bytes beyond first NUL. No output capacities or guards. Tests use deliberately padded malformed inputs to check skip behavior without invalid memory access. Reference agrees.

## Verification and limits

Standalone regression compiles the staged fragment independently; it does not include implementation into the test translation unit. `hooks.h` force-includes fake fprintf/ftell/fseek/localtime/asctime/calloc/perror/abort for the implementation only. Test owns only symbols prefixed strings_test_. All stdout reporting is normal; no file writes, process aborts, or original execution. Assertions cover truncation, zero/negative limits, zero padding/canaries, null-src behavior, prefix comparisons, high-byte preservation, parse separators/newlines/malformed padded input, allocation failure, time storage/truncation, file seek restoration, dump row layout and next-byte classification.

Command: GCC -std=c99 -O0 -Wall -Wextra -include work/utils_strings/hooks.h -c work/utils_strings/standalone.c, then link regression.c with object and run. Result: utils string/file regression passed. Host compiler is MinGW x64, so this establishes source behavior under the controlled fixture, not FreeBSD binary execution equivalence. ctype is supplied by target libc; host locale-dependent non-ASCII classification is not verified and must not be claimed identical. ASCII lowercase is deliberately locale-independent because target instructions implement literal A-Z comparisons. The original invalid-access edge cases remain; test backing avoids undefined accesses but does not make unpadded callers safe.

Integration: root should append fragment.c bodies into shared utils.c and add required stdio/stdlib/string/time/ctype headers. Header declarations above need no custom OS dependency. For full utils linking the parent must also supply its existing time/random/process/log fixtures; this standalone fixture supplies none of random/gettimeofday/select/fork/_sys_err/thecore_memcpy.

Integrated validation: these functions are now in libthecore/src/utils.c. The Makefile's utils_strings_regression links the actual full utility module with its dedicated hooks and common dependency stubs. The combined 16-suite check passes; see utils-recovery.md.

