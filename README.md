# Metin2 2009 Server Reconstruction

An ongoing reconstruction of an older Metin2 game-server binary into readable C++ source, guided by binary evidence and comparison with later source releases.

The goal is to recover the original behavior, preserve the corresponding server source-file structure, and work toward a compilable project that can be developed through Visual Studio.

**This is a work in progress, not a complete or production-ready game-server release.** The reconstructed **DB server successfully builds, starts, connects to MySQL 8, and remains running on FreeBSD**, as confirmed by the project maintainer. Full **game-server** compilation, linking, startup, and Visual Studio integration remain unfinished. Native Windows server support is not an immediate goal.

## Scope and approach

The project reconstructs complete function bodies, class declarations, packed records, constants, and ownership behavior. Later source trees provide context; the analyzed binary is the authority when behavior or data differs.

Recovery uses Ghidra analysis, disassembly, available symbols/debug information, static data, and source comparisons. This is behavioral reconstruction rather than recovery of the exact original source text. Original comments, formatting, and some implementation details cannot be recovered reliably.

Evidenced original bugs, unsafe preconditions, callback ordering, and integer-width behavior are retained. Modernizations should be documented separately. Unresolved gameplay logic is not replaced with empty methods, reduced classes, or partial application paths.

## Target

The supplied artifact is identified for this project as the **2009 Metin2 game server**. Its observed target is **32-bit x86 FreeBSD**. The supplied date has not been independently authenticated.

Target SHA-256:

```text
61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c
```

The original executable remains unchanged and is not executed during reconstruction or validation.

## Repository layout

```text
Original Binaries/        Supplied binary artifacts
Reversed Source/
  common/                 Shared declarations and protocol/data records
  game/src/               Reconstructed game-server source
  db/src/                 Reconstructed db-server source
  libgame/                Game support library
  libguild/               GuildImage handler/library
  libpoly/                Polynomial/parser library
  libsql/                 SQL support library
  libthecore/             Core runtime and platform support
  reconstruction/         Evidence, recovery notes, status, tools, and tests
```

Some working candidates and analysis inputs may remain outside the tracked repository. Recovery notes distinguish integrated implementations from staged or unvalidated work.

## Current status

**DB server — operational on FreeBSD/i386 (maintainer-confirmed):** The reconstructed DB executable compiles, links, starts, connects to a MySQL 8 server, and remains running without observed errors. This confirms basic startup and database connectivity, **not** complete protocol, gameplay, or data-integrity validation. The DB build requires C++23, Oracle MySQL 8 client headers/library, fmtlib, zlib, and POSIX threads. Both [GNU Make](Reversed%20Source/db/src/Makefile) and [CMake](Reversed%20Source/db/CMakeLists.txt) configurations are present; see [DB build notes](Reversed%20Source/db/DB_2009_RESUME.md).

As of **2026-11-12**, the last recorded **focused validation checkpoint is 112**, while the [source-restoration journal](Reversed%20Source/reconstruction/RESUME_CHECKPOINT.md) continues through **batch 652**. Later source batches explicitly defer new builds and runtime tests; batch numbers are not validation checkpoints.

- Checkpoint 112 restored CHARACTER shop/exchange setters; two focused runtime suites passed. The registry had 120 suites, with the last reported full run at checkpoint 103.
- Subsequent source batches restore substantial additional shop, character stat, item lifecycle/refinement, affect/combat, quest/Lua, movement/warp, PvP/P2P, guild/party/messenger, DB, and GM command code. These source changes are not yet covered by a new reported full validation run.
- Recent recovered groups include the skill factory, weighted and rare item attributes, attribute copying/removal, affect-flag lookup, and affect/fly packet delivery.
- The function ledger contains **1118 entries with structured address-backed evidence** and **370 bounded implementation/test mappings**, within a 7,238-entry inventory.

These are evidence and validation counts, **not a completion percentage**. Passing module tests does not establish complete server behavior or binary equivalence.

Infrastructure, networking, support libraries, and selected gameplay paths have been reconstructed incrementally. Earlier dependency-frontier descriptions are historical: many of these areas have since received integrated source implementations. Their complete transitive dependencies and current end-to-end runtime validation remain unconfirmed. The DB server now builds and runs, but complete **game-server** linking, startup, and integrated gameplay validation remain outstanding. PC lifecycle, flags/timers, quest storage, manager queries, start-script loading/registration, selected state helpers and constructors, and independently closed safebox operations now have validated implementations. Broader quest execution and populated safebox destruction remain unfinished.

Detailed records:

- [Current status](Reversed%20Source/reconstruction/CURRENT_STATUS.md)
- [Restart checkpoint and next steps](Reversed%20Source/reconstruction/RESUME_CHECKPOINT.md)
- [Remaining dependency frontier](Reversed%20Source/reconstruction/DEPENDENCY_FRONTIER.md)
- [Function coverage ledger](Reversed%20Source/reconstruction/function-coverage-ledger.md)
- [Validation records](Reversed%20Source/reconstruction/validation-status.json)

## Validation

The project includes C/C++ regression programs and a Python validation runner. Their recorded checkpoint results do not automatically validate later source batches. The DB server's successful native FreeBSD startup and MySQL connection are separately maintainer-confirmed. The established test environment uses GCC/G++, GNU Make, C++03/C99 compilation, and 32-bit compiler support for relevant ABI checks. Platform shims and explicit test-only boundaries support tests while unrelated server dependencies remain unfinished.

With the required toolchain and dependencies configured, run the registered suite from the repository root:

```sh
python "Reversed Source/reconstruction/tools/validate.py" run --suite quest_npc_context_regression --abi --make gmake --cc gcc --cxx g++
```

Use `--help` for focused-suite and toolchain options. This validates reconstructed modules; it does not build a complete runnable server. Existing results were obtained in the host test environment, not by executing the original FreeBSD binary. Modern STL layouts and host behavior are not assumed to reproduce the original compiler's ABI.

## Continuing the reconstruction

Start with the restart checkpoint and dependency frontier. Review original evidence before integrating candidates, preserve complete data/API declarations, and test actual application paths. Keep test-only external boundaries explicit and update recovery notes and validation records when implementations are integrated.

Build matching branches are supported by complete static evidence only; runtime tests cover empty/nonmatching branches because the original default-state copy reads indeterminate counters.

The supplied Lua5.0.3 sources support bounded public-API compatibility tests. They are not claimed to be the exact original Lua revision; internal MinGW layouts differ from the original binary.

OpenState is supported by complete static evidence only: original return-storage construction does not establish portable named-return copy elision when default counters are indeterminate. Unsuppressed empty script access remains an excluded original unsafe domain.

Genuine CQM/NPC ownership now replaces the earlier owning destructor test boundary. AArgScript default construction is now recovered; tests initialize scalars before populated copies. Original default-resize unsafe copies and full VM/game dispatch remain outside the verified runtime scope.

Use focused suites for routine changes and run full-suite checks at risk-based batch milestones. Registered suite counts and the latest full result are reported separately; see the reconstruction validation policy.
