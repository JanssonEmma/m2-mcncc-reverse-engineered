# Event subsystem recovery

Binary identity is recorded in README.md. All addresses below are from that binary.

| Function | Entry |
| --- | --- |
| __AllocEventInfo | 0x0804cc60 |
| FindEventInfo | 0x0804cd30 |
| FreeEventInfo | 0x0804cd80 |
| event_create_ex | 0x0804ceb0 |
| event_cancel | 0x0804cff0 |
| event_reset_time | 0x0804d120 |
| event_info | 0x0804d170 |
| event_process | 0x0804d190 |
| event_time | 0x0804d370 |
| event_destroy | 0x0804d390 |
| event_count | 0x0804d4a0 |

## Observations preserved in the reconstruction

event-ghidra.json contains the decompilation of all 11 functions. The DWARF
observations in event-metadata.json confirm the one-argument callback returning
long, the 64-byte origin filename, external stack<event*> s_pool_event,
map<int,event_info*> g_event_info, and the static signed-int ID counter.
event-dependencies.txt confirms the original unsigned-long DWORD, heartbeat
offsets, and C versus C++ linkage for the inspected symbols. core-layouts.json
contains the full event member layout.

Allocation zeroes an info block using calloc and registers it under an incremented
ID. Event objects are separately malloc-allocated and recycled through a LIFO pool.
Creation clamps delays to at least one pulse, copies at most 63 filename bytes,
and resets callback-state flags. The info pointer is populated only around callback
execution; event_info() resolves the numeric registry even outside a callback.

Cancelling a pending event frees its info immediately, pools its event object and
leaves a cancelled queue entry. It does not null q_el before pooling. A cancelled
queue entry may therefore refer to an already reused event object; processing must
discard that entry without touching its payload. Cancelling a processing event
sets its force flag; info is freed after the callback returns. A nonzero callback
result after forced cancellation emits the observed EVENT BUG diagnostic.

Resetting an event preserves its info, cancels its old queue entry and adds a new
absolute key. Reset does not clamp the delay and is ignored during a callback.
Repeating callbacks reschedule relative to event_process's pulse argument, which
can differ from the global heartbeat pulse used by creation/reset/time queries.
The process count excludes cancelled entries; event_count includes them.

Shutdown drains queue entries and frees pooled event objects. It does not free
the associated registry blocks or reset the ID counter. This observed behavior
is preserved, including its cleanup limitations, instead of silently replacing
it with the newer ownership model.

## Implementation choices and limits

The C++ map/stack expressions reconstruct the high-level operations inferred from
the decompilation and DWARF; original expression spelling is not recoverable.
EVENTINFO is a convenience alias for the recovered event_info structure. The
EVENTFUNC and event_create macros are supplied for integration; their original
spelling and exact expansion are not established by the inspected metadata.
The inline legacy integer-overflow behavior and standard-library internals are
not a claim of cross-compiler equivalence. Equal-key heap ordering remains
dependent on the library implementation. The original empty-queue sentinel
boundary at pulse INT_MAX has not been changed or validated as a supported input.

libthecore/include/{typedef,heart,main,log}.h are deliberately partial headers.
Only the recovered dependency interfaces needed so far are provided. Actual
heartbeat, logging and main runtime implementations are still required for the
server; no production stub is supplied. FreeEventInfo keeps its observed C++
linkage, while the public event entry points keep their observed C linkage.

## Verification

event_regression.cpp supplies controlled heartbeat/log/fatal hooks and exercises
allocation, lookup, delay clamping, filename truncation, repeated callbacks,
LIFO pool reuse, pending cancellation, pool reuse before tombstone removal,
negative-delay reset, cancellation/reset from callbacks, missing IDs and shutdown
registry retention. It runs only reconstructed code, not the original server.

The available MinGW GCC also compiled abi_layout.cpp with -m32. Its compile-time
checks cover all event/info/heartbeat member offsets, sizes of those types, and
the previously reconstructed queue and FSM sizes. This is a useful structural
check, not a 32-bit FreeBSD runtime integration test.

## Next dependency

heartbeat-ghidra.json retains heart_new, heart_delete, heart_idle, timediff,
timeadd and thecore_sleep observations. The old heart_idle uses the microsecond
remainder after counting missed pulses, computes the next wake time and sleeps
even when pulses were missed. The supplied newer heart.c instead resets last_time
without sleeping on its lag path. Reconstruct this old scheduling behavior before
reusing that dependency. The sleep helper ignores EINTR and logs other select
failures; it does not retry interrupted sleeps.
