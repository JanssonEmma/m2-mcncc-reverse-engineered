# Old buffer module recovery

Target game SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c, 32-bit x86 FreeBSD GCC ELF. The original server was not executed and Ghidra was used read-only.

All 16 addressed buffer.c functions are reconstructed in libthecore/src/buffer.c with the observed C interface in include/buffer.h. buffer-ghidra.json retains complete pseudocode for all 16; buffer-assembly.json retains six focused disassemblies. buffer-dwarf.json supplies exact signatures and the 32-byte BUFFER layout. buffer-globals.json records three local pool heads in BSS.

The newer reference has power-of-two normalized pools and C++ reference parameters. This old implementation instead has buffer_new(void*,int), pointer-by-value mutation, buffer_byte/word/dword names, and in-place object growth. The three zero-initialized heads are buffer_poll at 0x0842d31c, buffer_poll_big at 0x0842d320, and buffer_nodelete_poll at 0x0842d324.

## Allocation, reuse and ownership

Negative size returns NULL before allocation. Nonnull destination wraps external storage and sets low flag bit 0x1; it allocates only a header, reusing the nodelete header pool when available. NULL destination selects the small pool for requested sizes <=8192, otherwise the big pool. It pops the head without searching for a best size. A new owned allocation has exactly requested capacity, with calloc-zeroed data. Reused data is not zeroed and capacity does not shrink; a too-small reused allocation grows.

buffer_reset rewinds pointers and zeroes length/write position. It preserves flag and next. Deletion calls reset without a NULL guard. External storage is never freed: the header's mem_data is set NULL and the header is pooled. Its read/write pointers temporarily still contain the old external address until reuse. Owned capacity >131072 frees both data and header; lower capacities enter the <=8192 small pool or big pool. Pools are LIFO and have no recovered teardown API.

Header/data calloc failures use perror("malloc failure") then abort. A zero-size calloc result remains libc-dependent. No newer pool scavenging/retry behavior was copied.

## Growth and I/O

Write grows on equality as well as overflow: write_position+length >= capacity. Requested new capacity is old capacity+length+MIN(1048576,2*length). Adjust-size only grows for a strict > comparison and requests old capacity+add_size. Realloc leaves the BUFFER object's address unchanged, allocates new data with malloc, copies the entire old capacity through the real thecore_memcpy pointer, preserves read/write offsets, length, flags and links, then frees old data. The added tail is uninitialized.

Realloc of external storage logs original function/line240 and returns unchanged. Write still copies afterward, without checking that growth succeeded; callers must provide adequate backing storage. Malloc failure also has no recovered check before the copy call. These behaviors are retained.

Read copies its requested bytes BEFORE read-proceed clamps oversized consumption. Byte/word/dword getters directly load little-endian native-width data and have no pre-read length check; word/dword may be unaligned on x86. A complete read resets the positions. Negative proceed logs original line176 but may move the read pointer backward and increase unread length. Oversized proceed logs line179 and clamps. Partial progression beyond declared capacity logs line189 and aborts. Write-proceed itself has no range check.

The negative realloc assertion expression is confirmed from target bytes at 0x08233dc0: length >= 0 && "buffer_realloc: length is lower than zero". Standard assert reconstructs the condition, but compiler/OS assertion formatting and reconstructed source file/line differ. Extreme signed-overflow and invalid-pointer states remain outside guaranteed source parity under modern optimization; no broad arithmetic hardening was added.

## Validation and limits

buffer_regression.c includes the actual production implementation to inspect its private pools while intercepting allocation, free, memcpy, diagnostics and abort. It passes strict C99 at O0/O2, covering pool thresholds and LIFO reuse, stale data/capacity/flags, borrowed ownership and refusal to grow, in-place growth offsets and full-capacity copying, growth limits/equality, direct and typed I/O, negative/oversized progression and failure paths. Deliberately malformed cases have sufficient backing memory. The unchecked failed-growth copy is intercepted at NULL destination; no real invalid copy or process termination occurs.

The real manager regression separately links the C implementation as a separate object. Both modules pass strict -m32 object compilation, and check-abi verifies every BUFFER offset and its size32. The integrated suite passes 18 regressions plus structural checks. These are modern MinGW host tests, not FreeBSD runtime or binary equivalence verification.
