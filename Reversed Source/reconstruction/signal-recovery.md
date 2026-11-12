# signal.c recovery

Target: game, SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c, x86 LE32 GCC ELF. Ghidra get_program_info confirmed identity before analysis. No original server execution or Ghidra mutation performed.

All seven addressed signal.c functions in functions.csv were decompiled, without truncation; raw provider results are in signal-ghidra.json (this Ghidra connector does not issue retained Evidence IDs). Addresses: reap 081b7de0, checkpointing 081b7e30, hupsig 081b7e70, usrsig 081b7eb0, signal_timer_disable 081b7ed0, signal_timer_enable 081b7f10, signal_setup 081b7f50. Setup disassembly independently confirms timer-before-handler order and all eight signal registrations.

Observed behavior: reap drains positive waitpid(-1,NULL,WNOHANG) results, stops on zero or negative, then re-registers itself. Checkpoint clears any nonzero tics (including negative); zero logs original function/line28 diagnostic then aborts. Shutdown writes shutdowned=1 before logging original line39 diagnostic. User signal calls core_dump_unix with literal signal.c and line44. Timer operations set both interval and initial value, zero microseconds, ignore syscall failure, and accept unvalidated integer seconds. Setup arms 30-second ITIMER_VIRTUAL before registering checkpoint, shutdown, child, ignore and user handlers in original order. FreeBSD signal numbers are 26,1,20,2,15,13,14,30; production uses symbolic platform constants.

Public signal.h retains the reference's three declarations; handlers remain externally linked implementation functions as in target. No fabricated Windows no-op branch is added. Core dump dependency is real and deliberately unresolved at linkage until its original implementation is reconstructed; signature evidence identifies WORD line, reference gives unsigned short WORD and const char* who. Parent must reconcile exact DWARF typedef/constness with shared utils.h.

Validation: host MinGW GCC 13.2.0 C99 strict compile (-Wall -Wextra -Werror -pedantic) and signal_regression.exe passed. Test fixture intercepts all signal, setitimer, waitpid, abort, logging and core-dump calls; no real handlers/timers/process termination are invoked. Checks cover exact registration order and ignore pointers, timer-before-handler ordering, all timer fields and zero/negative/positive seconds, ignored timer errors, tics positive/negative/zero, original diagnostics and log order, core dump provenance, waitpid drain/zero/error and child re-registration. An i386 object compile with -m32 also passed using a relative output path from the staging directory, after absolute/working-directory-prefixed output paths failed in the assembler. This checks syntax and target compilation only; no i386 or FreeBSD runtime test is claimed.

Run from reconstruction/tests after installing:
  gcc -std=c99 -Wall -Wextra -Werror -pedantic -I signal_shim -include signal_hooks.h ../../libthecore/src/signal.c signal_regression.c -o signal_regression.exe
  ./signal_regression.exe

Remaining limits: static binary evidence establishes control/data flow, not actual asynchronous scheduling, FreeBSD signal restart semantics, kernel timer behavior or binary equivalence. Full server integration still pending. Current source paths and line layout differ intentionally while emitted diagnostic provenance stays exact.


Dependency resolved: core_dump_unix is now implemented in utils.c, and the shared utils.h declaration is DWARF-verified as const char*, WORD. signal.c includes that shared header. The utils_regression suite links the real signal and utility modules and verifies usrsig-to-core-dump behavior with all process calls intercepted. See utils-recovery.md.

