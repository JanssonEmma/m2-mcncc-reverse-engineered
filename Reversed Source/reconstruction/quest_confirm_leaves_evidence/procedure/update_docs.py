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
ledger=load(rec/'function-coverage-ledger.json');m=load(rec/'quest_confirm_leaves_evidence/RECORD_MANIFEST.json');st=load(rec/'validation-status.json');full=load(w/'checkpoint-100-validation.receipt.json')
assert m['checkpoint']==100 and m['status']=='passed' and not ledger['warnings'] and st['checkpoint100_quest_confirm_leaves']['status']=='passed' and st['runtime_suite_count']==113
assert m['typed_packet_size']==74 and m['standalone_function_mappings']==2
assert full['status']=='passed' and full['exit_code']==0 and full['suites']==full['executed_runtime_commands'] and len(full['suites'])==113
n=ledger['evidence_available'];mapped=ledger['summary']['reviewed_implementation_test_mapping'];date=full['finished_utc'][:10]
status=rec/'CURRENT_STATUS.md';text=status.read_text(encoding='utf-8-sig');history=text[text.index('---'):] if '---' in text else '\n---\n'+text
head=f'''# Current reconstruction status — {date}

Latest verified checkpoint: **100**. All **113** registered runtime commands and ABI checks passed. Proof: `quest_confirm_leaves_evidence/proofs/checkpoint-100-validation.receipt.json`. This module restores whole CHARACTER::ConfirmWithMsg08136310 and CQuestManager::SetSkinStyle081a5400 with the full original packed74-byte confirmation packet.

Confirmation retains the no-DESC early exit, header46, signed long timeout, DWORD request PID, strncpy64 plus the original single NUL at byte64, and actual DESC::Packet. Two original descriptor loads are preserved; runtime rebinding claims are limited to what the tests observe. Style uses unsigned comparison <6, storing the original int for0..5 and1 otherwise, including negative values. No enum, truncation repair, Confirm dispatcher or timeout behavior was invented.

Focused4+ABI, strict32, five new distinct O2 SQL/observer objects and actual O2 application graph passed. SDK28 objects/two archives were reused under strict hashes and all59 source/header/build guards, not rebuilt here. Supplied Lua5.0.3 public compatibility remains bounded; internal MinGW96/16 vsoriginal92/12 mismatch remains. Prior OpenState and Build matching branches remain static-only.

Ledger: {n} structured address-backed entries and {mapped} bounded implementation/test mappings over7238 entries,0warnings. Counts are not completion percentages or whole-server equivalence. Full VM/Confirm/RunState/Reward/EndRunning/stat/item/server/VisualStudio linking remains unfinished. Native Windows production support is deferred; original ELF unexecuted/unchanged.

''';status.write_text(head+history,encoding='utf-8')
frontier=rec/'DEPENDENCY_FRONTIER.md';text=frontier.read_text(encoding='utf-8-sig').replace('after checkpoint99','after checkpoint100').replace('Verified baseline:112','Verified baseline:113');text=re.sub(r'ledger\d+ entries/\d+',f'ledger{n} entries/{mapped}',text)
start=text.find('Next useful investigation:');end=text.find('Name/index queries,',start)
if start>=0 and end>start:text=text[:start]+'Next useful investigation: complete original ownership/dependency assessment before any next101 integration. ConfirmWithMsg and SetSkinStyle are verified leaves; Confirm dispatcher/timeout/PC wait handling and full RunState remain frontier work, not implemented substitutes. Preserve fresh original evidence and all unsafe/static-only limits. '+text[end:]
text=text.replace('Full112 validation is complete.','Full113 validation is complete.').replace('No new100work started.','Checkpoint100 leaves are now integrated.');frontier.write_text(text,encoding='utf-8')
readme=root.parent/'README.md';text=readme.read_text(encoding='utf-8-sig').replace('checkpoint 99','checkpoint 100').replace('**112 regression test programs**','**113 regression test programs**');text=re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{n} entries with structured address-backed evidence**',text);text=re.sub(r'\*\*\d+ bounded',f'**{mapped} bounded',text);readme.write_text(text,encoding='utf-8')
resume=f'''# Metin2 reconstruction restart — checkpoint100, {date}

Project: PROJECT_ROOT\\Reversed Source
Reference: REFERENCE_PROJECT\\Source\\srv\\server
Workspace: {cwd}
Original ELF SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Never execute or modify it.

## Verified state

Checkpoint100 passed113 exact ordered runtime commands plus ABI. Focused4: confirm leaves, state lifecycle, yield bridges and predicate diagnostics, plus ABI. Strict32, five distinct new optimized SQL/observer objects and the actual optimized application graph passed. Nine helper receipts include explicit SDK reuse and sealed PROOF_INDEX. No SDK rebuild by100.

Archive: reconstruction/quest_confirm_leaves_evidence/RECORD_MANIFEST.json and COPY_INDEX.json, package, proof logs/receipts and procedure. Ledger: {n} structured evidence entries, {mapped} bounded mappings,0warnings over7238 entries. These counts are not completion percentages.

Whole CHARACTER::ConfirmWithMsg08136310 (35instructions) and CQM::SetSkinStyle081a5400 (12) integrated. Full packet_quest_confirm/TPacketGCQuestConfirm is packed74 bytes: header@0, char msg[65]@1, signed long timeout@66, DWORD requestPID@70. Retain original long/DWORD32. Confirmation has no-DESC early exit, header46, strncpy64 plus single NUL at64 and actual fresh descriptor Packet. Literal source bytes are [39,92,48,39]; doubled slashes in serialized output do not justify a NUL repair. Unsigned style<6 stores input, otherwise1, including negative/extreme ints; no named enum invented.

Null/invalid message with a live descriptor, insufficient accessible message bytes, invalidated aliases/contexts and concurrent mutations remain original unsafe domains. No-DESC branch may skip message access. Duplicate descriptor source/instruction loads are preserved, but do not infer a runtime rebind case without test evidence. Full Confirm dispatcher/timeout callback/PC wait handling is NOT supplied by these two leaves.

## SDK and prior limitations

SDK28 actual O2 objects/two archives reused with strict hashes, all59 current source/header/build/provenance-note guards and58 reference-identical supplied files. Old query application-source hashes are ignored ONLY for SDK reuse; current application sources are independently guarded and rebuilt. Actual supplied Lua5.0.3 public opaque API compatibility is not exact original advertised5.0 revision/internal ABI recovery. MinGW32 lua_State96/TObject16(value8) differ original92/12(value4); Number8/CallInfo24 match. Internal probe failures remain diagnostic evidence. No packing repair, SDK target-address mappings or fake LuaClose.

99: seven whole CQM yield/diagnostic/predicate methods. CanStartQuest missing map returns true without Lua; present vector uses saved top, actual lua_dobuffer with ignored status, top boolean and restore. Actual SDK aux_do consumes errors through _ALERT; prefix truthiness retained. Present empty vector &v[0] remains unsafe/static-only. QuestError does NOT write error/script/state flags. Confirm and RunState exports remain frontier evidence only.

98: genuine CQM Open/Close/privateGotoEnd in questlua.cpp, Clear/Add/Send in questmanager.cpp and packet_script6. ELF_OWNER_AUDIT raw symbols/CU/artificial this are decisive. OpenState STATIC-ONLY: original direct sret vsoptional named-return NRVO/default indeterminate counter copies; no runtime or portability claim/zero repair/fake copyctor. Unsuppressed empty &script[0] remains excluded. Five other methods use actual registry/script-control/network tests.

97: actual Lua-backed PC state consumers/SendFlagList.96: five CQM Lua-query/event methods/full37-byte record; CQM data216/API97metadata retained, header stillpartialAPI. Event repeated singleton loads are static evidence, not singleton-switch runtime.95: Build matching remains STATIC-ONLY due original indeterminate counters even on duplicate keys; runtime onlyempty/nonmatching, whole body preserved. Queue producers do not execute rewards.94: whole Save/full106 record/static vector tails/DB reload/live clear.93:13 quest-info methods/header.size6.92: queries/hash/PC storage/file registration. ClearQuest live-key UAF/default counters/text-mode host differences remain unchanged.

## Next work / remaining goal

Do not reapply100 payloads or rerun its finalizer. Next101 ownership/dependency assessment is pending; no next101 implementation or pass claim yet. Fresh whole original body/data/API/callee review and current SHA guards are required before new source changes.

Full Confirm/RunState/InitializeLua/coroutines/NPC dispatch, Reward/EndRunning/LevelUp, broad SetEventFlag game effects, complete PointChange116cases127targets/stat/item/removal/affects/combat/Safebox/Boot/liveDB/server and VisualStudio linking remain pending. No partial gameplay switches, application stubs or fake VM. Native Windows production support is deferred. Preserve original file layout, complete owners/API/data, original bugs and explicit unrelated test lifecycle/OS boundaries.

## Tools and continuation

Python: USER_HOME\\.cache\\codex-runtimes\\codex-primary-runtime\\dependencies\\python\\python.exe
GCC/G++/gmake: C:\\Strawberry\\c\\bin. PYTHONPATH work/python-deps forELFtools.
Canonical runner: reconstruction/tools/validate.py run with explicit toolchain, unique log, --suite NAME --abi or --full. Require actual terminal exit and completed receipt. No overlapping builds or source mutations during builds. NativeF output works; nativeC file creation is unreliable. Reverse-engineer-anything skill applied; first liveGhidra call get_program_info, reuse hash-matched saved evidence. Do not repeat blocked low-level memory diagnostics; the old cyberflag was not a malware finding.

User's newContinue superseded the earlier99 reserve-only/no100 notes after allowance reset. Current primary/weekly windows are independent; check tool limits before expansion and save exact pending process/receipt state near limits. User authorizes autonomous work/subagents; ask only for genuine missing input. No reset-credit/account changes authorized. Keep both clean restart files identical.
'''
(cwd/'outputs/METIN2_RESUME.md').write_text(resume,encoding='utf-8');(rec/'RESUME_CHECKPOINT.md').write_text(resume,encoding='utf-8');print('Updated100/full113 docs and identical restart files; complete74 packet, SDK/static/unsafe limits explicit.')
