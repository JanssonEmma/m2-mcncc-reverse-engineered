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
ledger=load(rec/'function-coverage-ledger.json');m=load(rec/'quest_state_lifecycle_evidence/RECORD_MANIFEST.json');st=load(rec/'validation-status.json');full=load(w/'checkpoint-98-validation.receipt.json')
assert m['checkpoint']==98 and m['status']=='passed' and not ledger['warnings'] and st['checkpoint98_quest_state_lifecycle']['status']=='passed' and st['runtime_suite_count']==110
assert m['runtime_scope']['OpenState']=='static_only_original_sret_NRVO_indeterminate_copy'
assert full['status']=='passed' and full['exit_code']==0 and full['suites']==full['executed_runtime_commands'] and len(full['suites'])==110
n=ledger['evidence_available'];mapped=ledger['summary']['reviewed_implementation_test_mapping'];date=full['finished_utc'][:10]
status=rec/'CURRENT_STATUS.md';text=status.read_text(encoding='utf-8-sig');history=text[text.index('---'):] if '---' in text else '\n---\n'+text
head=f'''# Current reconstruction status — {date}

Latest verified checkpoint: **98**. All **110** registered runtime commands and ABI checks passed. Proof: `quest_state_lifecycle_evidence/proofs/checkpoint-98-validation.receipt.json`. Six whole original CQM methods restored in exact owners: questlua.cpp OpenState/CloseState/private GotoEndState and questmanager.cpp ClearScript/AddScript/SendScript, plus full packed6-byte packet_script. ELF_OWNER_AUDIT raw mangled symbols, low_pc CU/file and artificial this establish genuine member ownership; preserve it for future work.

**OpenState is static-only.** Original instructions construct directly in caller return storage; named-local source return permits optional NRVO and otherwise copies indeterminate default counters. Full body retained, no zero repair or altered by-value API. Five other methods have bounded actual SDK/script/network runtime tests, including240 script-control cases, real registry unref, WORD wrap/full payload, live callbacks, final clear and buffer-pool return. Unsuppressed empty &script[0] C++03 unsafe domain remains excluded and unrepaired.

Focused4+ABI, strict32 actual manager/questlua/test/ABI, five new distinct O2 SQL/observer objects and actual appgraph O2 passed; SDK28 objects/two archives reused with strict SDK hashes/all59 guards, not rebuilt here. Actual supplied5.0.3 opaque public compatibility only, internal96/16 vsold92/12 mismatch remains. Full VM/startup/RunState/Reward/EndRunning/server/VisualStudio remain unfinished; prior Build matching is also static-only.

Ledger: {n} structured address-backed entries/{mapped} bounded implementation/test mappings over7238 entries,0warnings. Counts are not completion percentages or whole-server proof. Native Windows production support deferred; original ELF unexecuted/unchanged.

''';status.write_text(head+history,encoding='utf-8')
frontier=rec/'DEPENDENCY_FRONTIER.md';text=frontier.read_text(encoding='utf-8-sig').replace('after checkpoint97','after checkpoint98').replace('Verified baseline:109','Verified baseline:110');text=re.sub(r'ledger\d+ entries/\d+',f'ledger{n} entries/{mapped}',text)
start=text.find('Next useful investigation:');end=text.find('Name/index queries,',start)
if start>=0 and end>start:text=text[:start]+'Next useful investigation: remaining complete quest VM/dispatch or other original dependency closure after full callgraph review. CQM Open/Close/privateGotoEnd belong to questlua.cpp per rawELF owneraudit; script Clear/Add/Send belong to questmanager.cpp. OpenState remains static-only original sret/NRVO/counter-copy domain; unsuppressed empty raw script access is excluded. Full RunState/InitializeLua/NPC dispatch/Reward/EndRunning and broad SetEventFlag remain unresolved. '+text[end:]
text=text.replace('Full109 validation is complete.','Full110 validation is complete.');frontier.write_text(text,encoding='utf-8')
readme=root.parent/'README.md';text=readme.read_text(encoding='utf-8-sig').replace('checkpoint 97','checkpoint 98').replace('**109 regression test programs**','**110 regression test programs**');text=re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{n} entries with structured address-backed evidence**',text);text=re.sub(r'\*\*\d+ bounded',f'**{mapped} bounded',text)
extra='OpenState is supported by complete static evidence only: original return-storage construction does not establish portable named-return copy elision when default counters are indeterminate. Unsuppressed empty script access remains an excluded original unsafe domain.'
if extra not in text:text+='\n'+extra+'\n'
readme.write_text(text,encoding='utf-8')
resume=f'''# Metin2 reconstruction restart — checkpoint98, {date}

Project: PROJECT_ROOT\\Reversed Source
Reference: REFERENCE_PROJECT\\Source\\srv\\server
Workspace: {cwd}
Original ELF SHA256:61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Never execute/modify it.

## Verified98 / owner authority
Full110 exactordered runtimes+ABI passed. Focused4:lifecycle/PCconsumers/Luaqueries/PCstatehelpers+ABI; strict32actualmanager/questlua/test/ABI;5newdistinctO2SQL/observerobjects+actualO2app passed.9helperreceipts include explicit SDKreuse and sealedPROOF_INDEX. Final8paths66support guarded, archive reconstruction/quest_state_lifecycle_evidence/RECORD_MANIFEST.json,COPY_INDEX.json/package/proofs/procedure. Ledger:{n} evidence/{mapped} boundedmappings/0warnings over7238, not completionpercentage.

SixWHOLE genuineCQMmethods: questlua.cpp OpenState0805cd80(28instructions),CloseState0805d000(22),PRIVATE GotoEndState0805cd10(34); questmanager.cpp ClearScript081a4ee0(21),AddScript081a4f30(7),SendScript081a4f50(122). Complete packet_script6:headerBYTE0,sizeWORD1,skinBYTE3,src_sizeWORD4. Preserve ELF_OWNER_AUDIT.json: rawmangled _ZN5quest13CQuestManager symbols,exactlow_pc/originalCUdeclfile/artificialthis definitively establish membership and questlua.cpp owner, overriding address-neighborhood/freefunction guesses. No inventedGetLuaState/freehelper/publicGotoEnd.

OPENSTATE STATIC-ONLY: original assembly constructs QuestState directly in caller sretstorage, setsargs0/stinput, newthread(currentL),co,ref(freshcurrentL,registry),ico, return. Namedlocal source allowsoptionalNRVO; absentelision copiesoriginalindeterminatecounters. No executedOpenState/copy-portability claim, zero repair/fakecopyctor/APIchange. Test manualrealSDKthread setup is Close input, NOT Open replacement/proof.

Close usescoonlyguard, actualCURRENTownerL luaL_unref thencoNULL, retainsico/args/state/allotherfields; no lua_close. Five othermethods dynamicallytested:Close registryrelease/idreuse/currentL/negative/NULL;Clear fullstring then skin1;Add fullbinary/selfalias;Send240DONE/NEXT/PCdone/skin/error/drop cases, WORDwrap/fullpayload, cachedheader/freshpayload/lateCHAR/DESC/debug/callback/finalclear/actualpool;GotoEnd appendtemporary[DONE]/destroy thenSend, keepsactualregisteredcoroutine live. Send noSendbranch consumesdrop thenClear, noPCdone. Unsuppressedempty &str[0] originalC++03UB excluded; fullsource retainsrawaccess, no c_strworkaround. Invalidcontext/registry/stringborrowed/COW/unshare/format/allocator domains preserved/unforced. Currentpacketlength WORDwrap remainsunpatched despite fullpayload.

## ActualSDK/prior state
SDK28actualO2objects/twoarchives reusedstrictSHA, all59source/header/build/provenancenote guards;58suppliedreferencefiles identical. Not rebuilt by98; oldqueryproof appsourcehashes excluded ONLYSDKreuse, allcurrentapplicationbytes independentlyguarded/rebuilt. ActualSDK5.0.3 external/publicopaque compatibility, not exactoriginaladvertised5.0. MinGW32lua_State96/TObject16(value8) differold92/12(value4); Number8/CallInfo24match. Internalprobe failures separatelyarchiveddiagnostic, no SDKpacking repair or targetSDKmappings. No fakeLuaClose; testsownstates/lendL/resetL0 beforeexistingunrelatedmanagerfixture andactualclose. FullproductionVMownership/startup remainspending.

97wholePCstateconsumers/SendFlagList retain actualLua/livealias/freshflag/queue/network behavior.96fivewholeCQMquery/eventmethods/full37record/5genuinedecls; headerpartialAPI/data216/97originalAPImetadata honest; broadSetEventFlag evidence-only, repeatedsingletonloadssupportedSTATIC notswitchruntime.95Build/Cancel/GiveItem/GiveExp/trueSetLoaded; Buildmatching STATICONLY defaultcountercopyindeterminateevenduplicates, runtimeempty/nonmatchingonly. Producersqueueonly.94Save/full106record/staticvectorstale-tail/zerogrowth/actualDBreload/liveclear;93full13questinfo/header.size6/livecallback/pool;92queries/hash/PCstorage/loader/register retained. ClearQuest live-keyUAF/defaultcounters/textmodeSDKhostdifferences preserved.

## Next / remaining closure
Do notreapply98preparedpayloads or rerunrecord/finalizer. Freshchanges need originalfullbody/API/data/callee review, updatedguards andserializedactualtests. FullRunState/InitializeLua/coroutines/NPCmatching/dispatch, Reward/EndRunning/LevelUp, broadSetEventFlag effects, fullPointChange116cases/127targets/stat/item/removal/affects/combat/Safebox/Boot/liveDB/server/VisualStudio link pending. No partialapplicationmethod or fakeVM/callees. NativeWindowsproduction deferred. Preservecompleteowners/sourcefilelayout/originalbugs and explicit test-onlyunrelated lifecycle/OS boundaries.

## Tools/workflow/allowance
Python USER_HOME\\.cache\\codex-runtimes\\codex-primary-runtime\\dependencies\\python\\python.exe; GCC/G++/gmake C:\\Strawberry\\c\\bin. PYTHONPATH work/python-deps forELFtools. reconstruction/tools/validate.py run explicitmake/cc/cxx/uniquelog --suite NAME --abi or --full. Actualterminalexit+receipt only; no overlappingbuild/source mutations. NativeFoutputworks/nativeCcreationunreliable. Reverse-engineer-anything skill applied; firstliveGhidra get_program_info, reusehashmatchedsaved evidence. Don'trepeatblockedlowlevelmemorydiagnostic; oldcyberflag not malwarefinding.

Checkcurrentallowance beforeexpansion;5h/weeklywindow independent. Userauthorizesautonomouswork/subagents andrequestsstatefilesnearallowance, realmissinginput questionsonly. Noresetcredit/accountchangeauthorized. Keepbothcleanrestartfilesidentical andappendexactpendingprocess/receiptstatebeforelimitifnewworkstarts.
'''
(cwd/'outputs/METIN2_RESUME.md').write_text(resume,encoding='utf-8');(rec/'RESUME_CHECKPOINT.md').write_text(resume,encoding='utf-8');print('Updated98/full110 docs and identicalrestartfiles; exactowner/OpenSTATICONLY/rawempty/SDKlimits explicit.')
