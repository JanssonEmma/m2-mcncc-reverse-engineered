# Weighted attribute group — complete candidate, pending parent proof
WORKSPACE ONLY, no F: writes/builds. Payload frozen after this handoff.
Original ELF SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c verified get_program_info first.
This is the COMPLETE four-method weighted selection/CreateItem magic closure, not a claim that all remaining item_attribute.cpp APIs or the full factory are recovered.

## First audited alternative: unique timer remains genuinely open
../unique_expire/FRONTIER.md and whole decomp/calls/assembly + full-body candidate retained:
unique_expire_event08168450, SetUniqueExpireEvent08168560, StartUniqueExpireEvent08168580, managerRemoveItem08192c20.
All timerbranches preserved in staging; NO starter-only integration/stub callback.
Exact open chain: callback->RemoveItem->owner log + safebox/mall Remove OR SyncQuickslot/RemoveFromCharacter; ALL->DestroyItem.
Equipped RemoveFromCharacter->Unequip->ModifyPoints(false)/accessory degradation/ComputeBattlePoints stat-affect SCC; GetSafebox/GetMall/CSafebox::Remove and fullDestroyItem also absent in CURRENT production. Existing event/info/clock/SetSocket/ShowAlignment/StopUnique are real, not duplicated.
Thus task pivots here without fake ownerless-only removal, survival-only callback, fake PointChange or lifecycle substitute.

## Guarded parent integration
stage_integration.py refreshes CURRENT production baseline into integration-ready/manifest.json. Re-render before apply if parent changed source/Makefile.
apply_guarded.py preflights ALL before/after hashes before any write and backs up originals.
10 files: common/tables.h full new record; constants.h/.cpp full map/data definitions; item.h4 additive signatures; item_attribute.cpp3 complete methods+private exact32 wrap helpers; item.cpp complete magic method; NEW regression/ABI/C observer; Makefile.
No stale full class replacement: additive existing fields/APIs preserved. No global fixture changes.
Makefile inherits CURRENT item_attribute_regression complete real item/CHAR/DESC/event/SQL graph, actual item_addon.cpp+item_attribute.cpp retained exactly once. Adds one canonical initial RUNTIME_TESTS/check/check-abi + observer cleanup. No new LTO workaround.
No application lifecycle mocked; existing unrelated full-class legacy fail-fast fixtures remain explicit.

## Full record, maps, data and original placement
Packed common/tables.h original SItemAttrTable/TItemAttrTable69:
char szApply[33]@0; DWORD dwApplyIndex@33; DWORD dwProb@37; long lValues[5]@41; BYTE bMaxLevelBySet[8]@61.
ALL fields retained even those selection does not consume. full-records-api-dwarf.json proves old types/bounds/offsets/sourceplacement.
Original constants.cpp complete globals:
TItemAttrMap = std::map<DWORD,TItemAttrTable>;
g_map_itemAttr08323298 and g_map_itemRare083232a4, default constructed/fully owning BY VALUE, not partial/custom map objects.
Old map size12 is evidence only. Modern STL internals differ; no sizeof old STL class assertion.
Exact original const signedint probability arrays5:
High0821aed4 raw1e00000028000000140000000800000002000000 ->30,40,20,8,2.
Low0821aee8 raw32000000280000000a0000000000000000000000 ->50,40,10,0,0.
globals-tables.json retains raw bytes/addresses/sizes; metadata retains complete declarations. No dynamic DB row content guessed for production.
Constants compiler lifecycle080e6cf0 + init/destructor wrappers080e6eb0/080e6ed0 exported fully; real std::map C++ global construction/destruction supplies ownership. No empty destructor fixture.

## Loader boundary is preserved, not faked
These maps are initially empty in the original and populated by external DB boot data. Whole CInputDB::Boot080e83f0 untruncated34459-character original saved Boot-loader-evidence.json; exact normal/rare map-copy blocks in Boot-attribute-block.txt.
Boot validates packed row69 and copies complete row by dwApplyIndex. Boot is a CALLER/data supplier, not a missing callee of the four selection methods.
Do NOT add a reduced item-only Boot, synthetic production probability rows or test-only loader method. Full Boot/startup remains unreconstructed. Regression assigns complete typed rows as explicitly synthetic TEST INPUT DATA, not recovered shipped DB content; does not claim startup/DB behavior.

## Complete body/API recovery
08186ec0 AddAttr(BYTE,BYTE), original item_attribute.cpp:
HasAttr before level0 (prototype required even0); duplicates return; level0 returns; full5 exact==5 diagnostic101; real operator[] zero-inserts missing row; indexMIN(4,level-1); long nonzero checked BEFORE short narrowing; actual SetAttribute.
08187060 PutAttributeWithLevel(BYTE), original item_attribute.cpp:
GetAttributeSetIndex; invalidset or level>5 returns, level0 still scans/draws.
Loops ALL keys0..81 with real operator[] (inserts69-byte zero values), filters maxBySet and HasAttr, vector availability+signed32 wrapped total.
ALWAYS actual number_ex(1,total,"item_attribute.cpp",139), including total0. Original number_ex reversal diagnostic/draw retained, no empty/zero guard.
Weighted selection rereads live map probabilities; selected0 remains failure sentinel diagnostic157. Final maxBySet reread/clamp; actual AddAttr.
081874d0 PutAttribute(int*) old NONCONST API:
actual number_ex(1,100,...,172), signed residual subtraction with exact32 wrapping, five-level cumulative selection; undersum can passlevel6 to normal guard. Inputs are read only despite oldsignature.
081681e0 AlterToMagicItem(), original item.cpp:
complete old locale/type/subtype thresholds. Locale0 weapon30/15, armorbody20/10, otherarmor10/5; anynonzero locale weapon20/5, body10/2, other10/1.
Actual first High put; independent second/third number_ex at item.cpp1047/1050 and optional Low puts. No return because existing attrs/full5, no clear/ApplyAddon/extra branches, no costume imports.
Private AttrAdd32/AttrSub32 are translation helpers for old x86 register wrap, not newly claimed original methods.

## Meaningful full actual regression — STAGED, NOT RUN
weighted_attribute_regression.cpp reuses complete current normal item audit graph. Scripted external random/_sys_err only; actual thecore_random/number_ex/map algorithms/SetAttribute/UpdatePacket/Save/ItemLog/SQL all execute. No app body replaced.
Coverage planned:
- Exact69-byte record and both5-int data tables, both actual globals initially empty; full rare sentinel row remains byte-for-byte unchanged.
- Direct AddAttr all gates, missing255 zero row, intrinsic apply duplication, full5 error before map insertion, unsupported itemtype allowed by helper, level255 clamp, rare-slot exclusion, long65536 still SetAttribute(short0), INT_MAX->short-1 and skip-save.
- All8 attribute sets and all10 weighted positions across weights2/3/5, caps2/4/5, outsidekey90 ignored, dwApplyIndex777 ignored in favor of actual key.
- Level0 scans/inserts/draws but no Set; empty map creates ALL82 complete zero rows then real reversal+cannot diagnostics.
- Eligible ZERO-WEIGHT row can be selected with swapped-range result0; result1 fails. This is original behavior, not a guard.
- Unsignedprobabilities wrap signed total, selected key0 error sentinel.
- External RNG callback changes final max and current weights after captured availability/total; actual live rereads tested.
- Every1..100 level-percent input for BOTH High/Low, input table unchanged, undersumlevel6, INT_MIN subtraction wrap.
- All four locale states0/1/-1/123 x weapon/body/otherarmor x2nd/3rd threshold pass/fail combinations, with SQL callback toggling locale after first mutation to prove cached thresholds.
- Existing attrs retained, full5 still consumes ALL original draws and emits3 overflows; invalidset early returns consume no RNG/maps.
- Every successful operation uses complete packed item packet, packet-before-save, real delayed-set state, signed audit SQL, true SQL queue order, no fake effects.
C++03 ABI uses typed method pointers, actual map type pointer checks, row69/offsets and data20 sizes; NEVER old whole STL/map sizes.
Native pointers remain native; record is scalar packed data, alltarget DWORD/long32 requirements explicit.

## Parent validation
Focused suite weighted_attribute_regression + check-abi using existing validate.py/compiler wrapper. No child full build requested.
Standalone32 source/API syntax command, cwd reconstruction/tests:
g++.exe -std=c++03 -Wall -Wextra -pedantic -m32 -Ifdwatch_shim -Isocket_shim -Iposix_shim -Isql_shim -I../dependencies/mysql-4.1.3/include -include sql_socket_platform.h -Wno-long-long -fsyntax-only ../../game/src/item.cpp ../../game/src/item_attribute.cpp ../../game/src/constants.cpp weighted_attribute_abi.cpp weighted_attribute_regression.cpp
work/weighted_attribute_o2.py prepared, NOT run: selected actual C++/real event graph -O2 +4 distinct *_weighted_attribute_o2.o SQL/config objects, source hashes -> work/weighted-attribute-o2.receipt.json.
Python scripts AST parsed and staged SHA checked only; no compiler/runtime/ABI pass claim.
Four normalized untruncated original exports weighted-addressed-ghidra.json; explicit staged pending source/test mappings weighted-coverage-mappings.pending.json; full function/data refs weighted-symbol-manifest.json. Rebase refs to actual applied source and gate on parent real receipts before ledger activation.
Limits: original ELF never executed. Prototype/null/rawpointer preconditions and allocation exceptions not dynamically forced. Full Boot, remaining rare/change attributes, unique timer/removal SCC, full factory/quest/stat/horse/server linking unfinished.
