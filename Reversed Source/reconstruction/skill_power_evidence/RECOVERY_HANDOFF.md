# CTableBySkill complete bounded handoff

Target SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Static matching game ELF/DWARF/Ghidra evidence only. No production edits, builds, or original ELF execution by this agent.

## Integration
Insert constants-skill-power-class.inc in original game/src/constants.h before its guard ends; include ../../common/singleton.h outside class. Append constants-skill-power-methods.inc to original game/src/constants.cpp, adding <stdio.h>. Do not move the class into the newer reference skill_power.h: old metadata proves constants.h193 and old methods constants.cpp1461..1551.

The full old class has public real singleton<CTableBySkill> base and private int* m_aiSkillPowerByLevelFromType[8], then int* m_aiSkillDamageByLevel. Target32 base4 at0, slots at4, damage at36, total40. Preserve all fields and implicit copy/assignment behavior; do not add padding or copy restrictions. All eight public methods are nonconst; both setters take mutable int*. These differ from the newer reference.

Constructor constants.h196 is inlined into main0809f744: instructions initialize singleton vptr, assert singletonNULL, register this at0823b490, install derivedvptr, clear8power pointers at this+4, then clear damage at this+36 (store0809f797). DWARF inline high_pc0809f791 excludes the last optimized/scheduled store; saved surrounding instructions prove it independently. Destructor calls DeleteAll then existing virtual singleton destructor; deleting destructor also scalar-deletes this. Reuse existing common/singleton.h.

## Preserved old behaviors
- Check returns false on first missing slot and emits exact stderr diagnostic without newline. Damage pointer is irrelevant.
- powerGet: true bMob always slot0; false returns0 for signed job>3 or skillgroup0. Otherwise uses target32 wrapped job*2+skillgroup-1 and raw skilllevel, with no further bounds guards. Candidate uses unsigned32 arithmetic to preserve the original LEA without signed-overflow UB. Final index0..7 and level0..40 with configured pointer are execution preconditions. Negative job/group can still produce a valid index; tests exercise those and INT_MIN wrapping.
- powerSet copies41 signed ints into new allocation and emits AddedSkill[job][group] newline. It first calls old powerDelete.
- CRITICAL machine-proven old powerDelete080e6960: JNZ skips NULL store; nonnull remains unchanged, no delete[] call. Setter replacement and destructor therefore leak power arrays. Candidate deliberately retains this old behavior. Regression cleans test-owned arrays only AFTER checking real destruction/leak.
- damageGet unsigned-Level<99 reads the configured table; all other signed values return0 before access. damageSet copies99 signed ints and deletes previous damage allocation. damageDelete frees nonnull and clears it.

The eight slots contain runtime-loaded tables, not eight ELF static arrays. Constructor correctly starts allNULL. Config file parsing/loading is broader unrecovered behavior, not a reason to supply invented defaults.

## Dependencies
Closed for this class: existing real common/singleton.h; JOB_MAX_NUM4, SKILL_MAX_LEVEL40, PLAYER_MAX_LEVEL99 already recovered in common/length.h; ordinary fprintf, new[]/delete[], assertions. No application helper, class mock, or locale setter is needed. Entire LocaleService_TransferDefaultSetting is NOT closed: it still needs name/multibyte callbacks, default combat/chain tables and its complete initialization context. See work/exp_locale/RECOVERY_HANDOFF.md. This class provides its actual Check/instance dependency only.

## Regression/API/ABI gates
Copy skill_power_regression.cpp, skill_power_alloc_observer.cpp/.h, skill_power_api_checks.h and skill_power_abi.cpp into reconstruction/tests; integrate Makefile.skill-power.inc. Observer only replaces external C++ array allocation, records fixed-capacity events and delegates to malloc/free. Real fprintf is captured by temporary-file stderr descriptor redirection; no application methods replaced. Constants.cpp links alone with the full existing header/class.

Regression covers all256 presence masks and exact firstmissing diagnostics; full real singleton registration/reset; all8x41 signed patterns including INT_MIN/MAX, copied ownership and mob/normal mapping; guard returns on absent tables; valid unusual negative/wrapped indexes; nonnull deletion/replacement leak; all99 damagevalues, negative/99+/extreme guard returns, replacement/deletion/idempotency/DeleteAll/destructor; real virtual deleting destructor. Array allocations are fully reclaimed by TEST ONLY cleanup after original behavior is verified. Invalid final indices, missing table dereference, invalid levels and aliasing freed damage inputs are documented preconditions, never executed.

API gates use exact method pointers for all eight nonconst signatures and mutable setter arguments. Strict32 ABI gate verifies complete class40/base4/memberoffset4+36, exact8-pointer member type, int4/pointer4 and dimensions41/99. This class has no STL containers. Do not run fixed40 gate as native64. Run normal and O2 regression plus strict32 ABI, then existing full suite when parent integrates. Staging agent ran no build; tests are candidates, not a passing claim.

## Evidence and limits
metadata.json/class-api.json preserve raw DWARF records/types; source-placement.json proves concrete method CUs/files/lines/addresses and inline constructor origin. Ten full decompile/instruction exports are preserved; main-constructor-context-instructions.json is explicitly a partial main page, not complete main recovery. Eight direct method callgraph pages are saved. addressed-evidence.json normalizes ten full exports with original raw hashes, with evidence status separate from implementation/test review. manifest.json hashes package artifacts. Existing RESUME.md is retained as historical pre-completion state; this handoff supersedes its pending constructor/source-placement steps.
