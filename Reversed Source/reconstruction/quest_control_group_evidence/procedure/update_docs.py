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
ledger=load(rec/'function-coverage-ledger.json');m=load(rec/'quest_control_group_evidence/RECORD_MANIFEST.json');st=load(rec/'validation-status.json');full=load(w/'checkpoint-99-validation.receipt.json')
assert m['checkpoint']==99 and m['status']=='passed' and not ledger['warnings'] and st['checkpoint99_quest_control_group']['status']=='passed' and st['runtime_suite_count']==112
assert full['status']=='passed' and full['exit_code']==0 and full['suites']==full['executed_runtime_commands'] and len(full['suites'])==112
n=ledger['evidence_available'];mapped=ledger['summary']['reviewed_implementation_test_mapping'];date=full['finished_utc'][:10]
status=rec/'CURRENT_STATUS.md';text=status.read_text(encoding='utf-8-sig');history=text[text.index('---'):] if '---' in text else '\n---\n'+text
head=f'''# Current reconstruction status — {date}

Latest verified checkpoint: **99**. All **112** registered runtime commands and ABI checks passed. Proof: `quest_control_group_evidence/proofs/checkpoint-99-validation.receipt.json`. Seven whole original CQM methods restored: four private questlua.cpp yield bridges, public QuestError, unsigned-index CanStartQuest and WriteRunningStateToSyserr. Focused4+ABI and both strict32/O2 actual appgraphs plus verified SDKreuse passed. The grouped transition is110→112; no full111 repeat.

Yield bridges preserve suspend/script/network ordering, actual Lua selection table traversal, diagnostics and original type-code behavior. CanStartQuest retains missing-map true without Lua, saved stacktop, actual lua_dobuffer with ignored status, top truthiness and restore. Actual SDK aux_do consumes errors through _ALERT; caller-prefix truthiness is preserved. Present empty vector &v[0] remains an excluded original C++03 unsafe domain. Diagnostic method retains old-state reverse query then fresh live quest/event/character/debug reads.

SDK28 objects/two archives reused by strict hashes/all59 guards, not rebuilt by99; supplied5.0.3 public compatibility only, internal96/16 vsold92/12 mismatch remains. Prior OpenState and Build matching branches remain static-only. Full Confirm/RunState/VM/Reward/EndRunning/SetEventFlag/stat/item/server/VisualStudio linking remains unfinished. No checkpoint100 expansion is part of this batch.

Ledger: {n} structured address-backed entries/{mapped} bounded implementation/test mappings over7238 entries,0warnings. Counts are not completion percentages or whole-server equivalence. Native Windows production support deferred; original ELF unexecuted/unchanged.

''';status.write_text(head+history,encoding='utf-8')
frontier=rec/'DEPENDENCY_FRONTIER.md';text=frontier.read_text(encoding='utf-8-sig').replace('after checkpoint98','after checkpoint99').replace('Verified baseline:110','Verified baseline:112');text=re.sub(r'ledger\d+ entries/\d+',f'ledger{n} entries/{mapped}',text)
start=text.find('Next useful investigation:');end=text.find('Name/index queries,',start)
if start>=0 and end>start:text=text[:start]+'Next useful investigation after allowance reset: remaining whole original Confirm/RunState/VM or other complete dependency closure after full callgraph review. Yield bridges/QuestError and CanStartQuest/WriteRunningStateToSyserr are now99verified; frontier-only Confirm/Run exports are not implementations. Preserve original ignored-status/stack behavior and unsafe empty-vector domain, no fallback repair. No new100work started. '+text[end:]
text=text.replace('Full110 validation is complete.','Full112 validation is complete.');frontier.write_text(text,encoding='utf-8')
readme=root.parent/'README.md';text=readme.read_text(encoding='utf-8-sig').replace('checkpoint 98','checkpoint 99').replace('**110 regression test programs**','**112 regression test programs**');text=re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{n} entries with structured address-backed evidence**',text);text=re.sub(r'\*\*\d+ bounded',f'**{mapped} bounded',text);readme.write_text(text,encoding='utf-8')
resume=f'''# Metin2 reconstruction restart — checkpoint99, {date}

Project: PROJECT_ROOT\\Reversed Source
Reference: REFERENCE_PROJECT\\Source\\srv\\server
Workspace: {cwd}
Original ELF SHA256:61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Never execute/modify it.

## Verified99 / reserve stop
Full112 exactordered runtimes+ABI passed frombaseline110,2newsuites grouped/no full111repeat. Focused4:yield/predicate/lifecycle/PCconsumers+ABI; both strict32/5newO2SQLobserver+actualO2appgraphs passed. Eachhelper9proofs inclverifiedSDKreuse +sealedINDEX; no28SDKrebuild. Archive reconstruction/quest_control_group_evidence/RECORD_MANIFEST.json,COPY_INDEX.json/bothpackages/group-integration/procedure/proofs. Ledger:{n} evidence/{mapped} boundedmappings/0warnings over7238, not completionpercentage. No checkpoint100expansion inthisbatch; inspectcurrentallowance beforemorework.

SevenwholeoriginalCQMmethods: privatequestlua GotoPause0805cc90/GotoInput0805cc10/GotoSelectItem0805cb90(36instructions each),GotoSelect0805c1d0(270); publicQuestError081a7bb0(53)manager.cpp; CanStartQuest081a4c20(71),WriteRunningStateToSyserr081a7a20(120)manager.cpp. Preserve ELF_OWNER_API_AUDIT.json genuineCQMmember/originalprivate-public/API proof. No freefunctions/shadowowners/fakeLua API introduced. HeaderpartialAPI andcomplete216data/original97entrymetadata remainhonest.

Pause/Input/SelectItem set suspend1/3/5 beforefull[NEXT]/[INPUT]/[SELECT_ITEM] Add/Send, no Lua stack mutation. Select usesactualSDKcheckstack/getn/args signedshort/rawgeti/string-conversion, originaltypecode-1 lua_typename diagnostic role, completeQUESTION string append/suspend2/debug/Send; allbranches present, no selection-onlyshortcut. QuestError boundedvsnprintf4096/diagnostic/freshserver-CHAR/cachedCHARtwomessage behavior retained; itdoesNOTset/reseterror/script/stateflags.

CanStart unsignedquestindex missingmap→true WITHOUTL. Presententry snapshotsgettop, actuallua_dobuffer overwholevector/StartScript withignoredstatus, lua_toboolean(-1), restoretop, boolreturn. ActualSDKaux_do/callalert consumeserrors via_ALERT; prefixtruthiness aftererror preserved. PresentEMPTYvector &v[0] originalC++03unsafe excluded, no emptyguard/c_str/statushandling/stackfallback repair. Invalidatedvectors/borrowedLua/rawcontexts/concurrent/format/outofrange/error-lifetime domains preservedunforced. WriteRunning capturesoldstate forrealreversequery, thenliveevent/name/CHAR/test_server rereads; rawborrowedstate_name remainsunrooted/unrepaired. FullConfirm0805c720/Run0805cde0 files are FRONTIERONLY, no mapping or executionclaim.

## SDK and earlier constraints
SDK28actualO2objects/2archives reusedstrictSHA +all59currentSDKsource/header/build/provenancenote guards/58referenceidenticalfiles. QuerySDKoldAPPsource hashes ignored ONLYSDKreuse; currentapplicationsource independentlyguarded/rebuilt. Actualsupplied5.0.3 external/publicopaque compatibility NOT exactoriginaladvertised5.0 orinternalABI equivalence. MinGW32lua_State96/TObject16(value8) vsold92/12(value4), Number8/CallInfo24match; internalprobe failures archiveddiagnostic, no packingrepair/SDKtargetmappings/fakeLuaClose. Testsownstates/lendL/resetL0 thenactualclose, no newproductionVMownershipclaim. Existingactualbuffer.c coreobjectnormal, no entireCRToptimizationclaim.

98sixwholeCQMOpen/Close/privateGotoEnd inexactquestlua.cpp andClear/Add/Sendmanager.cpp/fullpacket_script6. ELF_OWNER_AUDIT rawmangledsymbols/exactCU/artificialthis decisive, address-neighborhood freefunctionguess invalid. OPEN STATICONLY originaldirectsret vsoptionalNRVO namedreturn/defaultcountercopy indeterminate; no executedOpen/copyportability/zerorepair/fakecopyctor. Fiveothersactualregistry/script240controls/network tests. Unsuppressedempty&script[0] C++03unsafe excluded, originalrawaccess retained. WORDwrap/fullpayload/livecallback/clear/poolordering retained.

97PCstateconsumers/SendFlagList actualLua/flag/queue/network;96CQMquerypair/eventflags/full37record actualSDKpubliccompat. Broadcast repeatedsingletonreads STATIC notswitchruntime; broadSetEventFlag evidenceonly.95Build/Cancel/GiveItem/GiveExp/trueSetLoadedinline: BUILDMATCHING STATICONLY indeterminatecountercopiesevenduplicates, runtimeempty/nonmatching; producersqueueonly.94Save/full106record/staticvectortails/zero-growth/DBreload/liveclear;93full13questinfo/header.size6;92queries/hash/PCstorage/loader/register. ClearQuest live-keyUAF/defaultcounters/textmodehostdifferences preserved.

## Remaining goal / next window
Do notreapply99standalone111plans/groupedpayloads or rerunrecord/finalizer. Freshoriginalwholebody/callee/data/API review+guards+serializedproof required fornewmodules. FullConfirm/RunState/InitializeLua/coroutines/NPCmatching/dispatch, Reward/EndRunning/LevelUp, broadSetEventFlag gameeffects, completePointChange116cases127targets/stat/item/removal/affects/combat/Safebox/Boot/liveDB/server/VisualStudio linking pending. No partialapplicationmethod/fakeVM. NativeWindowsproductiondeferred. Preserveoriginalsourcefilelayout/fullowners/data/API/bugs andexplicit unrelatedtestlifecycle/OS boundaries.

## Tools/workflow/allowance
Python USER_HOME\\.cache\\codex-runtimes\\codex-primary-runtime\\dependencies\\python\\python.exe; GCC/G++/gmake C:\\Strawberry\\c\\bin; PYTHONPATH work/python-deps forELFtools. reconstruction/tools/validate.py run explicittoolchain/uniquelog --suite NAME --abi or --full. Finalreceipt+actualterminalexit only. Nooverlappingbuild/sourcechanges. NativeFoutputworks/nativeCcreationunreliable. Reverse-engineer-anything skill applied; firstliveGhidra get_program_info, reusehashmatchedoriginals. Don'trepeatblockedlowlevelmemorydiagnostic; oldcyberflag notmalwarefinding.

Primaryreserve triggeredfinish99only;5h andweeklywindows independent, readcurrentlimitsbeforenewexpansion. Userauthorizesautonomouswork/subagents, statefilesnearallowance, questionsonlyrealmissinginput. Noresetcredit/accountchangeauthorized. Keepbothcleanrestartfilesidentical andsaveexactpendingprocess/receiptstateifnewworkstarts.
'''
(cwd/'outputs/METIN2_RESUME.md').write_text(resume,encoding='utf-8');(rec/'RESUME_CHECKPOINT.md').write_text(resume,encoding='utf-8');print('Updated99/full112 docs and identicalrestartfiles; no100expansion, originalSDK/unsafe/staticlimits explicit.')
