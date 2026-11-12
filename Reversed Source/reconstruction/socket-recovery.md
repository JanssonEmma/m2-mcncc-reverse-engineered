# Socket module recovery

Target: game, SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Read-only analysis of the supplied 32-bit x86 FreeBSD GCC ELF; the original server was not executed.

All 20 addressed socket.c functions now have reconstructed definitions in libthecore/src/socket.c and DWARF-verified declarations in include/socket.h. socket_t is a signed four-byte int in typedef.h. Native Windows networking branches were not added.

## Evidence and coverage

socket-io-ghidra.json retains ten complete I/O/bind/accept/connect decompilations; socket-io-assembly.json retains five focused complete disassemblies. socket-options-ghidra.json retains target identity, ten complete option decompilations and their complete assembly. socket-dwarf.json records all public prototypes and relevant type layouts. socket-ghidra.json consolidates all 20 bodies, and socket-coverage.json maps evidence and definition coverage. tools/audit_socket_recovery.py rebuilds the ledger.

Coverage means addressed-body/source-name coverage, not original source spelling or binary equivalence. Inline libc/compiler helpers such as byte swapping and rune classification are represented by target system headers/library calls rather than copied from decompiler internals. The additional five external helpers absent from the newer public header are declared because the old target emits them externally.

An independent bounded review found no material signature or control/data-flow discrepancy in the first ten functions/header versus the retained DWARF, decompilation and assembly. Detailed option observations are in socket-options-recovery.md.

## Observed old behavior

- UDP read forwards recvfrom's return and caller's address/length pointers without special error processing.
- Stream read returns positive bytes, -1 on EOF, 0 on negative EINTR/EAGAIN/EWOULDBLOCK/EDEADLK, otherwise logs original function/line58 and returns -1.
- Low-level TCP write returns positive bytes, -1 on a zero send, 0 for negative EAGAIN/EWOULDBLOCK/EDEADLK, and -1 for other failures. EINTR is fatal here, unlike stream read.
- The full write wrapper is a do/while loop, even for zero length. Positive partial writes advance the pointer and remaining size. A transient zero result retries immediately with no yield, timeout or return to the caller; persistent would-block can spin indefinitely. A negative result logs original line109 or111 and returns -1 without closing. Tests use a finite scripted retry sequence, not an infinite loop.
- Bind creates an IPv4 socket, enables reuse, disables linger, zeroes the address, uses inet_addr and a truncated 16-bit network-order port, then binds. It leaves BSD sin_len zero. Bind failure returns -1 without closing the created descriptor. Option helpers returning void can close it on failure, but bind still continues. Nonblocking setup is followed by a TCP/UDP log without the newer fd suffix; TCP listen uses the target SOMAXCONN=128 and its result is ignored.
- Accept initializes peer length to16. -1 logs original line196. Results >=65500 log line202 and close the LISTENING descriptor rather than the accepted descriptor, then return -1. Ordinary accepted descriptors become nonblocking with linger disabled. The original close wrapper ignores close's result.
- Connect dispatches on the first host byte: the observed signed-byte/rune guard leads numeric-first strings directly to inet_addr, even if they are not valid numeric addresses. Other strings use gethostbyname and copy four bytes from the first returned address through the real thecore_memcpy pointer. DNS failure logs line229 before socket creation. No hostent address-family/length/list validation is added.
- Connect setup order is keepalive, send buffer233016, receive buffer233016, ten-second send/receive timeout, then linger enabled with zero linger duration. Helpers return void and do not stop the sequence after closing on an option failure. The connection remains blocking.
- On negative connect result the descriptor closes, but error classification switches on that return value rather than errno. Normal -1 therefore takes the generic original line285 diagnostic; positive EINTR/ECONNREFUSED/ENETUNREACH cases inside that branch are unreachable for ordinary syscall results. perror("connect") follows. Socket creation failure uses perror("socket").

The ten option helpers preserve fcntl GET/SET sequencing, no GET-result check, the block helper's "nonblock" diagnostic, the asymmetric receive/send-buffer close behavior, and timeout set/get ordering and length reset. Timeout values are forwarded without normalization. Option diagnostics retain original line constants. The reconstruction uses standard C99 __func__ in place of the staged GCC __FUNCTION__ spelling; both produce the same function-name string.

## Indeterminate original behavior

socket_dontroute declares an uninitialized int and passes its address to setsockopt. Assembly confirms no prior write to that stack location. Its exact bytes cannot be recovered as a fixed value and are not guaranteed equivalent under modern compiler behavior. Initializing it to1 would invent behavior, so the source keeps it uninitialized. The regression checks option identity, length and failure cleanup but deliberately does not read/assert the option payload. GCC's maybe-uninitialized warning remains visible. A hardened variant is not substituted.

System errno numbers, socket/fcntl constants and ABI structures must come from the intended target's system headers in production. Target-valued test headers are isolated under reconstruction/tests/socket_shim and are never production replacements.

## Validation

The socket regression links the actual complete socket.c and intercepts every socket, send/receive, bind/listen/accept/connect, option, fcntl, close, DNS, address conversion and diagnostic operation. No sockets are opened, descriptors modified or hosts resolved. Tests cover partial/retried/zero-length writes, read error distinctions, port encoding/address clearing, bind failures and ignored listen errors, acceptance thresholds and wrong-descriptor close, DNS selection/copy, connection setup order and continuation after every option failure, close/log ordering, option readback and timeout-length reset.

The socket suite passes at O0 and O2. A -m32 O2 object compilation passes using the test POSIX declarations. Optimized/32-bit checks use -Wall -Wextra -Werror -pedantic with only -Wno-error=maybe-uninitialized to retain the known DONTROUTE diagnostic as a warning rather than changing behavior. This exception is not a warning-free claim.

check-abi now also compiles socket_abi.c in32-bit mode. It checks the test declarations against DWARF: socket_t/size_t/socklen_t/ssize_t four bytes; BSD sockaddr/sockaddr_in16 with separate byte-length/family fields; in_addr4; hostent20; linger8; timeval8 and member offsets. This validates the fixture model, not a genuine installed FreeBSD SDK.

The combined make check/check-abi run passes19 regression suites and structural checks. Existing two logger command-buffer warnings remain alongside the socket DONTROUTE warning. Full server networking/descriptor integration, FreeBSD runtime verification, original libc locale/rune classification and binary equivalence remain unverified.
