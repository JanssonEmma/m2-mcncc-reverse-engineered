# Mount/polymorph/ComputePoints recovery

Target game ELF SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c reconfirmed via Ghidra before this pass.
All files are staging/evidence only. No builds, tests, production/status/Makefile edits or original ELF execution.

## Complete source candidate

candidate-mount-polymorph.cpp translates full SetPolymorph08135b10, MountVnum08136010, ComputePoints0812c520, plus full real RefreshAffect0813e780, ComputeAffect0813e880 and CHARACTER::StopRiding0816d720. It also contains the actual original empty ComputeSkillPoints0806cd70. No gameplay branch is removed and no pending callee is given a fake body.
candidate-char.h and char-api-additions.inc retain complete existing data/API and add original signatures only. candidate-item.h adds genuine void ModifyPoints(bool). candidate-guild.h adds genuine DWORD UnderAnyWar(BYTE). candidate-questmanager.h adds genuine void Unmount(unsigned int). Full target records, method types, const/reference/static/virtual information are in full-records-api-virtuals.json. Existing known methods/fields remain intact.

Mount/polymorph code is not independently runtime-closed: see frontier below. Do not promote this whole candidate as green merely because leaf tests pass.
The candidate uses C++03-compatible explicit32-bit modular add/subtract/multiply. No C++11 static_assert/type_traits, no narrow class replacements and no obsolete whole STL class-size gates against modern GCC. mount-polymorph-abi.cpp uses typed member pointers and fixed scalar/non-STL-record checks only.

## Resolved indirect call

SetPolymorph CALL at08135c47 is CALL [EAX+0x34], primary vptr slot13. Raw ELF _ZTV9CHARACTER starts08226d00, primary addresspoint08226d08. Entry at08226d3c (raw word15) is0816d720, symbol _ZN9CHARACTER10StopRidingEv. This is virtual bool CHARACTER::StopRiding(), NOT CHorseRider::StopRiding or a lifecycle shortcut. vtables.json and setpolymorph-virtual-resolution.json retain exact bytes/symbol mapping; complete91-instruction SetPolymorph assembly is saved. Source calls StopRiding() virtually, preserving derived override behavior. Its complete real implementation enters quest Unmount, horse logic and ComputePoints.

## Critical translated behavior

SetPolymorph normalizes race<4 to0 and maintain=false. Same resulting race returns without changing maintain state. A changed race stores race/maintain, logs, calls virtual StopRiding only for nonzero race, sets added-character-state bit2 and affect flag bit4, ViewReencode, then clears added bit2 (affect bit4 stays set). When the cached normalized maintain argument is false it calls PointChange types12,14,15,13 in that order with0,false,false, then always ComputeBattlePoints.

MountVnum only acts on change: assign vnum, ViewReencode, ComputePoints. No implicit dismount/PC test is invented.

ComputePoints preserves the exact initial14 point reads in original order, zeros ONLY instant points[255], restores the original six persistent stat counters and eight party/recovery points, copies real stats12/13/14/15 and original parts0..3, resets points125/126. Full original ComputePoints657-instruction assembly is saved.
PC/NPC branching uses descriptor presence. NPC HP comes from mob; SP/stamina0; attack/move speed from mob and casting speed reloads attack speed. PC maxHP/maxSP/maxStamina use six actual GetJob calls with original ordering and32-bit wrap; attack speed receives haste110 via real PointChange.
Mounted descriptor-backed characters raise ST/DX/HT/IQ to horse minima with original repeated reads and PointChange order12,14,13,15. ComputeBattlePoints precedes maxHP/maxSP handling. Computed max is compared to INSTANT current max before conditional SetRealPoint; PointChange(max,0) happens unconditionally. Max stamina stored directly.
Wear slots0..18 each perform original three GetWear reads around actual ModifyPoints(true); immunity is ORed from the freshly fetched item's current proto (null proto=>0). No null guard is added after item callbacks where original code dereferences it. Only excess HP/SP are reduced; UpdatePacket then ComputeSkillPoints then RefreshAffect. No post-affect clamp or extra recomputation is invented.

RefreshAffect snapshots both affect words, reads speed19/17, preincrements list iterator before actual ComputeAffect, then sends UpdatePacket only if flags or short-circuit speed reads differ. ComputeAffect preserves guild-skill151..162 gating through UnderAnyWar(3), one-based flag bit operations, signed32 add/remove value, actual PointChange and Muyeong type78 start/stop event. No bounds guard was added to old unchecked flag index.

CHARACTER::StopRiding first calls actual base CHorseRider::StopRiding; false returns immediately. On success quest Unmount runs before dead/stun checks. Dead/stun clears mount field directly, ComputePoints, UpdatePacket; otherwise MountVnum(0), HorseSummon(true,false). It then calls PointChange12,14,13,15 with0,false,false. Base StopRiding clears bRiding then calls actual StartStaminaRegenEvent.

## Three independent leaf bodies for selective parent integration

1. guild-under-any-war.cpp: CGuild::UnderAnyWar080add20. Only actual map iteration/STL increment. Return first ordered enemy key whose state==6 and either requestedBYTE>2 or storedtype==requested; else0. Nonconst DWORD(BYTE), original complete guild record retained.
2. stop-muyeong-event.cpp: StopMuyeongEvent08073900, exact five instructions/tailjump event_cancel(&m_pkMuyeongEvent), field targetoffset0xd44. Actual existing event_cancel/FreeEventInfo code closes immediate dependency; never replace event_cancel with observer for a green claim.
3. compute-skill-points.cpp: actual target empty body0806cd70, complete four instructions and no callees. This is addressed original behavior, not a placeholder. Only original API/body evidence and typed signature gate are necessary; no invented effect.

Select standalone leaves for integration. candidate-mount-polymorph.cpp already defines ComputeSkillPoints, so do not also link its standalone TU into the same target. Actual StopRiding candidate replaces the abort-only virtual test boundary only when its full runtime closure is available; do not simultaneously define both.

## Meaningful staged leaf regression package

independent-leaves-api-abi.cpp is C++03, checks typed original method signatures and scalar SGuildWar offsets0/4/8/12/16,size20; no whole STL class-size gate.
under_any_war_regression.cpp exports void CheckGuildWarLookup() for the existing guild_level_regression main/source group. It uses the actual full map and method, existing explicit TEST ONLY CGuild lifecycle fixture, and sguildwar_lifecycle_fixture.cpp. Every SGuildWar field is overwritten by test setup before lookup. The fixture is NOT production constructor recovery. Tests cover empty map, all256 request/storedtype values, state0..9 with exactstate6 filter, unsigned high keys/lowest matching key, key0 immediate return, and unchanged map records.
Original SGuildWar constructor defaultstate remains unknown: inline DWARF guild.h140, retained inline sites DeclareWar080ae600/ReserveWar080aed80 both overwrite it before observation. See sguildwar-constructor-dwarf.json and UNDER_ANY_WAR_CONSTRUCTOR_GAP.md. No production ctor or reference-derived guess is added. Parent explicitly accepted this test-only lifecycle boundary after reviewing optimized-away evidence.
stop_muyeong_regression.cpp uses full CHARACTER and actual event_info allocation/create/cancel/process/free lifecycle. It checks null-handle idempotence, queued cancellation/no callback, processing cancellation/force-end/deferred-infofree/no reschedule. Reuse base desc_output_syscalls observer and established CHARACTER/quest lifecycle fixtures; required transport globals are declared in the test. The existing DESC fixture owns the global thecore_heart; this test temporarily points it at local testHeart and restores it, with no duplicate definition. Do not select target_walk observer unless its required hook is actually supplied.
No tests were compiled or run in this staging task. Later parent validation should use existing normal/O2/strict32 source grouping and separate lifecycle boundaries; never stub the recovered method or map/event behavior.

## SCC dependencies still genuinely unfinished

- PointChange0812e340 and ComputeBattlePoints0812bda0 full runtime SCC, including polymorph/mount reentry. Parent has separate staged full candidates; latest production state must be inspected before integration rather than assuming old gaps persist.
- CItem::ModifyPoints08167420: full body/callgraph exported. It reaches CHARACTER::ApplyPoint0812f7d0, accessories, original parts, ITEM_MANAGER::GetTable, constants and errors. No fake equipment modifier was added.
- CHARACTER::StartMuyeongEvent08073890 allocates char_event_info and schedules skill_muyoung_event. Scheduling body is exported, callback/gameplay closure remains unresolved. Stop leaf is separately closed.
- CHARACTER::HorseSummon0816d940: complete body/calls saved, actual character spawning, movement, rider linking and event callbacks still broad. Its additional indirect calls are not claimed resolved by the SetPolymorph slot proof.
- CHorseRider::StopRiding080c8610 -> StartStaminaRegenEvent080c8870 -> actual event creation/cancel and horse_stamina_regen_event. Full bodies/calls saved; do not omit stamina behavior.
- quest::CQuestManager::Unmount081a4540 -> actual GetPC, loaded guard, NPC0 OnUnmount081b1a00 and event/VM dispatch. Reuse quest_level evidence; no fake quest success.
- Existing ViewReencode, UpdatePacket, GetWear, GetPoint/SetPoint/real points, parts, JobInitialPoints and c_aHorseStat are actual current dependencies. Recheck source before reuse; retain all record/API types.

## Main SCC regression strategy once genuine closure is ready

Exercise same-race no-op (including changed maintain argument), normalized races0..3, changed races with maintaintrue/false, virtual override observation, real StopRiding false/success/dead/stun branches, exact reencode/addedstate/affect flag and packet order, nested PointChange ordering, mount same/change, descriptor-backed and NPC ComputePoints, preserved counters/reset-only-array, six job reads, signed wrap extrema, mounted horse minima, item callbacks that mutate wear/proto, full19-slot immunity accumulation, HP/SP clamp order, affect flag/speed changes and guild-skill war gating. These are plans for actual-source integration; do not substitute PointChange/ModifyPoints/ComputePoints/StopRiding bodies to manufacture a runtime-complete claim.

Saved decompiles are complete and paired with callgraph exports. Large ComputePoints assembly was written in bounded chunks and read back successfully (657 records, has_morefalse). manifest/audit scripts retain artifact identities.
`nCurrent-source correction: see closure-audit.md/json. Horse base/stamina/event code was ALREADY production-recovered; only CHARACTER override/quest/HorseSummon and other listed bodies are missing. Current UnderAnyWar/StopMuyeong/ComputeSkillPoints/Save now actual production. Stop regression synced from parent corrected real-event semantics; no new production edits.
