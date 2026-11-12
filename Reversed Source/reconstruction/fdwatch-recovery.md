# fdwatch kqueue recovery

Target game SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c, FreeBSD x86 little-endian32 GCC ELF. Read-only Ghidra/DWARF analysis; the original server was not executed.

All12 addressed fdwatch.c functions are reconstructed in libthecore/src/fdwatch.c. include/fdwatch.h declares their exact interfaces and old public layout. fdwatch_register is externally visible in DWARF and remains nonstatic, despite its omission from the newer reference header. No addressed fdwatch_check_fd exists in this CU or the searched full inventory; no select/Windows implementation was fabricated.

## Evidence

fdwatch-ghidra.json retains all12 complete decompilations. fdwatch-assembly.json retains six focused complete disassemblies. fdwatch-dwarf.json supplies signatures, enum values and layouts. fdwatch-coverage.json maps all12 definitions and body evidence; tools/audit_fdwatch_recovery.py rebuilds it. Name/body coverage is not proof of binary equivalence.

Target FDWATCH is32 bytes: queue0, capacity4, changes pointer8, change count12, results pointer16, write-event-index pointer20, client-data pointer24, read/write-state pointer28. KEVENT is20 bytes: uint32 ident0, signedshort filter4, ushort flags6, uint fflags8, int data12, pointer udata16. Timespec is8 with signedint32 seconds and signedlong32 nanoseconds; timeval uses signedlong32 fields. Test headers model these layouts; production uses the target's system headers.

## Observed behavior

Creation calls kqueue before six calloc operations: watcher, 2*nfiles change events, 2*nfiles result events, nfiles indices, nfiles read/write states, and nfiles client pointers. A -1 queue result logs original line15 and returns NULL. Allocation failure prints malloc failure and aborts with no partial cleanup. There is no size validation. Delete frees the six allocations in observed order, without a NULL guard and WITHOUT closing the queue descriptor or registered sockets. This resource-lifetime limitation is retained.

Poll always passes a nonnull timespec. NULL timeout means zero wait; a supplied timeval copies tv_sec and copies tv_usec directly to tv_nsec, without multiplying by1000 or normalization. It submits pending changes and requests at most nfiles returned events, despite allocating room for2*nfiles. Pending count resets even if kevent returns -1. A failed poll leaves previous result/index memory; success clears all indices then stores the last WRITE event index for each descriptor. Out-of-capacity positive identifiers log line79; no negative-identifier guard is added.

Register writes only ident, flags and filter in a change entry. It preserves fflags/data/udata and does not bound pending-change count. EV_DELETE=2 enqueues READ then WRITE deletions according to currently tracked bits. Other flags enqueue one event; only rw==FDW_READ selects EVFILT_READ, so a combined mask selects WRITE. Flags truncate to16 bits.

Add rejects descriptor>=nfiles but not negative descriptors. Any overlap between an existing state and the requested mask returns immediately, without replacing client data. It ORs the mask, logs the old FDWATCH_REGISTER text, and stores the client pointer. Any nonzero oneshot sets EV_ADD|EV_ONESHOT (0x11) plus the state marker FDW_WRITE_ONESHOT=4, even for a read registration. Delete submits deletions before clearing client/state memory; clear_fd by itself submits nothing.

The overflow log at0x081b7500 supplies the descriptor argument. Ghidra's pseudocode missed that tail-call argument; assembly stores it at EBP+0x14 before repurposing the earlier argument slots. The source therefore correctly passes the descriptor to the line132 format rather than reproducing a decompiler omission. The apparent extra arguments to the one-shot log are stale decompiler stack artifacts; only its consumed integer is represented in source.

Check-event first compares ident, then prioritizes EV_ERROR=0x4000 and EV_EOF=0x8000 as FDW_EOF=8. Registered READ returns1. Registered WRITE returns2, clearing only WRITE when the sticky one-shot marker is set. The marker itself stays set; a later non-one-shot write registration can still be consumed as one-shot. Read events do not clear tracked READ state. Unknown filters log line200.

Event-index APIs assert only index<2*nfiles, not the latest poll result count. Clear-event sets a matching ident to0 and leaves its other fields, so it can subsequently alias descriptor0. Client lookup uses the result identifier to index the table; only the upper capacity bound is checked. Buffer-capacity lookup uses fd_event_idx[descriptor], tests only the selected filter, and returns its data. Since unrecorded descriptors retain index0 after a successful poll, this can report the first WRITE event's capacity for a different descriptor. These quirks are tested, not silently corrected.

## Validation and boundaries

fdwatch_regression links the actual complete module with fake kqueue/kevent, allocation, diagnostics, abort and assertion paths. It passes O0/O2 strict C99 checks, exercising allocation order and all six failure points, free order, registration overlap and partial event-field writes, deletion order, raw timeout units, zero waits, failed-poll state retention, duplicate write-event indexing, descriptor overflow, event priorities, one-shot rearming/sticky flags, descriptor0 aliasing, and all four event-index assertions.

No real kernel queue, socket, descriptor close or wait occurs. Test-only teardown frees allocations left by injected fatal allocation failures. Actual invalid memory accesses, negative descriptor indices, oversized pending-change lists and signed arithmetic overflow are outside the supported fixture cases and not claimed equivalent under modern optimization.

The module passes strict -m32 O2 object compilation using isolated test POSIX declarations. fdwatch_abi.c verifies FDWATCH32, KEVENT20, timespec8/timeval8, all member offsets and enum values. The test timespec declaration is guarded to coexist with C++ runtime headers; original-target ABI checks run separately in32-bit mode. Assertions retain the condition but reconstructed file/line and platform formatting differ.

Descriptor callsites are retained in descriptor-callers-ghidra.json and descriptor-output-assembly.json. The integration assessment identifies ProcessOutput as the first independently recoverable descriptor method; full Setup/Input/io_loop still need other modules. A complete server build and FreeBSD runtime validation remain pending.
