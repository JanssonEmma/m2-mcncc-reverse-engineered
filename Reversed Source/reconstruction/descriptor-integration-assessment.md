# Descriptor integration assessment

Target confirmed through attached Ghidra get_program_info: game, ELF x86 LE32/GCC, SHA256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Static evidence only; original executable not run. Focused provider output is retained in ghidra-focused.json; ProcessOutput and FlushOutput assembly is retained in assembly.json. Reference code guides file placement and names, not behavioral authority.

## Five important call sites

| Caller | Address | Verified contract |
| --- | --- | --- |
| DESC::Setup | 08139660 | Registers descriptor pointer as client data, READ=1, oneshot=0. Allocates output buffer 65536 and input 2048; minimum input growth 1024. Ping event and handshake follow registration. |
| DESC::ProcessOutput | 08139bb0, capacity call 08139bef | fdwatch_get_buffer_size(fdw, socket) is indexed by descriptor, not event index. Nonpositive capacity returns zero. Writes min(capacity, output bytes). socket_write returns zero on success; consume exactly requested length then rearm WRITE=2, oneshot=1 only when bytes remain. Updates max/current/total counters by consumed length. Error result propagates. |
| DESC::FlushOutput | 0813a990, check call 0813aae3 | Polls with 10 ms timeout; only events whose client data equals this descriptor are inspected. WRITE=2 calls ProcessOutput; negative result or ERROR=8 sets phase zero via virtual slot 4. Timeout closes after >10 sec with no write event, or >20 sec after any write event. Calls socket_block and eventually usleep(250000). Does not restore nonblocking mode in observed body. |
| io_loop | 080a1cd0, client retrieval 080a1d37 | Calls TryConnect before poll. For descriptor events, check_event(fdw,d->socket,event_idx) returns READ=1, WRITE=2 or ERROR=8. READ invokes ProcessInput, WRITE ProcessOutput; errors close via phase setter. get_buffer_size calls at 080a1e9f/080a1f2d/080a1faf provide diagnostic values, not event counts. |
| DESC::ProcessInput | 08139900 | Reads into actual buffer writable tail. Negative socket result returns -1, zero returns zero, positive returns bytes read. Processor virtual slot 2 receives this descriptor, current bytes and consumed-byte output. False means consume reported bytes and call again (phase/processor may change); true ends processing and still consumes reported bytes. Encrypted path uses recovered TEA plus TEMP_BUFFER. |

io_loop null-client events are listeners/UDP: tests tcp, p2p then udp with check_event; accepted/read listener events are explicitly cleared. It dispatches CInputUDP::Process for positive UDP reads. Descriptor paths do not directly call clear_event. The unusual Teen descriptor path invokes ProcessOutput twice on a successful write dispatch; preserve until assembly-focused confirmation if implementing io_loop.

## Smallest useful next implementation

Recover exact old DESC declarations/layout from the desc.cpp CU first; do not copy the modern desc.h wholesale or introduce a reduced production class with a different ABI. Then implement DESC::ProcessOutput as the first partial desc.cpp slice and test it against the real buffer/socket/fdwatch sources with deterministic syscall fixtures. This directly connects all three recovered layers and has no packet-decoder dependency. Cover empty output, nonpositive capacity, capacity smaller/larger than output, error propagation, counters, final drain and one-shot rearming.

FlushOutput is the next bounded method, after resolving virtual phase dispatch safely. Its tests must cover filtering another descriptor's event, poll failure, write failure, error event, 10/20-second boundaries, terminal sleep, and retained blocking mode.

Full DESC::Setup/ProcessInput and io_loop require additional unrecovered interfaces. Setup calls locale predicates, ping_event, Handshake, Log and SetPhase. ProcessInput needs CInputProcessor's actual vtable and Process implementation. SetPhase embeds/selects concrete input processors, whose packet metadata is unrecovered. io_loop additionally requires DESC_MANAGER::TryConnect/AcceptDesc/AcceptP2PDesc and CInputUDP. These should remain explicit unresolved dependencies instead of production stubs.

A separate coherent prerequisite module is game/src/packet_info.cpp: 12 method-body addresses (including constructor/destructor ABI variants) from 0813bcf0 through 0813c270, excluding STL helpers/static init. CInputProcessor::Process 080cd110 depends on this metadata path; reconstruct packet_info before full input dispatch. The packet definitions themselves remain to be verified against target DWARF.

## Limits

Callsites and branches above are observed decompilation, with assembly retained for the two output methods. Numeric event meanings are corroborated by branches and setup registrations, not original source enums. Original C++ references may appear as pointers in pseudocode; virtual slot labels require old vtable/layout extraction before source declarations. This assessment adds no production code or tests and does not claim descriptor integration is complete.

Update: the ProcessOutput slice and complete old member layout are now present in desc.cpp/.h, with packed account/player declarations in common/tables.h. The actual method passes unit and real buffer/socket/fdwatch/utils integration tests; desc-output-recovery.md describes the test-only lifecycle fixtures and remaining production methods. Compatible32-bit flags now verify the580-byte layout and all51 member offsets.

