# Regression integration handoff

All files are staged in work/stat_primitives/. No production or test Makefile edits and no builds were performed.

Copy these test candidates into reconstruction/tests after the data/helper candidates are integrated:
- stat_primitives_regression.cpp
- stat_primitives_desc_output_syscalls.c
- stat_primitives_abi.cpp
- stat_primitives_unused_input_fixture.cpp (isolated graphs only)

Append Makefile.stat-primitives.inc to the existing test Makefile, or merge equivalent targets deliberately. The fragment reuses DESC_PACKET_SOURCES, including its current real party.cpp plus party_lifecycle_fixture.cpp, and the existing ITEM_SOCKET sources/objects. It adds real char_battle.cpp only when absent from the shared list. RaceToJob comes from the existing real input_login.cpp in DESC_PACKET_SOURCES; it is never relocated or reimplemented by a fixture. Only the specialized syscall/logger object is substituted for desc_output_syscalls.o.

The check and check-abi phony dependencies add the new execution/ABI commands without replacing existing recipes. RUNTIME_TESTS += keeps executable cleaning correct. Do not also add a second invocation to the original check recipe unless removing the phony execution dependency, or the test will run twice.

## Coverage implemented in the staged regression

- Real unsigned RaceToJob, exhaustive input0..255 plus256/257/65535/INT_MAX/highbit/UINT_MAX; success and output mapping0..7, failure and zeroed output for invalid values. The original nonnull ret_job precondition is documented and no null call executes.
- Real CHARACTER constructor and Initialize with the existing quest/lifecycle fixtures; real GetJob, GetName and GetRaceNum. All256 raw job bytes, exactly248 expected diagnostics, plus independent real polymorph/mob race and name-fallback cases. Total251 exact logged failures.
- Exact external GetJob function label, line1569, full original format, returned name, raw zero-extended race and fallback0. The narrow logger accepts only that exact diagnostic; all other diagnostics abort. C++ observation rejects any unarmed/additional accepted diagnostic.
- All eight original positions, negative states and long32 extrema against HP0/positive/negative/int32 extrema. Deadness depends only on position0. Stun tests each individual32-bit flag, high/sign bits and mixtures with/without bit8 across dead/living positions and HP changes.
- Real full CHARACTER_POINT and CHARACTER_POINT_INSTANT byte snapshots prove the tested helpers do not mutate those records.
- Complete six-byte skill record with exact signed int32 time storage, all signed extrema and a literal negative-two byte vector. No host CRT time_t field is used.
- Complete17-byte point packet, golden header17/VID89abcdef/type13/signed amount+value vector, all signed header/amount/value extrema and type255. Member-pointer types enforce int header and long amount/value at compile time. The staged ABI translation unit checks every offset and width.
- No production application method is mocked by the integration regression. Existing full-class fixtures cover only unrelated unfinished lifecycle/virtual boundaries; all four tested helpers and their used application dependencies are actual source.

## Isolated graph input link closure

Adding real GetJob to char.cpp requires RaceToJob from input_login.cpp in every graph containing that char.cpp. input_login.cpp also emits CInputLogin destructor/GetType and its vtable, so isolated graphs without input.cpp need explicit unused-input boundary definitions. Do not truncate input_login.cpp or move RaceToJob merely to avoid those references.

stat_primitives_unused_input_fixture.cpp includes the full actual input.h and defines only an aborting CInputProcessor destructor, Process and CInputLogin::Analyze. These are never instantiated/called in isolated CHARACTER suites. It contains no RaceToJob/GetJob/state implementation. Do not link this file with real input.cpp or desc_fixture.cpp, which already define the corresponding symbols. The full DESC regression uses real input.cpp and existing desc_fixture Analyze boundary instead.

isolated-input-closure.patch is a reviewable patch generated from the current Makefile. It adds ISOLATED_LOGIN_CLOSURE = ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp to dependencies and compile/link source lists of exactly:
- character_creation_regression
- character_initialize_regression
- character_manager_regression
- character_name_regression
- entity_regression
- item_regression

isolated-input-closure.json records the source Makefile SHA-256 and selected target names. If the Makefile changes before integration, rerun stage_isolated_input_closure.py to regenerate the staged patch; the script only reads production and writes workspace output. Review/apply after the current parent build finishes. No fixture is required in a full DESC graph, and no future actual input processor test may use this isolated aborting boundary.

## Validation still required

These candidates have been statically reviewed against current source/fixture recipes and original evidence, but have not been compiled or run. Parent should integrate the complete data/helper files, apply the isolated link closure, run focused regression/ABI and O2, and then the affected full suite. This test does not claim recovery of PointChange/ApplyPoint, original character destruction, input dispatch or a FreeBSD runtime.
