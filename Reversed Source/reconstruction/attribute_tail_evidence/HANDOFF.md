# Complete original item attribute tail — checkpoint86 candidate

Root applied the six exact guarded files and is running focused validation. Child did not build, run tests, write production files or execute the original ELF. Do not re-run stage/apply or edit frozen files during root builds.

## Scope and evidence

Ten complete original methods, original owning item_attribute.cpp and complete CItem:

- ChangeAttribute(const int*)08187540
- AddAttribute()08187610 (overload; existing BYTE,short overload unchanged)
- ClearAttribute08187670
- RemoveAttributeAt(int)081876f0
- SetAttributes(const TPlayerItemAttribute*)081877c0
- CopyAttributeTo(CItem*)08187900
- HasRareAttr(BYTE)08186e00
- GetRareAttrCount08187920
- ChangeRareAttribute08187940
- AddRareAttribute08187a20

Ghidra program hash confirmed61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. original-exports.json retains full program identity/full ten decompilations. attribute-tail-ghidra.json is normalized ten-address untruncated subset. Ten individual *-assembly.json records exhaust disassembly with has_more=false, including full240-instruction AddRareAttribute. full-records-api-dwarf.json preserves full original CItem/attribute/prototype records and method types. symbols.json preserves actual ELF mangles/sizes; const ChangeAttribute/SetAttributes are established by both mangles and DWARF despite decompiler's stripped const display. Existing nonconst PutAttribute(int*) API retained; ChangeAttribute forwards via const_cast to the actual read-only body without changing API.

No new production data records/globals: original full69-byte rows, complete normal/rare by-value owning maps, seven packed3-byte attributes are already actual. Default/add percent arrays are local original5-int arrays, not invented globals. No SetForceAttribute symbol/API exists in this old target. Existing recursive RemoveAttributeType is byte-for-byte untouched; do not replace call with RemoveAttributeAt.

## Original behavior retained

ChangeAttribute captures first-hole normal count, clears ALL5 normal slots even if count0, returns0count before proto access, then actual ApplyAddon when full proto's signed sAddonType nonzero. After addon it reads fresh normal count ONCE and makes fixed remaining number of PutAttribute calls, regardless later side effects. Default0/10/40/35/15 and caller const table preserved; no updates/save caused merely by clear.

AddAttribute() local40/50/10/0/0, only count<5 gate. Clear touches normal5 only, no packet/save/log. RemoveAt only count<=index return gate, no new index>=0 check; each shifted slot invokes actual SetAttribute then final slot4zero, retaining original packet/save/log order and callback changes to upcoming source slots. SetAttributes invokes actual indirect thecore_memcpy for21 bytes then actual Save, no packet/audit. Copy only destination SetAttributes/save; no nullguard.

Rare Has checks both rare slots including zero; no normal/proto scan. Rare count is population count, not first-hole. ChangeRare captures it once, clears PREFIX[count] starting slot5 even when existing rare lives only at6, emits old low-level ItemLog(0,0,0,id,SET_RARE_CHANGE,empty) before any new save/packet, loops captured count actual AddRare calls ignoring return values, returns true whenever capturedcount nonzero.

AddRare fullcount==2 logs RARE: over2 and returns false; no broader newguard. Captures slot=count+5 BEFORE GetAttributeSetIndex/map/RNG; scans ALL keys0..81 with operator[] zero-insertion; eligible record requires nonzero dwApplyIndex, nonzero raw maxLevelBySet[set], and !HasRareAttr(KEY), not record apply index. dwProb is UNUSED; uniform external number_ex(0,size-1,item_attribute.cpp,366). After random callback rereads selected live record via operator[], live cap clamps5, BYTE narrows dwApplyIndex and short narrows selected long, then actual UpdatePacket/Save, then ItemLog with LIVE slot type/value and ID after callbacks. Thus hole6overwrite, duplicate final types through mismatched rowindex/key and post-RNG type0 audit remain original.

Unsafe original domains retained/documented, not executed in fixtures: negative RemoveAt index, null pointers, invalid rare set-1, empty available vector, post-RNG max0 causing negative value index, allocation failure. No silent hardening or fake safe exit.

## Actual regression scope

attribute_tail_regression.cpp copies full current weighted fixture graph owner types/setup, not a reduced class. Actual item/CHAR/DESC/event scheduler/SQL queue/log/packet/memcpy function-pointer graph. External C random/error/syslog/OS hooks only; diagnostics fail fast because valid cases should not reach them. Existing unrelated fixture boundaries stay explicit in inherited Makefile graph.

Plain: clear no effects/rare intact; remove all shifts+zero packet and next-slot mutation through actual SQL callback; high/count/hole rejection; full21-byte indirect copy hook, destination-only save, skipped save, selfalias copy hook. Hook avoids calling C memcpy on exact selfalias, which leaves bytes unchanged while proving original hook call/save path.
Normal: every1..100 percentile for localadd and default/custom change, both exactpercent arrays/callerunchanged, original count0withhiddenlaterattr clearing, rare intact, actual Addon singleton/gaussian cachedpair; post-addon freshcount reduces number of randomdraws (including one-slot input where addon itself satisfies count).
Rare: all4 presence combinations and zero query; all8 sets x all3 uniform positions/caps2,4,255; normal attr ignored; dwProb irrelevant; dwApplyIndex263 narrows7 independently ofkey7; zeroapply/cap excluded and outkey90ignored while mapinsertions82 occur; hole6overwrite; existingtype7/key8rowtype7 duplicate behavior; external RNG mutates live index/cap (including index0 followed realtype0rare audit); full2log beforemap/RNG; zerochange no effects; count1hole/prefix and count2fullrarechange log/save ordering. Real complete36-byte item packets checked against complete actual item attrs; exact signed audit SQL queries and dirty-save ordering asserted.

ABI uses typed10methodpointers plus existing recursive/weighted ABI, full row69 and full attribute21 bytes. No modern wholeSTL sizes asserted.

## Root validation and recording

One canonical runtime was added at initial RUNTIME_TESTS/check, plus ABI and observercleanup/rule. Existing93 preserved at freeze =>94 total. No additional LTO or partial real-event change. Both real event translationunits inherited; existing oldskill standalone recipes untouched.

Focused: attribute_tail_regression, weighted_attribute_regression, item_attribute_regression + ABI.
O2: work/attribute_tail_o2.py, actual graph +four distinct item_sql_runtime/item_sql_semaphore/item_sql_api/item_socket_config_attribute_tail_o2.o, source hash receipt work/attribute-tail-o2.receipt.json. Sequential with normal build; not run by child.
Strict32: from reconstruction/tests: g++ -m32 -D_USE_32BIT_TIME_T -std=c++03 -pedantic-errors -Wno-long-long -Ifdwatch_shim -Isocket_shim -Iposix_shim -Isql_shim -I../dependencies/mysql-4.1.3/include -include sql_socket_platform.h -fsyntax-only ../../game/src/item_attribute.cpp attribute_tail_abi.cpp attribute_tail_regression.cpp. Capture actual exit0, not empty-log inference.
Full94+ABI must actually execute all ordered runtimes.

prepare_record.py is ONE gated checkpoint86 workspace-only recorder. --root PROJECT --full FULL.receipt.json --focused FOCUSED.receipt.json --o2 O2.receipt.json --strict32-log LOG --strict32-exit-code0 (separate flag/value). Optional --source-guards requires explicit nonempty review + same6path set for parent-reviewed fix. Checks all gates/actualruntimeordered list/exactcurrent6SHA/source-matchedO2/priorverified85, then prepares normalized exports/symbolmanifest/merged10pendingmappings/status/CURRENT_STATUS/RESUME/recovery/RECORD_MANIFEST. Does not write F or run builds. Root must review and copy prepared payload under baselineSHA guards, archive evidence/receipts, update ledger conservatively.

All10 method mappings are bounded test associations, not complete-server/ABI/wholebranch equivalence. Full Boot/stat/quest/horse/startup still unfinished.
