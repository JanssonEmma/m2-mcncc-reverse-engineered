# CHARACTER::Save08128b50: closed immediate dependency recovery

Target verified via Ghidra get_program_info before this pass: game ELF SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.
No original ELF execution, compilation/build, production, status or Makefile edits.

## Full behavior and closure

Original public nonvirtual nonconst signature: void CHARACTER::Save().
Its entire body is:
if (!m_bSkipSave) CHARACTER_MANAGER::instance().DelayedSave(this);

The skip guard occurs before manager singleton access/assert. There is no PC/PID/descriptor check, immediate DB packet, flag reset, or SaveReal call. Save is a delayed-save registration entry, not immediate serialization.
Original direct calls: DelayedSave0818e250 and external __assert0804c9ac (singleton missing). Save symbol bounds are in symbols.json and full27-instruction assembly is saved; no omitted branch.

Actual production CHARACTER_MANAGER::DelayedSave(LPCHARACTER) already exists in game/src/char_manager.cpp:87. Its original body is unique insertion of the pointer into m_set_pkChrForDelayedSave; full15-instruction assembly/decompile/calls saved. STL set insertion is its only direct callee. Duplicate pointers coalesce, distinct objects with equal PID remain distinct, null is not filtered by the manager API. Save does not process or remove queue entries.

Thus the immediate Save closure is genuinely closed against current production CHARACTER_MANAGER and STL. ProcessDelayedSave/FlushDelayedSave/SaveReal are deferred consumers rather than reachable calls from Save. Their broader persistence closure is not recovered here and is not misrepresented as part of this small complete entry method.

Original data/API evidence full-records-and-save-api.json contains full157-member/inheritance CHARACTER3812 and full23-member/inheritance CHARACTER_MANAGER172, plus exact Save/DelayedSave methods and type chains.
m_bSkipSave is protected bool1 at target offset3013 (0xbc5).
m_set_pkChrForDelayedSave is private fullset12 at target offset108 (0x6c).
No fields/classes were reduced or replaced.

## Candidate files

- character-save-body.inc: additive method only.
- character-save.cpp: standalone normal translation unit including full char.h/char_manager.h.
- candidate-char.cpp: current actual production char.cpp verbatim plus full Save method.
- candidate-char.h: complete current production header, adding only public void Save() after destructor.
- current-char-manager.h/.cpp: read-only snapshots of actual dependency, not replacement variants.
- 08128b50 and0818e250 decompile/calls/assembly exports, full-records-and-save-api.json, symbols.json.

For eventual production integration prefer the additive method/declaration over replacing full snapshots if concurrent source work has progressed. No new dependency implementations or lifecycle substitutes are required for this method.

stage_save.py asserts candidate header differs only by Save declaration, candidate source preserves exact current source prefix, and callgraph contains exactly the singleton assertion and actual DelayedSave. Source-only checks passed. Manifest records hashes. This is not a compile/test claim.

## Meaningful regression strategy (staged, not run)

character_save_regression.cpp uses the full actual CHARACTER/manager declarations and real Save/DelayedSave bodies. Reuse established CHARACTER initialization, desc clock, quest lifecycle and virtual/destructor test fixtures already used by character_stat_regression and related targets. Those are explicitly test-only lifecycle boundaries; no test definition of Save, DelayedSave, SaveReal, fake manager, or queue may be introduced.

The test instantiates actual characters; skip=true Save runs before any manager singleton exists, proving guard-before-access. Once the actual manager exists it checks:
- skipped Save leaves the real set empty;
- PID0 and descriptor-null character still inserts;
- repeated calls coalesce by pointer;
- second object with equal PID yields a separate real set entry;
- setting skip after enqueue does not cancel;
- actual manager insertion accepts null without filtering;
- explicit test-side cleanup followed by skip toggle proves the current flag is read on every call;
- byte snapshots verify Save does not mutate the character record.

character_save_abi.cpp checks fullCHARACTER3812/fullmanager172 and exact nonconst void pointer-to-member signatures. Compile target strict32; do not mistake native64 layout for old ABI.

Later integration validation: existing normal/O2/strict32 source profiles plus this actual-set regression. Reuse established source grouping/lifecycle fixture, not application stubs to conceal link failures. Missing-manager unskipped branch can additionally be covered by an existing assertion/death-test harness; do not disable singleton asserts just to link it. A SaveReal DB packet test is outside Save's callgraph and remains separate.

This recovers one genuine PointChange dependency at original callsite0812f14c and quest EndRunning callsite0805f569. It does not claim the remaining PointChange/quest SCC is complete.
