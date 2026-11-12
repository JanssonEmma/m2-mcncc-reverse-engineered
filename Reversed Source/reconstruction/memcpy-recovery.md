# Optimized memcpy recovery

Four emitted implementations are reconstructed from retained decompilation and disassembly in memcpy-ghidra.json, with table and initial pointer metadata in memcpy-metadata.json. The default thecore_memcpy pointer refers to libc memcpy. The original reference notice is preserved in memcpy-reference-notice.txt.

linux_kernel_memcpy uses rep movsl/movsw/movsb. For lengths below four, its return pointer advances by the copied byte count; lengths at least four return the original destination. This unusual branch was checked in assembly and intentionally retained.

The MMX implementation switches at 2048 bytes, aligns destination to eight bytes, copies 64-byte groups and executes emms. MMXEXT switches at 64 bytes, prefetches, uses movntq and executes sfence/emms. Raw 0f18 instruction bytes establish prefetch behavior where Ghidra printed NOP. GNU x86 inline assembly is used; no native MSVC implementation is supplied.

The selector allocates two 0xfa0000-byte buffers, warms libc twice, times 2000 8-KiB copies per method using rdtsc, selects the first strict minimum, logs and frees buffers. Method ordering is libc, MMXEXT, MMX, linux kernel. No extra CPU feature gate was added.

Tests execute copies across thresholds and unaligned destinations, verify return pointers and selector behavior. Host MMX execution and a -m32 object compile passed. Timing outcomes and exact performance depend on CPU/compiler and are not claimed identical. Overlapping copies remain outside memcpy's contract.
