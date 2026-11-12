# Closed item mutation batch — ready for parent review
Status: WORKSPACE ONLY. No F: writes or builds. Seven complete original methods, no replacement lifecycle/application methods.
Original ELF SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c (Ghidra verified this turn).
Skill: USER_HOME\.agents\skills\reverse-engineer-anything\SKILL.md.

## Integration
Run stage_integration.py again immediately before apply_guarded.py if parent source/Makefile changed.
The generator reads current production and stages additive declarations/body plus a new original-placement item_attribute.cpp; never writes F:.
apply_guarded.py preflights ALL original/new SHA256 guards before writing any file; saves parent-apply-backup.
Manifest currently covers 7 files. It preserves global fixtures and all current source additions. It adds the canonical initial RUNTIME_TESTS, first check recipe, check-abi, a dedicated C observer and its clean rule.
No class reduction, no constructor fixture, no unrelated virtual boundaries removed.

## Complete bodies
- item.cpp: AlterToSocketItem 08168170. count>2 clamps to3 AND logs3, including input3. count<=0 does nothing. Repeated SetSocket writes are retained.
- item_attribute.cpp: GetAttributeSetIndex 08186cf0, HasAttr 08186d90, AddAttribute(BYTE,short) 08186e30, GetAttributeCount 081876a0, FindAttribute 081876c0, SetAttribute 08187800.
- Original placement, full method parameter types and full CItem/item-record DWARF are saved in full-records-api-dwarf.json.
- Source/API snapshots in integration-ready preserve the CURRENT complete CItem data/virtual record; no whole old STL-class-size assertion.
- Each address.json retains untruncated decompilation + direct call graph, each *-assembly.json retains full instructions. This is a closed batch, NOT a claim that the entire item_attribute.cpp API is recovered.

Closed dependencies already production:
SetSocket, UpdatePacket, Save -> ITEM_MANAGER::DelayedSave, LogManager::ItemLog -> EscapeString/Query/CAsyncSQL, actual packet/buffer/DESC; sys_log/_sys_err are normal external diagnostic seams.
No PointChange, fake equipped modifier, fake lifecycle or host DB connection introduced.

## Regression and ABI
Staged item_attribute_regression.cpp reuses the complete existing item_socket_expiry graph plus real event.cpp/event_queue.cpp and -DMCNCC_TEST_REAL_EVENTS.
Dedicated item_attribute_desc_output_syscalls.c includes base desc_output_syscalls.c with renamed diagnostic functions. Only expected socket-count warning/AddAttribute overflow are observed; all other base fail-fast boundaries remain intact.
All item, character, quest manager, event, logger and SQL object lifecycle is the existing full graph.
Tests cover:
- All 256 type x 256 subtype mappings (and null-proto mapping), prototype apply vs normal-slot vs rare-slot distinction.
- HasAttr scans all normal slots while GetAttributeCount stops at first hole; FindAttribute excludes prototype applies/rare slots and returns first match.
- SetAttribute across slots0..4, signed short extremes, zero/nonzero type, repeated writes, owned/unowned, skip-save enabled/disabled.
- Entire 36-byte item update packet with all seven packed attribute records, packet-before-save, real delayed-save insertion before real SQL enqueue, exact SQL including signed-to-DWORD extension. bType0 still updates/saves but emits no audit SQL.
- AddAttribute prototype/existing duplication suppression, rare-only duplication allowed, zero-value suppression, first-hole insertion preserving later/rare entries, full-slot overflow before zero-value suppression.
- AlterToSocketItem INT_MIN,-1,0,1,2,3,4,INT_MAX; sequential full socket packet/save/SQL paths; unchanged socket0 is still written; untouched suffix preserved; count3 warning retained.
- C++03 typed member-pointer ABI checks plus packed attribute record/scalar sizes. No static_assert/type_traits or whole STL class sizes.

Validation: extraction, source/closure/manual harness review and guarded staging completed. Compilation/runtime/ABI execution intentionally NOT run, per task.
Parent focused target: make ... item_attribute_regression (same compiler/flags/validate.py recipe as current item_socket_expiry target); execute resulting program. check-abi now includes item_attribute_abi.cpp. Full suite only at parent's discretion.

## Exact remaining frontier (not narrowed)
CreateItem still needs full Blend_Item table/lifecycle and StartUniqueExpireEvent; skillmanager is owned by other agent.
AlterToMagicItem 081681e0 -> PutAttribute 081874d0 -> PutAttributeWithLevel08187060 -> AddAttr08186ec0, g_map_itemAttr and full TItemAttrTable/load/reset/lifecycle plus exact static probability tables. These whole decompiled bodies/callgraphs are retained, NOT replaced by simple random selection.
ApplyAddon08168720 -> CItemAddonManager::ApplyAddonTo0816e880 -> actual gauss_random/ROUND/MINMAX and RemoveAttributeType08187770 plus AddAttribute.
WARNING: original RemoveAttributeType itself calls RemoveAttributeType(this,(BYTE)FindAttribute(...)); confirmed instruction081877a0 is e8cbffffff CALL08187770. It does NOT call a removal-by-index function. This can recurse forever for attribute cycles, or return false without removing anything. We do not silently repair this original behavior. Its complete decomp/calls and remove-asm.json are retained; not included in the closed integration batch.
SetCount zero branch remains blocked by actual complete removal/Unequip/ModifyPoints and accessory degradation frontier; do not substitute nonzero-only behavior.
