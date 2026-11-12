# Quest PC lifecycle / lookup — bounded save
WORKSPACE ONLY. No F writes, builds, project header/fixture/Makefile changes.
SOURCE/API/EVIDENCE READY; NOT INTEGRATION READY. Parent requested freeze. Actual test sources and guarded legacy graph migration remain to stage.
Original SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c verified first.

## Complete closed body candidates
questpc.cpp.candidate:
PC C2ctor0805d580/C1ctor0805d790 constructs ALL full STL/string fields, initializes reward/donefalse,ID0,runningNULL,send0,loadedfalse,waitPID0,confirmfalse. m_iLastState stays UNINITIALIZED (modern reference differs).
PC D2dtor0805d900/D1dtor0805db30 calls Destroy then ALL complete member teardown; no Lua/coroutine/reward action.
Destroy0805dd60 ONLY ClearTimer; leaves other state untouched.
SetID0805dd70 clears done BEFORE storing DWORD; loaded/timers/context retained.
GetCurrentQuestName0805dd90 returns conststring& but member itself NONCONST.
ClearTimer0805ef10 logs map count, iterates full string-key timer map in lexical order, actual CancelTimerEvent(&value), then clears.
questevent.cpp.candidate:
CancelTimerEvent081ad980 uses actual FindEventInfo((*handle)->dwInfoID), deletes name[] if present, setsNULL, actual event_cancel. NOT modern dynamic_cast of event->info. Both pointer and pointed event must be nonnull.
questmanager.cpp.fragment:
GetPCForce081a4d40 full native map<unsignedint,PC> lookup/insertion/copy/destruction; SetID ONLY new key; stable value identity.
GetPC081a4e50 actual CHARACTER_MANAGER::FindByPID; missing/null character returnsNULL preserving ALL context/map, even existing PC. Success sets only currentPC/character and selectedDungeonNULL.
Actual manager C1ctor0819f5e0 initializes ONLY L among raw scalar/pointer fields. Source L(0), all other scalar fields indeterminate; full singleton/STL construction real. DO NOT import modern reference pointer/scalar zero initializers. Additional C2variant0819f0f0 NOT yet audited.
Implicit PC copy081ab2b0 full evidence retained; memberwise COMPLETE data, no repaired/deep ownership. Avoid copying populated timer registries in tests (old ownership hazard). Native map temporary/copy counts may differ from old compiler; do not assert exact empty-clear log count.
All direct callees are existing real event registry/scheduler, FindByPID, STL/allocator and diagnostics; no Lua needed.

## Full old data/API
Fresh pc-full-record-api-dwarf.json: PC108, RewardData12, nested statechange12, QuestState60, plain timer/server timer16 from4 old CUs.
questpc.h.candidate preserves every existing private member/type/order and adds all61 non-artificial method declarations with original access/signatures; compiler copy/assignment implicit. pc-api-declarations.json retains linkage/access/file/line; public/private API fragments/render_api.py retain derivation.
Most methods are genuine UNDEFINED declarations, not fake implementations. Only bodies above plus inline IsLoaded field read are staged. Nested statechange constructor signature declared, body pending; RewardData existing constructor remains unresolved. No invented modern GetID or unsupported QUEST_SEND enum.
Old IsLoaded/GetCurrentQuestName/IsConfirmWait NONCONST: names lack manglingK. Do not replace with newer const API.
Inline IsLoaded read witnessed by original LevelUp gate; no synthetic standalone original function address/count.
questevent.h.candidate contains BOTH complete plain records, NO modern EVENTINFO base/vptr/ctor:
server {intcycle,uintnpc,uintarg,char*name};
player {intcycle,uintplayer,uintnpc,char*name}.
Old16/name12 under32-bit. Native64 pointer layout naturally differs; no forced native old size.
All3 event helper API signatures in event-api-dwarf.json; creation helper bodies remain pending actual timer/VM closure, no fake creators.
No whole oldPC108/CQM216/QuestState60/STL size assertion allowed against modern native STL.

## Explicit necessary legacy migration (NOT silently applied)
1 New real questpc.cpp/questevent.cpp; additive fullPC and2manager lookup APIs; original manager fragment +char_manager.h include.
2 Every literal questmanager.cpp source/command graph must add both new units ONCE, including DESC_PACKET_SOURCES and isolated recipes.
3 quest_flag_fixture.cpp currently has empty manager ctor/dtor and abortPC/NPCdtors. Retire ONLY recovered manager ctor +PCdtor; real methods now own those paths. Keep manager destructor explicitly TEST ONLY L==0 +emptyNPC registry (member PC destruction real); keepNPCabort. No fakeLua_close/VM.
4 Legacy desc_fixture.cpp/entity_packet_fixture.cpp lack FindEventInfo. quest_pc_event_boundary.fragment proposes explicit unused abort under !MCNCC_TEST_REAL_EVENTS, NEVER fake successful lookup. Preserve ALL existing bodies/hooks/thecore_shutdown aborts. Parent explicitly permits this for never-populated legacy timers; do not swap fabricated-event legacy tests onto actual scheduler silently.
5 Dedicated newPC suite MUST define MCNCC_TEST_REAL_EVENTS and link actual event.cpp/event_queue.cpp.
6 Existing standalone quest_flag_regression only links questmanager.cpp+fixture. Expand to full real TARGET_WALKING/CHAR graph; filter custom target_walk observer to base desc_output_syscalls.o; add3 transport int globals to its test if needed. NO fakeFindByPID/newLTO workaround.
7 Constructor current context/LastState unknown: initialize test sentinel fields BEFORE failure/context assertions; never read unknown fields expecting0.
8 No apply-ready plan has been emitted; source closed does not imply all93 legacy graphs currently link. Exact migration above remains required.

## Concrete regression source plan next
Full production records/methods +real scheduler, current complete CHAR/DESC graph; no reduced PC/CQM/CHAR.
- Constructor known scalars/containers empty; never inspect unknownLastState or old sizeof.
- SetID/data preservation, const-reference name identity; Destroy retains flags/saveflags/currentquest/running/loaded/ID while clearing only timers.
- Complete info via actual __AllocEventInfo(sizeofrecord), fill all fields/name via newchar[], actual event_create_ex TEST callbacks. Pending event->infoNULL but numeric registry lookup must still free name.
- External paired new[]/delete[] observation verifies name freed/setNULL BEFORE actual FreeEventInfo log/free. IDs follow timer-map string lexical order (including embeddedNUL).
- Real queued cancellation clears info/handles but queuecount remainsN until event_process canceled entries.
- Processing callback ClearTimer/Cancel setsforceend; info retained until callback return. Return99 can trigger established real event_process211 diagnostic then real final free.
- Missing-info branch may use fully initialized raw EVENT processing-state input with missing numericID; observe skip-name+forceend/clear; no unsafe nullhandle/invalidqueued success claim.
- GetPCForce uintkeys0/1/INTMAX/0x80000000/UINTMAX, insertedflags/ID, stableidentity, existing loaded/done/flags/timers NOT reset.
- GetPC real FindByPID map valid/absent/storedNULL, failure preserves all context/no insertion includingexistingPC; success sets only intended3 context fields. Manager L==0 test ownership boundary explicit, emptyNPC only, no Lua function provided.
- C++03 typed gates GetPC/GetPCForce(unsignedINT), SetID(DWORD), nonconst getters; strict32 timer16/name12 and scalar record12 only, no STLwhole sizes.
No test source yet compiled/run; this is an exact plan, not runtime coverage.

## Lua status
Earlier ../lua-elf-evidence.json: target staticLua5.0 copyright1994-2003; double8/API/layout evidence.
Re-read supplied liblua/include/lua.h: Lua5.0.3 copyright1994-2006, defaultdouble, lua_resume(L,int). Public source compatibility supported; exact target patch equivalence NOT proven.
This PC group calls NO Lua API. No Lua source adoption/rebuild needed, no downloads/original execution/fake VM cleanup.
Whole LevelUp/NPC dispatch/coroutine/EndRunning/reward/persistence/registration and manager owningLua lifecycle remain RECOVERY_PLAN.md. This advances leveling's PC storage/lifecycle/lookup, not unrelated items.

## Evidence
13 addressed fresh decomp/calls/assembly files:2PCctors/2dtors/defaultcopy/Destroy/SetID/name/ClearTimer/Cancel/GetPCForce/GetPC/managerC1ctor.
Compiler variants are not independent semantic features. PCsymbol/API metadata retained.
manifest.json records saved SHA/size and NOT_APPLY_READY status. No production status/ledger changes.

## Bounded source follow-up 2026-10-27 — no execution
Actual quest_pc_lookup_regression.cpp and C++03 quest_pc_lookup_abi.cpp are now SAVED with dedicated external diagnostic observer quest_pc_lookup_desc_output_syscalls.c. quest_flag_fixture.cpp.candidate and MIGRATION_SNIPPETS.md give exact legacy ownership/link transitions (recipe lines need literal TAB when inserted). No broad Lua/new RE, F writes, integration or builds.
Regression covers known PC construction/SetID/name/Destroy preservation; actual queued timers sorted full string keys including embeddedNUL, pending event->infoNULL numeric lookup and real cancellation/queue drain; null name; absent info processing-state input; actual processing callback force-end+event_process211; full uint lookup identity/existingstate and real PID context behavior.
Explicit remaining limits: test source is UNCOMPILED and depends on saved API/production body candidates being integrated with the documented full real graph. It does not instantiate QuestState/reward payload objects because those constructors/full execution remain deferred. NameNULL is observed before actual info free; exact delete[] invocation is static body evidence, not a paired allocator observer in this source. The processingHandle test global is a non-owning observation alias, cleared by test callback after asserting actual owning PC map cleared. No fake quest callback/VM body is supplied; timer callbacks are explicitly TEST input events. Invalid null timer-map handles remain an original precondition, not silently guarded.
Old HANDOFF paragraphs saying test source still pending are superseded by this bounded follow-up. Guarded CURRENT Makefile/source apply pipeline remains pending, deliberately not forced before parent allowance checkpoint.