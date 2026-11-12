# Complete LocaleService_TransferDefaultSetting handoff

Target game SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c; original function08068580, locale_service.cpp970, exact void() / _Z36LocaleService_TransferDefaultSettingv. Original full decompile,69 instructions and exhausted4-callee graph preserved. Parent confirmed Banword focused5+ABI passing; all direct application/data dependencies now actual production.

## Complete source
locale-transfer-default.inc preserves this entire original order:
1. Set check_name_euckr, is_twobyte_euckr and exp_table_euckr only for NULL early globals.
2. For job0..7, call actual CTableBySkill::instance().Check() EVERY iteration. On false, emit original no-newline %d job diagnostic and actual std::exit(1).
3. After all8 succeed, fill boss, normal and chain defaults in original order only for NULL late globals.

No callback is invoked, no table is fabricated, no one-time guard is added, no Check is collapsed and no name/path/service-specific initializer is supplied. Missing singleton retains existing real instance() assertion behavior. Required includes constants.h and cstdlib are added explicitly; existing Cstdio/config/helper declarations already present. Data and callbacks are complete previous recoveries.

## Read-only compiler instrumentation
The dedicated regression compiles COMPLETE actual constants.cpp into locale_transfer_constants_instrumented.o with -finstrument-functions and -fno-lto. Only this TEST object gets instrumentation; production source/class/signatures/flags remain unchanged. The regular constants.cpp source is removed exactly once from the new target's shared source list so there is one definition.

The C observer __cyg_profile_func_enter/exit and counter helpers are no_instrument_function. It counts only actual CTableBySkill::Check entries, calls an annotated observational callback and ignores other functions. Hooks never mutate application fields, global defaults, table bytes or returns. Observational table pointer is const. Existing unrelated shared syscall/lifecycle/clock/input-dispatch boundaries remain; no new appmethod mock.

Address identification byte-copies the first word of a nonvirtual member-function pointer under the existing GCC x86 ABI. This is TEST-only address matching and never calls a wrongly typed function. The regression requires8 observed real entries; wrong identity cannot silently pass. This instrumentation strategy is explicitly parent-approved. It observes conceptual function entries, not instruction/branch coverage.

## Concrete tests
locale_transfer_regression.cpp:
- All64 combinations of NULL/preset early and late pointers, each followed by another call without resetting them. Every128 successcall observes8 actual Check entries (1024 total). At every entry, the three early defaults must already be correct and the three late globals must still equal pre-call values. After return, late defaults/preserved values are exact.
- Actual full CTableBySkill setup uses real setters for8x41 power values and99 damage values. Table pointers/bytes and unrelated service strings, enum, encoding, locale flags, PK level, auth/debug state remain unchanged.
- Eight child inputs omit a different power slot BEFORE calling the function. Real Check prints that missing slot; the outer loop prints job0 because its first full Check failed. Exact concatenated no-newline diagnostics are verified.
- Child executes REAL std::exit(1). An atexit observer only reads application state/counters and writes an external witness: earlydefaults set, late3 stillNULL, table state unchanged and exactly1 real Check entry. It returns to actual exit1; no exit replacement/status override. Parent verifies status1, diagnostic bytes and witness before removing fixtures.
- Windows _spawnl applicationfilename remains unquoted, argv0 is quoted for paths with spaces. POSIX uses actual exec/wait. Only reconstructed regression executable runs; never original ELF.
- Parent success cleanup frees known old leaked power arrays only AFTER real class destruction. Child process exits normally through original exit path; no destructor unwinding or invented leak repair.

Later outer-loop failures after a prior success are not forced: deterministic Check sees unchanged tables, and hook mutation is prohibited. Complete original loop/counter/error body is retained in source/instructions. No all-branch recovery claim.

locale_transfer_abi.cpp checks exact void() API, actual nonconst bool Check(), all mutable global pointer/callback types,32-bit int/pointer and eight-slot count. It adds no fields or class-size assumptions.

## Guarded parent apply and canonical Makefile
Review parent_apply/plan.json and six full desired files in payload. prepare_apply.py only reads F and renders workspace files. apply_candidates.py dry-run verifies ALL baseline/payload hashes; --apply is parent-only and backs up existing targets before per-file atomic replacement. Dry-run passed; no production writes/builds here.

Parent may run python work/locale_transfer/apply_candidates.py --apply after review using existing F authorization. Baseline mismatch refuses rather than overwrites. Guarded files: locale_service.cpp/h, three new test files and Makefile.

The guard prepends locale_transfer_regression to the FIRST canonical RUNTIME_TESTS assignment and adds its command to MAIN check recipe. There is NO runtime += and no extra global check dependency that reruns it. Standalone check-locale-transfer and check-locale-transfer-abi targets remain available for focused validation. The instrumentation object and specialized C observer are TEST-only and cleaned explicitly.

New focused graph uses actual current TARGET_WALKING sources/objects, filters only regular constants.cpp and target_walk observer, then inserts instrumented actual constants object and new external observer. No LTO/GC elimination flags on the dedicated link; object-specific -fno-lto ensures code identity. Existing legacy scoped -O2 -flto strategy remains untouched.

When validating O2, force rebuild locale_transfer_constants_instrumented.o and observer as normal Make does not track flag changes. Parent owns focusednormal, O2, strict32 ABI and appropriate fullsuite. These staged tests have NOT run here.

## Frontier
This is a complete transfer setter, not an EXP-only shortcut. No service-specific initialization added. Broader full service startup remains pending quest/config globals and initialization provenance, full slot-name literal matrix, dispatch and initializer integration. Full2009 reconstruction remains unfinished. See manifest/audit for hashes and RESUME for current state.
