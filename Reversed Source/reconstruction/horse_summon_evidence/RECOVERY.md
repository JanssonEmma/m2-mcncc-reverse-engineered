# HorseSummon and closed horse/movement package

FROZEN for parent review. No production, global fixture, Makefile, build, runtime test or original ELF execution changes were made. Source-only audits are reported separately from unrun regressions.

Target: original game SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c, reconfirmed through Ghidra. Original x86 little-endian GCC. Full366-instruction HorseSummon assembly retained. Full records/API and symbol/source-placement evidence retained; no reduced class definitions or fake gameplay callees.

## Concrete independently closed package

After adding genuine declarations/records, integrate ONLY these original-source fragments:
- char.cpp-closed-additions.inc -> existing game/src/char.cpp: SetRotation0812d390, SetEmpire0812a660, global EncodeMovePacket081282a0, SendMovePacket0812dde0.
- char_battle.cpp-closed-additions.inc -> existing game/src/char_battle.cpp: GetVictim08153f20.
- char_horse.cpp-closed-additions.inc -> game/src/char_horse.cpp: GetMyHorseVnum0816df10, ClearHorseInfo0816e000, SendHorseInfo0816e060, SetRider0816d8c0.
- horse-summon-detail.h -> game/src/horse-summon-detail.h, C++03 modular32 and x87-style truncation helpers.
- guild-api-additions.inc adds original public nonconst DWORD GetMasterPID() field accessor; exact DWARF link/type corroborated.
- Relevant char-api-additions.inc methods and full packed packet-record-additions.inc (dead5/move24). That API file ALSO includes genuinely pending broad HorseSummon/Show/Goto/etc declarations; they may remain declarations without fake bodies.
- EncodeMovePacket global prototype is in encode-move-api.inc if a header declaration is desired; original declaration/definition was char.cpp.

Combined closed-horse-methods.cpp/closed-move-packet.cpp are readable staging variants, NOT extra objects to link alongside split source fragments. source-placement.json proves original translation units/lines. Preserve placement rather than making all bodies belong to char_horse.cpp.

Actual closure: existing ChatPacket, real DESC/temp-buffer/output path, CHorseRider max-health/max-stamina methods and c_aHorseStat; actual CHARACTER_MANAGER::Find(const VID&) with CRC validation; actual get_dword_time; actual CEntity::PacketView. These dependencies were inspected in current production. Underlying CHARACTER/quest/guild lifecycle remains explicit TEST ONLY in focused tests, never a production-completion claim.

## Exact closed behaviors

SetRotation is a rawfloat store, no angle wrapping. SetEmpire rawBYTE store. GetVictim calls CRC-aware Find(VID), not VID-number-only lookup.
GetMyHorseVnum is c_aHorseStat[level].iNPCRace +0(no guild),+1(member),+2(master whose PID equals character PID), unsigned32 sum; no level/horse/PID0 guard.
ClearHorseInfo: not riding=>actual command5 hide_horse_state then reset3cache bytes; riding=>no command/cache change. m_chHorse always becomesnull.
SendHorseInfo runs only when horse pointer or riding is nonzero. Health grade0 iff health==0, otherwise1 then strict30%/70% thresholds yield2/3. Stamina grade0 through10%, then1 through30%,2 through70%,3 above70%, all strict comparisons. Negative short values retain original behavior. Changed level/grades cause real command5 horse_state, then reread level for cache and store computed grades. Cache updates even if ChatPacket has no descriptor and returns early.
SetRider caches old pointer and virtually clears old rider, assigns new pointer, virtually sends new rider info; same-pointer replacement is NOT a no-op.
EncodeMovePacket writes full old packet24 header3; time0 calls actual get_dword_time; explicit time retained. Coordinates preserve32-bit signed wire bits.
SendMovePacket function0 overrides x/y/duration from current destination/move duration. Rotation-1 truncates float towardzero, then signed division5 and BYTE conversion. Other rotations are explicit rawBYTE conversion. PacketView uses except=this, so recipients get packet while owner is excluded; real sector/observer rules apply.

## Indirect-call proof

Raw old _ZTV9CHARACTER addresspoint08226d08, first2words metadata excluded:
- HorseSummon0816da97 CALL[vptr+0x38], slot14 -> GetMyHorseVnum0816df10.
- SetRider0816d8eb slot18 -> ClearHorseInfo0816e000.
- SetRider0816d8fd slot17 -> SendHorseInfo0816e060.
- Show slot2 -> EncodeInsertPacket08128300; this remains an actual unresolved CHARACTER insert-packet body/test boundary in the broader graph.
virtual-call-resolution.json and original parent vtables.json retain exact entries/symbol identities. Source uses genuine virtual calls, not a direct base shortcut.

## Regression/Makefile handoff

horse_methods_regression.cpp is STAGED, NOT compiled/run. It uses actual full classes and all newly recovered methods, real ChatPacket/DESC/PacketView/map/clock paths.
Cases: rawfloat bit patterns(-0/NaN/infinity), every horse level0..30 and guild/member/master delta, high masterPID, CRC victim match/mismatch/absence; real command packets at exact10/30/70% boundaries including negatives; unchanged-cache suppression, no-descriptor cache update, hide/cache behavior while riding/not riding, old-clear/new-send and same-pointer SetRider; exact golden24 bytes, high signed coordinate bits, explicit/default clock, default movement fields/negative rotationBYTE, view-recipient delivery/owner exclusion/observer suppression.
horse_methods_abi.cpp uses C++03 typed member pointers and fixed scalar/packed record offsets/sizes only. No C++11 type_traits/static_assert or obsolete whole STL class-size checks.
Makefile.horse-methods.inc gives concrete focused source/object grouping, check/ABI/clean additions for parent review; not applied here.

horse_methods_character_fixture.cpp is a DEDICATED copy of the current explicit test lifecycle/virtual boundary file. It removes ONLY abort definitions of GetMyHorseVnum/SendHorseInfo/ClearHorseInfo because actual methods now link. Other unused abort boundaries remain explicit. Do not silently edit global character_fixture.cpp or substitute a reduced CHARACTER. Use existing TEST ONLY CGuild lifecycle fixture unchanged. Base desc_output_syscalls observer replaces target_walk observer requiring unrelated callbacks. Existing heartbeat definition is reused; no duplicate. Test defines the required transport counters.
This focused suite does not exercise events/spawning/Show/affect state. The default unused event fixture may remain; do not claim real event coverage for this leaf target. No application implementation being tested is replaced by an observer.

## Whole broader candidate (NOT independently closed)

candidate-horse-summon.cpp is the complete original2-bool HorseSummon plus actual horse_dead_event. Retains all summon/despawn branches, random call order/line numbers, map/z/rotation capture before virtual race getter, actual SpawnMob/Show/DestroyCharacter, raw Korean locale keys, SetLevel/name suffix, real60second dead-event scheduling, owner PacketAround dead5, SetRider, far-away movement and unconditional SendMovePacket after Goto.
Source does NOT add modern pet/customname APIs or null guards absent from old callback. Despawn relies on real virtual ClearHorseInfo for owner horse-pointer clearing; no extra shortcut writes.
strings-floats.json preserves original CP949 bytes/suffix/error/move-log keys and180float bytes. Code octal escapes are exact. x87 FNSTCW/OR0xc00/FLDCW/FISTP is truncation, not nearest ROUND; helper preserves integer-indefinite INTMIN for NaN/out-of-range. Horse far move sums original integer coordinate+float delta in extended precision, not prematurely converting coordinate tofloat.
The failed-show _sys_err has only function,line,format stores in assembly; decompiler's extra y is stale and not fabricated as an argument.

candidate-spawn-mob.cpp fully translates SpawnMob0818d010: original type1/3 attribute bypass, sector absence, attr0x81 blocked/static log flag+time refresh, attr4 banPK, real CreateCharacter/SetProto, empire assignment, random rotation, optional Show/destruction. No path is trimmed merely because HorseSummon supplies show=false.
candidate-show-goto.cpp fully translates Show08129ae0 and Goto0812d810, preserving sector migration/viewcleanup, actual virtual insertion, player stamina-affect trigger, mob last-attacked position, spawn flags, destination fields, original state-machine/duration/victim/chat/log behavior.
candidate-spatial-lookups.cpp contains complete map GetMap/Get overloads/SECTREE_MAP::Find/GetEmpireFromMapIndex. Methods are STL/data-closed, but spatial-manager/map lifecycle is NOT recovered; do not claim a complete spatial subsystem.

candidate-sectree_manager.h preserves complete old SECTREE_MAP inheritance from std::map (new reference uses composition and must NOT replace it), full setting, and complete52-byte-target manager data with all4containers, region/area/NPC records and ALL recovered original method declarations. All declarations are metadata-backed, without invented bodies. Other candidate-*.h preserve full current records and add genuine APIs/packets only.
No spatial class ctor/destructor/map-loading no-op has been added to manufacture a green runtime.

## Genuine remaining broader closure

SetProto0812ac60 remains evidence-only: full decompile/calls retained, including many Ghidra removed-unreachable-block warnings. Do not claim a complete compiled translation without branch/assembly review. It reaches ComputePoints SCC, AssignTriggers, ApplyMobAttribute, DetermineDropMetinStone, warp/ore events and StateHorse/state bindings. No horse-only stripped SetProto.
Show reaches actual missing StartAffectEvent/affect_event and missing original EncodeInsertPacket. Goto reaches actual missing MonsterChat/IsGuardNPC/script-string data plus real existing FSM/duration. Horse dead callback reenters full HorseSummon. Spawn/destruction ultimately require actual CHARACTER lifecycle, not the focused test destructor.
Spatial manager/map lifecycle/loaders remain missing. Existing base horse lifecycle/stamina events are ALREADY actual production; never list them as missing bodies again.
Other agent owns AutoGive/CreateItem; parent owns GetTable/RealNumber integration. No overlap edits or new stubs were made.

## Validation status

Complete returned JSON, callgraphs, full raw record/API/source-placement evidence and complete relevant assembly pages are saved. Artifact/source-only checks and hashes are in manifest.json. NO build, ABI compile, runtime test or original executable run was performed. Parent should review concrete package, integrate focused bodies in original files, run targeted normal/O2/strict32 gates, then full required checks before broader SCC promotion.
