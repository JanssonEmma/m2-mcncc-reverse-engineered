# 2009 game reconstruction

Current module and validation status is maintained in CURRENT_STATUS.md.
The original inventory and initial recovery history below remain useful context.

Target SHA-256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c

Target: 32-bit x86 ELF. DWARF identifies GCC 3.3.3 [FreeBSD] 20031106.
The supplied 2009 date has not been independently authenticated.
Native Windows compatibility is deferred. Existing newer sources are unchanged.

## Inventory and placement

- source-map.json: all 174 compilation units and raw filename candidates (152
  unique filename candidates, 9 ambiguous, 13 unmatched).
- file-placement.json: proposed paths and the basis for each choice. Provisional
  unless checked per module. event.c belongs to the game event system despite a
  filename match in newer libthecore. Old event.c and event_queue.c become
  game/src/event.cpp and event_queue.cpp following the newer source convention.
- dwarf-inventory.json: 6,503 addressed subprogram records with 5,952 unique start
  addresses, across all 174 units. This differs from Ghidra's 7,238 functions:
  DWARF and disassembler inventories differ in coverage and duplicate rules.
  Also includes 216 selected layout observations and 26,923 type nodes from five
  focused units. The type-node index is incomplete metadata: array dimensions,
  all function-type parameters and full inheritance metadata are not preserved.
- functions.csv: searchable function addresses, names, declaration files and
  binary-local DIE identifiers. Compilation-unit ownership is recorded separately
  from declaration files, which may be included headers.
- core-layouts.json: focused member offsets, sizes and method declarations.
- tools/: read-only Python extractors requiring pyelftools 0.33. Binary paths are
  explicit in each script. Redirect stdout to save their output.

No-low_pc declarations and inline-only records are excluded from the addressed
list. Duplicate observations are deliberately retained. These inventories do not
prove completeness of recovered source code.

## Reconstructed source

FSM.cpp: both constructor variants, Update, IsState and GotoState compared with
Ghidra. Update retains the old binary's unconditional current-state dereference
on transition; the newer source added a null check.

FSM.h and state.h: reference-derived declarations checked against target member
layouts and vtables, destructor behavior, empty initial callbacks and callback
dispatch. CState's abstract methods are confirmed by __cxa_pure_virtual entries;
DWARF virtuality alone was insufficient. The inline Set routine's exact assertion
diagnostics are not independently reconstructed. Supporting records are retained
in FSM-ghidra.json, state-ghidra.json, core-extra-ghidra.json and fsm-vtables.json.

event_queue.cpp and event_queue.h: old std::priority_queue, absolute-key
Enqueue(void*, int), raw payload pointers and 12-byte SQueueElement. The newer
queue adds iStartTime, different enqueue arguments and stable_priority_queue.
The old queue does not filter cancellation flags or own payloads. Requeue is a
DWARF declaration only: no emitted implementation has been found. Equal-key
ordering across different standard library versions is not proven identical.

Target 32-bit GCC sizes: CState=4, CStateTemplate<CFSM>=32, CFSM=44,
SQueueElement=12, CEventQueue=16. See core-layouts.json and event-queue-ghidra.json.

## Validation

C++03 syntax checks and compiled regression tests passed using the available
x86_64 MinGW GCC 13.2.0 host compiler with -Wall -Wextra -pedantic. Tests cover
deferred transitions, callback order, restarting the current state, transitions
requested during begin callbacks, queue ordering, cancellation flags, empty
queues, duplicate keys and entry versus payload ownership.

Run make check from reconstruction/tests on a Unix-like host. The Makefile
accepts CXX, CPPFLAGS, CXXFLAGS and LDFLAGS overrides. Keep assertions enabled.
An i386 GNU build also checks target type sizes and queue-entry offsets.
The separate abi_layout.cpp compile-time checks have passed with -m32; no i386
runtime executable has been run. This subset test build is not the server build
system or proof of binary equivalence. Run make check-abi for the structural checks.

## Event subsystem added

game/src/event.cpp and event.h now reconstruct all 11 event/registry functions.
The old raw pointers, one-argument callbacks, numeric info registry, event pool,
cancellation tombstones and shutdown cleanup behavior are preserved. Partial
libthecore dependency headers are included. See event-recovery.md for evidence,
implementation limits and the regression scenarios.

The event regression executable passed on the host compiler. In addition,
abi_layout.cpp passed a 32-bit GNU compile-time check, covering the recovered
sizes and event/info/heartbeat member offsets. No FreeBSD runtime build has been
performed. The real libthecore logging, heartbeat and fatal-error dependencies
are not replaced with test hooks in production code.

## Remaining work

The full server is not reconstructed or buildable yet. Heartbeat, timing, main,
logging, ciphers, optimized copying, signals and selected libgame modules have
since been recovered; see CURRENT_STATUS.md. Continue resolving larger game
systems, remaining dependencies, source assignments and packet layouts. Target
integration verification still needs a 32-bit FreeBSD-compatible runtime.
