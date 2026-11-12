# Current state: complete bounded package staged

Constructor and source-placement proof are complete. See RECOVERY_HANDOFF.md for current candidates, exact semantics, regression/API/ABI gates and remaining broader locale dependencies. No builds or production edits. Original stopped-state notes follow for provenance; their pending steps are superseded.

# Skill power recovery resume

Target game SHA256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c; live Ghidra confirmed matching /game x86 LE32 GCC, 7238 internal functions. Parent requested stop at primary usage92%. No production edits/builds/original ELF execution. No implementation candidates written yet.

## Complete evidence saved
metadata.json: old main.cpp DWARF complete CTableBySkill logical class and singleton base, member offsets/types/access and methods.
class-api.json: exact old declarations/signatures, including return/parameter types and mangled names. extract_metadata.py and extract_api.py reproduce static extraction using work/python-deps pyelftools.
Ten complete decompile/instructions pairs: destructor080a2940/080a29b0, Check080e6820, DeleteAll080e6870, GetSkillPowerByLevelFromType080e68a0, SetSkillPowerByLevelFromType080e68e0, DeleteSkillPowerByLevelFromType080e6960, GetSkillDamageByLevel080e6980, SetSkillDamageByLevelTable080e69a0, DeleteSkillDamageByLevelTable080e6a00. All instruction pages limit500 exhausted, no truncation. Eight method callgraphs saved with both directions limit200 (check has_more before claiming caller exhaustion).

## Verified class/API
Original declaration is game/src/constants.h line193, NOT newer reference skill_power.h. Class40 on target32: public singleton<CTableBySkill> base4 at0 (virtual destructor/vptr), private int* m_aiSkillPowerByLevelFromType[8] at4, private int* m_aiSkillDamageByLevel at36. No STL ABI issue. Existing production common/singleton.h is full template with virtual destructor and static pointer, constructor registration/assert and destructorclear; reuse it rather than inventing a reduced base.
Constructor original header196 inline; destructor203. All eight methods NONCONST in old metadata. Setter arguments int* (NOT newer const int*). Check208, DeleteAll211, powerGet213/powerSet214/powerDelete215, damageGet217/damageSet218/damageDelete219; fields221/222. Newer reference differs in constness and fixes old delete bug; do not copy it wholesale.

## Exact behaviors already proven
Check loops8 slots, first NULL fprintf(stderr,"[NO SETTING SKILL] aiSkillPowerByLevelFromType[%d]",index) without newline and false; allnonnull true. Damage pointer irrelevant.
DeleteAll calls powerDelete indices0..7 then damageDelete.
powerGet: bMob true ignores job/group and uses slot0[level]. Otherwise signed job>3 OR group==0 returns0; else slot[job*2+group-1][level]. Negative job/group and out of range level are NOT guarded; preserve valid indexing preconditions and do not execute invalid memory cases.
powerSet: calls old powerDelete, new int[41] (164 bytes), copies41 signed ints, installs pointer, fprintf(stderr,"AddedSkill[%d][%d]\n",idx/2,idx%2).
CRITICAL old powerDelete bug confirmed by ALL11 instructions: loadslot/test; JNZ skips store; only NULL storesNULL; NO delete[] call, NO nonnull clear. Existing nonnull allocation stays/live and replacement setter leaks old allocation. Preserve exact old behavior, do not repair based on reference.
damageGet: unsigned(Level)<99 then pointer[Level], otherwise0; negative/99+/INT_MIN safe returns0, validlevels0..98 require nonnull pointer.
damageSet: calls damageDelete; new int[99]396bytes; copies99ints; installs pointer.
damageDelete: ifnonnull delete[] thenNULL, otherwise noaction.
Destructor calls DeleteAll then real singleton destructor/assert/reset; deleting destructor additionally scalar operator delete. Implicit copy ctor/assignment present old DWARF; do not invent deletion/copy restriction.

## Still needed before integration
1. Recover inline constructor instructions at main.cpp low_pc decimal134870852 = 0809f744. Main body0809f3d0..080a131d8007bytes; get disassembly page containing0809f744, inspect DWARF high_pc. Confirm base singleton registration and all9NULL initializations before staging inline ctor. Earlier summary assumes newer reference ctor but not yet machine-verified.
2. Confirm old out-of-line method source placement via DWARF concrete subprograms (selected main metadata only; no CU named constants.cpp detected selected extraction). Original class definitely constants.h; likely body constants.cpp but do not state as proven until inspect compilation-unit inventory/low_pc DIE.
3. Stage full class in constants.h and complete eight bodies in original source location, preserving old mutable inputs/nonconst and deletion bug. Include common/singleton.h outside class and stdio/allocation dependencies. Constants JOB_MAX_NUM4,SKILL_MAX_LEVEL40,PLAYER_MAX_LEVEL99 already real in common/length.h. No missing application dependencies beyond existing singleton and C++/C runtime for this class.
4. Meaningful real regression: all256slotpresencepatterns firstmissing diagnostic/order; actual singleton ctor/destructor lifecycle; all8x41 signedpatterns setters/getters/mob routing; invalid job>3 and group0 returnbeforepointer; deletion retained pointer and replacement leak observable using external new[]/delete[] observer (test-only operator replacements, not class stubs); damage99values, extrema, deletioncalls/idempotency; destructor clears singleton but old power pointers remain intentionally allocated. Test-owned cleanup only after proving old behavior, not production fixes. Verify exact varargs types int for diagnostics.
5. ABI strict32 sizeofclass40/offset4+36, memberarrayexact8 int*, publicfullsingletonbase, exactnonconst method pointers with int* setters. Nativehost size differs pointerwidth; no fixed40 assertion on64.
6. Full8-slot tables are runtime configured allocations, not static default tables; config parser populates them. No class setter should silently supplydefaults. Locale TransferDefaultSetting remains broad, no partial locale setter.
