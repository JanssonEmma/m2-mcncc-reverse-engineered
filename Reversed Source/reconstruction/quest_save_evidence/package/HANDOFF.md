# Complete PC::Save preparation (uncompiled)

Target SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c confirmed live by Ghidra get_program_info before targeted read-only disassembly. Saved complete280 instructions (has_more false), existing complete addressed decompile and20-edge callgraph. No ELF execution, F mutation, build, runtime or ABI claim.

## Payload and guards

questpc.cpp.fragment contains the WHOLE PC::Save08060770. questpc.cpp.candidate is a full additive source file based on current quest-info-applied baseline. tables.h.fragment/candidate add the entire original common/tables.h566-571 record SQuestTable/TQuestTable. MANIFEST.json provides exact before/after hashes and five read-only dependency guards. Check before copying; do not force stale baseline or re-run prepare.py as a silent review substitute. Declaration PC::Save already exists in the complete PC header; no owner narrowing/API changes.

The original record DWARF at questpc.cpp CU DIE0x1c8413 is size106 with DWORD unsigned-long32@0, char[33] szName@4, char[65] szState@37 and signed-long32 lValue@102. TQuestTable typedef DIE0x1c84bb. This differs from a guessed32/64array layout. Candidate uses original DWORD/long types and packing1, no host-port replacement. Target32 checks required. Methods like operator=/constructors in DWARF are compiler implicit, no custom constructor body added.

## Instruction review / ordering

- Empty m_FlagSaveMap returns0806078d before touching function-static s_table guard. First nonempty call initializes actual vector and registers its destructor08060bd9-08060bfd. Candidate native function-static std::vector retains standard complete lifetime; no fake singleton.
- Every nonempty call constructs an all-zero106-byte TQuestTable temporary080607a0-080607b6, then std::vector::resize(old map.size), with full-zero fill only for newly grown rows. Existing rows survive resize unchanged. Candidate C++03 resize's default T() argument expresses the actual value-init. Existing unused bytes retain prior row contents; do not memset each accepted row or use fresh vector.
- While map iterator is live: retain const key reference, snapshot signed int value into long, increment iterator BEFORE find or any log08060830-0806083f. First dot find returns32-bit size_t and is assigned signed int08060866-0806086f. Negative results log line537 and continue. Copy prefix and suffix using full string.length(), not C-string split; embedded NUL remains part of length/find until eventual strcpy.
- Empty prefix OR suffix log549 invalid-data string; no row/count increment. Valid nonempty fields log exact original `%ld` format BEFORE length validation08060ac6. Later reference's `%d` and strlcpy differ; candidate uses binary `%ld` and strcpy.
- Original name accepted length<=31, state<=63 (NOT physical buffers33/65): bounds08060ad4/08060ae3. Name overflow log557 takes precedence; state overflow563. No truncation or repaired thresholds.
- Only accepted row obtains s_table[i++] after diagnostics. It reloads current m_dwID after callbacks08060b29, then strcpy(name), strcpy(state), cached lValue. Reentry can therefore change PID, values are already captured, current key remains borrowed/live and iterator remains advanced. No snapshots/copies added.
- If accepted count>0: log QuestPC::Save count; DBPacketHeader numeric11/handle0/count*106 at080609d0. Then RELOAD db_clientdesc and static vector BASE after header callback080609d9/080609e2 before actual CLIENT_DESC::Packet. Candidate uses two separate original calls, not DBPacket helper. Current global pointer can change between calls.
- Entire LIVE m_FlagSaveMap clears only AFTER actual Packet returns080609ef-08060a41, even flags inserted by final-log/header/body callbacks. If no valid rows, still clear the live map; no DB call. Original m_FlagMap is not cleared. No early clear, nullDB guard, retry or recovery semantics.

## Preserved unsafe/reentrant domains

Map-growing callback may create more accepted entries than initial resized vector and overrun it; map-clear/erase callback can invalidate current borrowed key/next iterator; reentrant Save shares/resizes static vector and can invalidate pointer/row. Huge size/find/count conversions and dangling/null DB context retain original risks. No concurrency safety added. Native std::string SDK differences do not prove old COW implementation equivalence. Tests should avoid forced undefined behavior and explicitly document it. No field memset, C-string semantic correction, range clamp, DB fallback or valid-only narrowing.

## Actual dependency closure

Existing CLIENT_DESC::DBPacketHeader0813fb40 and Packet0813fc70 in game/src/desc_client.cpp are real. Existing descriptor buffers/network/event graph and PC ctor/dtor/flag methods can be linked. No Reward/EndRunning/PointChange/Lua call occurs in Save. Diagnostics may be scripted external boundaries in tests, but do not replace either actual DB descriptor method or create a shadow PC/table.

## Required next validation (not performed)

1. Independent full-source review, fresh source/dependency guards and inspect diffs; add canonical one suite only. ABI checks sizeof106, offsets0/4/37/102 and exact array33/65 plus sizeof(DWORD)==sizeof(long)==4 under -m32.
2. Actual full PC lifecycle/flags with actual CLIENT_DESC DB header/body and buffers. Empty map no allocation/log/packet; valid sorted flags; invalid no-dot/empty-prefix/suffix; first-dot split incl multi-dot; lengths31/32 and63/64; embeddedNUL length-vs-strcpy; values0,negative,INTMIN/MAX, high PID; mixed invalid rows compact output; allinvalid no packet but pendingmap cleared. Inspect actual9-byte DB header numeric11/zero handle/payload size and complete106-byte rows.
3. Reused vector rows: first long accepted names/states then shorter subsequent save, verify stale tail bytes across calls/PC instances. Fresh-grown rows zero except stores; resize shrink/regrow distinguishes preserved prefix and zero-grown suffix. Keep native SDK/lifetime limitations clear.
4. Diagnostic callbacks may change PID or future pending value; prove value cached pre-log and PID reread after. Safe insertion before current iterator can be cleared without traversal; pending additions in final log/header/body callback cleared after body. Header callback may swap actual DB descriptor: header first, body second actual instance. Reentry can be tested only bounded safe configurations; do not overrun vector/invalidate iterators.
5. Focused actual DB/PC +ABI, strict32 actualsource/test/ABI, optimized actualgraph with distinct SQL/observer objects and real events, then full canonical104+ABI if exactly one new suite; actual receipts/terminalexit only. Do not record compiled recovery from this prepared package.

No original named QUEST_* enum was established here; candidate uses proven numeric11/32/64 without inventing public enum. Full table sizes/names/membertypes are original DWARF evidence.

## Regression/integration preparation appended (still UNCOMPILED)

pc_quest_save_regression.cpp, pc_quest_save_abi.cpp and pc_quest_save_desc_output_syscalls.c now staged. The normal graph uses existing TARGET_WALKING_SOURCES and real-events switch, actual CLIENT_DESC::DBPacketHeader/Packet +buffer_manager_core actual buffer object, five existing SQL/observer object slots. Existing desc_fixture CLIENT_DESC::Destroy/SetPhase boundary is TEST lifecycle only; production output methods are the real methods. Save diagnostics use explicit C observer wrappers; unrelated OS/errors retain existing fail-fast fixture. No shadow app class or wire-layout replacement.

Runtime test checks full wireheader and fields; empty/allinvalid/mixed flags, limits and zero/signed extrema, embeddedNUL, staticvector stale tails acrossPCinstances and zero-grown rows, cachedvalue/currentPID/futuremapvalue mutations, final-log/header/body additions lost on late LIVE mapclear, and globalDB swap between actual header/body. Borrowed source is copied into owned BYTE vector; row interpretation uses memcpy into actual TQuestTable to avoid byte-storage object-lifetime aliasing. Undefined iterator invalidation/vector-overflow/reentrantSave domains remain excluded and documented.

Original long/DWORD stay unchanged. Canonical Strawberry MinGW Windows is LLP64: long/DWORD4 despite pointer8. Runtime asserts both4 and fullrecord106; strict -m32 asserts original offsets/arrays. LP64 hosts intentionally reject, rather than substituting a fake signed-int field or making a native productionport claim. No native type probe/build was run by this agent.

prepare_integration.py verifies reviewed candidate source SHA and produces integration/plan.json/payload with6 paths:two original sourceowners,three testfiles,Makefile. Registry/ordered canonical check104 has the new suite exactlyonce. apply.py default successfully validated all6paths+5dependencies without mutation. --apply is PARENT ONLY after all current builds stop and finalreview; never force stale guards.

validate.py is PARENT ONLY and unexecuted: normal forced build/runtime/ABI, strict32 actualsource/regression/ABI, five distinct optimizedSQL/observer objects, fullactualgraph optimizedbuild/runtime. It writes9pass/failure receipts if allstages run. Dedicated buffer_manager_core.o is actualbuffer.c but retains existing normal object; O2 claim applies appgraphC++ +fiveSQL/observer objects, not entireSDK/runtime. No LTO requested. Root canonical validationdriver should run focused suite first if more detailed execution receipts desired; do not overlap builds.

Commands from currentworkspace, AFTER independentreview and no activebuild:

    PY work/pc_quest_save/apply.py
    PY work/pc_quest_save/apply.py --apply
    PY work/pc_quest_save/validate.py --root "PROJECT_ROOT\Reversed Source" --out "PROJECT_ROOT\Reversed Source\reconstruction\tests\pc_quest_save_validation"

Then root canonical full104+ABI with fresh unique log, exactorderedexecution and terminalexit. Gate allcoverage/status changes on actualpasses. Preparation did not execute anymake/compiler/originalELF. Static PythonAST and source/payload/dependencyguards passed; C++ compile and runtime remainpending. Weeklyallowance95% perparent: STOP expanding here and resume with actual focusedcompilefixes after allowance available.
