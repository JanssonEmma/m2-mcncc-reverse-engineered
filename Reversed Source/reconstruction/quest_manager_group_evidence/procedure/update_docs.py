# Resolve portable paths from the repository root, regardless of current working directory.
import sys as _portable_sys
from pathlib import Path as _portable_Path
for _portable_parent in (_portable_Path(__file__).resolve().parent, *_portable_Path(__file__).resolve().parents):
    if (_portable_parent / 'project_paths.py').is_file():
        _portable_sys.path.insert(0, str(_portable_parent))
        break
else:
    raise RuntimeError('project_paths.py not found in repository ancestors')
from project_paths import resolve_path as _resolve_portable_path
from pathlib import Path
import json,re
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';cwd=w.parent
load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'))
ledger=load(rec/'function-coverage-ledger.json');m=load(rec/'quest_manager_group_evidence/RECORD_MANIFEST.json');st=load(rec/'validation-status.json');full=load(w/'checkpoint-96-validation.receipt.json')
assert m['checkpoint']==96 and m['status']=='passed' and not ledger['warnings'] and st['checkpoint96_quest_manager_group']['status']=='passed' and st['runtime_suite_count']==108
assert full['status']=='passed' and full['exit_code']==0 and full['suites']==full['executed_runtime_commands'] and len(full['suites'])==108
evidence=ledger['evidence_available'];mapped=ledger['summary']['reviewed_implementation_test_mapping'];date=full['finished_utc'][:10]
limit='Supplied Lua5.0.3 is an actual external source dependency used for bounded public-API compatibility tests; the original advertises5.0. MinGW32 lua_State96/TObject16(value8) differ original92/12(value4); Numberdouble8/CallInfo24 match. Mandatory ABI checks public opaque types only. Internal-layout probe failures are archived diagnostics, no packing/source repair or exact revision/FreeBSD ABI equivalence claim.'
status=rec/'CURRENT_STATUS.md';text=status.read_text(encoding='utf-8-sig');history=text[text.index('---'):] if '---' in text else '\n---\n'+text
head=f'''# Current reconstruction status — {date}

Latest verified checkpoint: **96**. All **108** registered runtime commands and ABI checks passed. Proof: `quest_manager_group_evidence/proofs/checkpoint-96-validation.receipt.json`. Group restores five whole original manager methods: three event-flag list/request/login operations and two Lua state-name/index queries, plus the full packed37-byte event-flag record and five genuine declarations. CQM header remains partialAPI with original complete data/metadata retained.

Focused4+ABI, eventhelper8 and actualLuahelper39+sealed index passed. All28 SDK C objects and5SQL/observer objects were distinctly optimized for the Lua O2 query proof; event helper and full108 link actual SDK archives. No SDK target-address mappings were added. {limit}

Query methods retain stack restore, live manager.L rereads, native rint/current rounding and raw borrowed-string returns including null. Unsafe lifetime/invalidLua/nonfinite/out-of-range domains remain unforced and unrepaired. Event login's repeated current-singleton loads remain static-only evidence; tests mutate future flags/descriptors but not singleton identity. Broad SetEventFlag, fullVM/Reward/EndRunning and server/VisualStudio linking remain unfinished. Earlier Build matching branches also remain static-only due original indeterminate-counter copy.

Coverage ledger: {evidence} structured address-backed entries and {mapped} bounded implementation/test mappings over7238 entries,0warnings. Counts are not completion percentages or whole-server equivalence. Native Windows production support deferred; original ELF unexecuted/unchanged.

''';status.write_text(head+history,encoding='utf-8')
frontier=rec/'DEPENDENCY_FRONTIER.md';text=frontier.read_text(encoding='utf-8-sig');text=text.replace('after checkpoint95','after checkpoint96').replace('Verified baseline:106','Verified baseline:108');text=re.sub(r'ledger\d+ entries/\d+',f'ledger{evidence} entries/{mapped}',text)
start=text.find('Next useful investigation:');end=text.find('Name/index queries,',start)
if start>=0 and end>start:text=text[:start]+'Next useful investigation: separately prepared PC SendFlagList/string-state consumers after fresh instruction/dependency/guard review. They are NOT part of96 and have no new pass claim. Broad SetEventFlag/game-effects, fullVM/Reward/EndRunning/stat dependencies remain unresolved. Actual Lua querypair and eventflag list/request/login are now integrated; SDK public compatibility only, internal mismatch explicit. '+text[end:]
text=text.replace('Lua-backed state-name/index resolution and coroutine ownership remain separate.','Lua-backed state-name/index query methods are integrated with actual supplied SDK; full coroutine/VM ownership remains separate.').replace('Full106 validation is complete.','Full108 validation is complete.');frontier.write_text(text,encoding='utf-8')
readme=root.parent/'README.md';text=readme.read_text(encoding='utf-8-sig');text=text.replace('checkpoint 95','checkpoint 96').replace('**106 regression test programs**','**108 regression test programs**');text=re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{evidence} entries with structured address-backed evidence**',text);text=re.sub(r'\*\*\d+ bounded',f'**{mapped} bounded',text)
publicnote='The supplied Lua5.0.3 sources support bounded public-API compatibility tests. They are not claimed to be the exact original Lua revision; internal MinGW layouts differ from the original binary.'
if publicnote not in text:text+='\n'+publicnote+'\n'
readme.write_text(text,encoding='utf-8')
resume=f'''# Metin2 reconstruction restart — checkpoint96, {date}

Project: PROJECT_ROOT\\Reversed Source
Reference: REFERENCE_PROJECT\\Source\\srv\\server
Workspace: {cwd}
Original ELF SHA256:61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Never execute/modify it.

## Verified96 and exact limitations
96/full108 exactordered runtimes+ABI passed. Focused4:eventflags/Luaqueries/Save/PClookup+ABI; eventhelper8; actualSDKhelper39+PROOF_INDEX passed. SDK28Cobjects/twoarchives and5SQL/observer objects distinctly O2 compiled forqueryproof, noLTO; actualbuffer.c coreobject remains normal, no wholeCRT/runtimeO2 claim. Archive reconstruction/quest_manager_group_evidence/RECORD_MANIFEST.json,COPY_INDEX.json,bothpackages/groupedpayloads/procedure/proofs. Final merged70paths10support, parent107plans NOT independently applied. Current ledger:{evidence} evidence/{mapped} bounded mappings/0warnings over7238; not completionpercent.

Whole5 original appmethods integrated: SendEventFlagList081a5c40, RequestSetEventFlag081a5df0, BroadcastEventFlagOnLogin081a6c00, GetQuestStateName081a5130, GetQuestStateIndex081a5240. Full SPacketSetEventFlag/TPacketSetEventFlag37:char33@0/long32@33, original strncpy32+NUL, realDBPacket55/handle0. Live map/cache/future callback order retained. Broadcast's5separate singletonloads are STATIC evidence; runtime does notswitchsingleton. Full broad SetEventFlag081a5e60 remains evidence-only, no partial mapwrite implementation.5genuine manager declarations added; CQM complete data216/97entry originalAPI metadata, headerpartialAPI remainshonest.

Queries use actualSDK tables/metamethods/errors/stack/number/string conversions. Current manager.L reloaded before eachcall, originalnil diagnostic thenrestore/fallback, non-table actualerror unprotected, raw borrowed lua_tostring maynull and returns afterstackrestore. No copy/rooting/fallback/numberclamp. rint/currentrounding thenintcast preserved. Tests own2actualstates, lendL, setL0 before existing unrelated managerfixture destructor and call actualSDK lua_close; no fakeLuaClose/recoveredCQM Luaownership claim.

SDK provenance58 supplied reference text/source/header/build files byteidentical (plus separateauthoredprovenancenote),31prebuilt/user artifacts excluded. Actualsource5.0.3/1994-2006 vsoriginaladvertised5.0/1994-2003. {limit} OriginalDWARF/failedinitial-Iprobe/retry/internal-layout.s retained separately, not mandatoryABI failures. No originalSDK function mappings or fullVM equivalence. Borrowedstring/GC/invalidation/null/invalidLua/rintNaN-out-of-int domains remain unsafe/static-only, not forced.

## Prior groups
95: wholeBuild/CancelRunning/GiveItem/GiveExp andtrueSetLoadedinline. Build matching .__status branches STATIC-ONLY: defaultQuestStatecounter copies indeterminate, even duplicatekey; wholebodypreserved/runtimeempty-nonmatching only, nozerorepair/replacementcopy. Queueproducers do notexecuteReward/AutoGiveItem/PointChange/Lua.94:wholeSave/full106byte table/staticvector stale tails/zero growth/actualDBreload/liveclear.93:13questinfo/full6byteheader unchangedsize6/actualpool/livecallbacks.92:4managerqueries/hash,7PCstorage+4inline,fullfileloader/registration. ClearQuest originallive-keyUAF/defaultindeterminatecounters/textmodeSDKhostdifferences preserved. See archives, no stale reapply.

## Next preparation only / remaining work
PC SendFlagList08061980, SetCurrentQuestStateName0805e2a0 and string-state SetQuestState0805e380 consumers are separately prepared for potential97, NOTapplied or passed as96. Find latest workspacehandoff, inspect fresh guards after96SDK/header changes and fulloriginal callgraph before apply. Exactlyone futurecanonicaltest wouldraise109 onlyafteractualproof. Do not add109 retrospectively or rerun96 standalone107plans/finalizer.

Full LuaVM startup/coroutines/dispatch, Reward/EndRunning/LevelUp, fullstat/item/combat/removal/affect/Safebox/Boot/liveDB/server and VisualStudio link remain pending. FullPointChange116cases/127targets candidates requirecompleteactualSCC, no partial switch/appstub. NativeWindows production support deferred. Preservecompleteoldowners/API/data/layout/source structure and original bugs; no fakeVM/loader/stat/applicationmethods.

## Tools/allowance
Python USER_HOME\\.cache\\codex-runtimes\\codex-primary-runtime\\dependencies\\python\\python.exe; GCC/G++/gmake C:\\Strawberry\\c\\bin. PYTHONPATH work/python-deps forELFtools. Canonical reconstruction/tools/validate.py run explicitmake/cc/cxx/uniquelog, --suite NAME --abi or --full. Receipts+actualterminalexit only. No overlapping builds/source mutations; nativeF outputs work/nativeC creation unreliable. Reverse-engineer-anything skill applied; firstliveGhidra get_program_info, reuse hashmatched evidence. Don't repeatblocked lowlevelmemorydiagnostic; oldcyberflag not malwarefinding.

Primary allowance was nearingreserve during96 preparation; read currentlimits beforenewexpansion.5h reset andweeklywindow differ. No resetcredit/account changes authorized. User authorizesautonomouswork/subagents and requests statefilesnearallowance; only ask realmissinginput. Save furtherexactpendingprocess/receiptstatus beforeending if anotheroperationstarts.
'''
(cwd/'outputs/METIN2_RESUME.md').write_text(resume,encoding='utf-8');(rec/'RESUME_CHECKPOINT.md').write_text(resume,encoding='utf-8');print('Updated96/full108 docs +identicalrestartfiles; SDKmismatch/staticlimitations explicit.')
