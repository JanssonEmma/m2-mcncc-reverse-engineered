# Socket option recovery

Target identity: attached Ghidra `game`, ELF x86 little endian 32-bit GCC, SHA-256 `61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c`. `socket-options-ghidra.json` retains identity, ten decompilations and complete non-paginated disassemblies. No target execution or real socket operations performed.

## Recovered interface

All ten return `void`; socket_t is the target's signed int descriptor. Functions `socket_nonblock`, `socket_block`, `socket_dontroute`, `socket_lingeroff`, `socket_lingeron`, `socket_reuse`, `socket_keepalive` take `(socket_t s)`. `socket_rcvbuf`, `socket_sndbuf` take `(socket_t s, unsigned int opt)`. `socket_timeout` takes `(socket_t s, long sec, long usec)`. All target integers/longs and socklen_t are 4 bytes, struct linger and timeval are 8 bytes. The supplied reference POSIX bodies match the observations; Windows branches were excluded.

| Function | Entry | Failure log line(s) |
|---|---|---|
| socket_nonblock | 081b68d0 | 311 |
| socket_block | 081b6950 | 325 |
| socket_dontroute | 081b69d0 | 349 |
| socket_lingeroff | 081b6a50 | 368 |
| socket_lingeron | 081b6ae0 | 387 |
| socket_rcvbuf | 081b6b70 | 401, 411 |
| socket_sndbuf | 081b6c90 | 427, 436 |
| socket_timeout | 081b6da0 | 460, 467, 476, 483 |
| socket_reuse | 081b6f80 | 497 |
| socket_keepalive | 081b7010 | perror only |

## Observed behavior

fcntl uses F_GETFL=3 and F_SETFL=4, toggling bit 4 (O_NONBLOCK). It does not check the GET result before transforming it; it reports only a negative SET result and never closes the descriptor. The block helper's message still says `fcntl: nonblock: %s`.

All socket options use SOL_SOCKET=65535. Constants in the binary match the test shim: SO_REUSEADDR=4, KEEPALIVE=8, DONTROUTE=16, LINGER=128, SNDBUF=4097, RCVBUF=4098, SNDTIMEO=4101, RCVTIMEO=4102. Linger disable sends {0,0}; enable sends {1,0}. Reuse and keepalive send integer 1. The latter prints `setsockopt: keepalive` through perror, whereas the others use _sys_err with strerror(errno).

Receive/send buffer routines first set the caller's requested value, then clear their local value to zero, reset optlen to 4 and read back the kernel result. A failed receive option call closes the descriptor; a failed send option call reports and returns without closing. A successful pair logs the read-back value at level 1.

Timeout initializes two distinct timeval values, then calls set RCV, get RCV, set SND, get SND. It resets optlen to 8 before the send half even if the receive getter altered it. First failure logs and closes immediately. Only complete success logs both read-back timevals at level 1. Source uses sizeof rather than hardcoded 8 to express original platform layout; 32-bit ABI tests must establish that layout.

## Preserved uncertainty

socket_dontroute's option integer is indeterminate. At 081b69db the function forms the address of [EBP-8], and the path to setsockopt at 081b6a01 contains no write to that location. Both the decompilation and supplied reference contain uninitialized `int set;`. The fragment preserves that declaration; assigning 1 would invent behavior. Its exact stack bytes are unknown, modern compiler behavior around uninitialized storage is not binary-equivalent, and tests must not assert or read the option value. A modern warning is expected when this fragment is compiled against an externally declared setsockopt, depending on compiler flags; do not silence it by changing the initialization.

The fragment uses _sys_err(__FUNCTION__, original line, ...) to retain original diagnostics independently of the reconstructed file's physical line numbers. Static analysis confirms call order and branches; parent owns syscall-mocked regression integration and compilation. No original FreeBSD runtime parity is claimed.

Integrated: the complete module is now in libthecore/src/socket.c. C99 __func__ replaces the staged __FUNCTION__ spelling with the same diagnostic name. The syscall/DNS-mocked suite passes at O0/O2, the 32-bit object compiles with the documented DONTROUTE warning exception, and all19 integrated suites pass. See socket-recovery.md.

