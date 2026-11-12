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
ledger=load(rec/'function-coverage-ledger.json');m=load(rec/'pc_state_consumers_evidence/RECORD_MANIFEST.json');st=load(rec/'validation-status.json');full=load(w/'checkpoint-97-validation.receipt.json')
assert m['checkpoint']==97 and m['status']=='passed' and not ledger['warnings'] and st['checkpoint97_pc_state_consumers']['status']=='passed' and st['runtime_suite_count']==109
assert full['status']=='passed' and full['exit_code']==0 and full['suites']==full['executed_runtime_commands'] and len(full['suites'])==109
n=ledger['evidence_available'];mapped=ledger['summary']['reviewed_implementation_test_mapping'];date=full['finished_utc'][:10]
status=rec/'CURRENT_STATUS.md';text=status.read_text(encoding='utf-8-sig');history=text[text.index('---'):] if '---' in text else '\n---\n'+text
head=f'''# Current reconstruction status — {date}

Latest verified checkpoint: **97**. All **109** registered runtime commands and ABI checks passed. Proof: `pc_state_consumers_evidence/proofs/checkpoint-97-validation.receipt.json`. Three whole PC methods restored: SetCurrentQuestStateName0805e2a0, string/string SetQuestState0805e380 and SendFlagList08061980. Actual Lua-backed queries, real flags/transition queues and network output preserve original callback/alias/live-read ordering.

Focused4+ABI, strict32, five new distinct optimized SQL/observer objects and actual O2 application graph passed. Previously verified28 SDK objects/two archives were reused with strict hashes and all59 source/header/build/provenance guards, not rebuilt by97. Only SDK source subset of old query index is used for SDK reuse; its old application-source hashes do not claim current PC bytes. Actual SDK5.0.3 public compatibility remains bounded; internal MinGW96/16 vsold92/12 ABI mismatch, raw borrowed Lua string lifetime, invalidLua/null/iterator/format/conversion unsafe domains remain explicit. No fake VM, new class/API, default-state zero repair or recovered whole VM.

Ledger: {n} structured address-backed entries/{mapped} bounded implementation/test mappings over7238 entries,0warnings. Counts are not completion percentages. Prior Build matching remains static-only because original default-state copies indeterminate counters. Broad SetEventFlag, fullVM/Reward/EndRunning/stat/item/server/VisualStudio linking remain unfinished. Native Windows production support deferred; original ELF unexecuted/unchanged.

''';status.write_text(head+history,encoding='utf-8')
frontier=rec/'DEPENDENCY_FRONTIER.md';text=frontier.read_text(encoding='utf-8-sig').replace('after checkpoint96','after checkpoint97').replace('Verified baseline:108','Verified baseline:109');text=re.sub(r'ledger\d+ entries/\d+',f'ledger{n} entries/{mapped}',text)
start=text.find('Next useful investigation:');end=text.find('Name/index queries,',start)
if start>=0 and end>start:text=text[:start]+'Next useful investigation: select the next whole original dependency closure after fresh callgraph review; PC state consumers are now verified97 using actual Lua/query/flags/network dependencies. FullVM/SetEventFlag/Reward/EndRunning/stat-item closures remain pending. No original unsafe-domain repair or partial application method is authorized as a reconstruction shortcut. '+text[end:]
text=text.replace('Full108 validation is complete.','Full109 validation is complete.');frontier.write_text(text,encoding='utf-8')
readme=root.parent/'README.md';text=readme.read_text(encoding='utf-8-sig').replace('checkpoint 96','checkpoint 97').replace('**108 regression test programs**','**109 regression test programs**');text=re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{n} entries with structured address-backed evidence**',text);text=re.sub(r'\*\*\d+ bounded',f'**{mapped} bounded',text);readme.write_text(text,encoding='utf-8')
resume=f'''# Metin2 reconstruction restart — checkpoint97, {date}

Project: PROJECT_ROOT\\Reversed Source
Reference: REFERENCE_PROJECT\\Source\\srv\\server
Workspace: {cwd}
Original ELF SHA256:61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Never execute/modify it.

## Verified97
Full109 exactordered runtimes+ABI passed. Focused4:PCconsumers/Luaqueries/eventflags/PCstorage+ABI; strict32actualPC/test/ABI;5newO2SQL/observerobjects+actualO2app runtime passed.9helperreceipts include explicit verifiedSDKreuse plus consumerPROOF_INDEX. Archive reconstruction/pc_state_consumers_evidence/RECORD_MANIFEST.json,COPY_INDEX.json/package/proofs/procedure. Ledger:{n} evidence/{mapped} boundedmappings/0warnings over7238, not completionpercent.

Whole3methods: SetCurrentQuestStateName0805e2a0(61instructions), string/string SetQuestState0805e380(35), SendFlagList08061980(121). No newdeclarations/record or constchar overload claim. Queryfirst with real uppercase CQM::Instance; currentquest reread postLua callback forflag path, SetFlag/DeleteFlag/SaveFlag actual, runningstate unchanged. Namedsetter usescachedindex but live constrefquestname in realintegeroverload/transitionqueue, includingalias changes. Display fullLIVE sorted map, realreverseLua query forstrict suffix, copiedprefix/rawborrowedname/freshnumericflag aftercallback andactualChatPacket/DESC. Matching branch tested with actualSDK, no defaultQuestStatecopy. Testcounters explicitlyassigned beforeuse.

SDK28O2objects/twoactualarchives REUSED from verified96querySDKindex, not rebuilt by97. Strict checks:39oldreceiptSHA/status/logSHA,SDK28source/objectSHA,2archivesSHA andall59SDKsource/header/build/provenancenote guards. Oldqueryindex PCapplication hash ignored ONLY for SDKreuse because thismodule changesPC; rootactualPC independentlyguarded/rebuilt. Original58suppliedsource bytes stillmatchreference. Existingactualbuffer.c coreobject normal, no entireCRToptimizationclaim.

## Prior96 SDK/query/event boundary
FivewholeCQMmethods SendEventFlagList081a5c40/RequestSetEventFlag081a5df0/BroadcastEventFlagOnLogin081a6c00/GetQuestStateName081a5130/GetQuestStateIndex081a5240 integrated. Packed37 eventrecord char33@0/long32@33;5genuinedecls/CQMdata216/API97metadata retained, headerpartialAPIhonest. BroadSetEventFlag remainsoriginalevidence-only. RepeatedsingletonloadsSTATIC evidence, runtimefutureflags/DESC changes do not switchsingleton.

ActualsuppliedLua5.0.3 externaldependency not exactoriginaladvertised5.0/revisionrecovery. MinGW32lua_State96/TObject16(value8) differoriginal92/12(value4); Numberdouble8/CallInfo24match. MandatoryABIpublicopaqueAPI only; originalinternalprobe failures separatelyarchived, never repairedpacking/fakeinternal92 orSDKtargetaddressmappings. Queries reloadL eachcall, restorestack, returnrawborrowed lua_tostring pointerpossiblyNULL andrint/currentrounding thenintcast. RawborrowerGC/invalidLua/null/nontable/format/iteratorinvalid/nonfinite/outofintrange unsafe domains remainunforced/unrepaired. TestsownrealLua states/lendL/resetL0 then actualclose, nofakeLuaClose ornewproductionVMownershipclaim.

## Older and remaining work
95Build/CancelRunning/GiveItem/GiveExp/trueSetLoadedinline retained. BUILD MATCHING STATIC-ONLY: qualifying.__status copiesdefaultQuestStateindeterminatecounters evenduplicates; fullbodypreserved, runtimeempty/nonmatchingonly, nozerorepair. Producers queue only, noReward/AutoGiveItem/PointChange/Luaexecution.94Save/full106record/staticvectorstale-tail/zero-growth/actualDBreload/liveclear;93full13questinfo/header.size6/livecallback/pool;92queries/hash/PCstorage/loader/register retained. ClearQuest live-keyUAF/defaultcounters/textmodeSDKhostdifferences remain.

Do notrerun97prepare/apply/finalizer;5paths alreadyintegrated andrecorded. Nextclosure requiresfreshfulloriginalinstruction/API/data/callee review andguards after97sourcechanges. FullVMstartup/coroutines/NPCdispatch, Reward/EndRunning/LevelUp, completePointChange116cases/127targets/stat/item/removal/combat/affects/Safebox/Boot/liveDB/server andVisualStudio link pending. No partialswitch/appstub/shadowclass. NativeWindows production supportdeferred. Sourcefilelayout/completeoldowners/bugs muststayfaithful.

## Tools/workflow/state
Python USER_HOME\\.cache\\codex-runtimes\\codex-primary-runtime\\dependencies\\python\\python.exe; GCC/G++/gmake C:\\Strawberry\\c\\bin. PYTHONPATH work/python-deps forELFtools. Canonical reconstruction/tools/validate.py run explicittoolchain/uniquelog --suite NAME --abi or --full; finalreceipt+actualterminalexit required. No overlappingbuilds/source mutation. NativeFoutputswork/nativeCcreationunreliable. Reverse-engineer-anything skill applied; firstliveGhidra get_program_info, reusehashmatchedoriginals. Don'trepeatblockedlowlevelmemorydiagnostic; oldcyberflag notmalwarefinding.

Checkcurrentallowance beforeexpansion;5h andweeklywindows resetseparately. Userauthorizesautonomouswork/subagents, wantsstatefilesnearallowance, asksquestionsonlyforrealmissinginput. Noresetcredit/accountchangeauthorized. Keepthisrestartfileidentical toworkspaceoutputs/METIN2_RESUME.md andsaveexactpendingprocess/receiptstatus beforelimitifnewworkstarts.
'''
(cwd/'outputs/METIN2_RESUME.md').write_text(resume,encoding='utf-8');(rec/'RESUME_CHECKPOINT.md').write_text(resume,encoding='utf-8');print('Updated97/full109 docs and identicalrestartfiles; SDKreuse/originalunsafe limits explicit.')
