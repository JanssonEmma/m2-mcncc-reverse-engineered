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
from datetime import datetime
from zoneinfo import ZoneInfo
import json,re
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';cwd=w.parent
load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'))
ledger=load(rec/'function-coverage-ledger.json');m=load(rec/'quest_ownership_evidence/RECORD_MANIFEST.json');st=load(rec/'validation-status.json');full=load(w/'checkpoint-101-validation.receipt.json')
assert m['checkpoint']==101 and m['status']=='passed' and not ledger['warnings'] and st['checkpoint101_quest_ownership']['status']=='passed' and st['runtime_suite_count']==114
assert m['standalone_function_mappings']==8 and m['semantic_methods']==4 and m['compiler_copy_context_only']=='081ab460' and m['global_manager_NPC_pairs']==13
assert full['status']=='passed' and full['exit_code']==0 and full['suites']==full['executed_runtime_commands'] and len(full['suites'])==114
fixture=(rec/'tests/quest_flag_fixture.cpp').read_text(encoding='utf-8-sig');code=re.sub(r'//[^\n]*|/\*.*?\*/','',fixture,flags=re.S);assert not re.search(r'CQuestManager\s*::\s*(?:~CQuestManager|Destroy)\s*\(',code)
n=ledger['evidence_available'];mapped=ledger['summary']['reviewed_implementation_test_mapping'];date=datetime.fromisoformat(full['finished_utc']).astimezone(ZoneInfo('Europe/Berlin')).date().isoformat()
status=rec/'CURRENT_STATUS.md';text=status.read_text(encoding='utf-8-sig');history=text[text.index('---'):] if '---' in text else '\n---\n'+text
head=f'''# Current reconstruction status — {date}

Latest verified checkpoint: **101**. All **114** registered runtime commands and ABI checks passed. Proof: `quest_ownership_evidence/proofs/checkpoint-101-validation.receipt.json`. Genuine CQM Destroy/destructor and full NPC default constructor/destructor are integrated: eight original compiler address variants represent four semantic source methods. NPC copy081ab460 remains compiler context only, not a default constructor or new mapping.

The old L0/emptyNPC destructor replacement has been removed. Actual CQM/NPC lifecycle now links globally at all13 manager source/dependency/recipe literals; full114 validates the affected old graphs. Real SDK finalizers, current-L close-before-clear, complete NPC ordinary/nested containers, PC timer cleanup, singleton teardown and raw server-event nonownership are exercised. Empty NPC destructor user body still invokes full automatic member teardown.

Focused4+ABI, strict32 actual manager/NPC/test/ABI, five distinct new O2 SQL/observer objects and actual app graph with NPC.cpp exactly once passed. SDK28 objects/two archives reused with strict hashes/all59 guards, not rebuilt here. Populated AArgScript elements remain excluded because their original constructor is unproven. SDK revision/internal layout and native STL ABI equality remain unestablished; prior OpenState/Build matching remain static-only. Unrelated fixture boundaries remain explicit.

Ledger: {n} structured evidence entries and {mapped} bounded implementation/test mappings over7238 entries,0warnings. Counts are not completion percentages or whole-server proof. Full NPC/VM dispatch, Confirm/RunState/Reward/EndRunning/stat/item/server/VisualStudio linking remain unfinished. Native Windows production support deferred; original ELF unexecuted/unchanged.

''';status.write_text(head+history,encoding='utf-8')
frontier=rec/'DEPENDENCY_FRONTIER.md';text=frontier.read_text(encoding='utf-8-sig').replace('after checkpoint100','after checkpoint101').replace('Verified baseline:113','Verified baseline:114');text=re.sub(r'ledger\d+ entries/\d+',f'ledger{n} entries/{mapped}',text)
start=text.find('Next useful investigation:');end=text.find('Name/index queries,',start)
if start>=0 and end>start:text=text[:start]+'Next useful investigation: complete original NPC/quest dispatch or other remaining dependency closure after fresh whole-callgraph review. Genuine CQM/NPC ownership is now verified101 and the former owning destructor fixture is retired. Populated AArgScript construction remains unproven; do not invent it to force runtime coverage. Full VM/Confirm/RunState/Reward/EndRunning/stat-item closure remains pending. '+text[end:]
text=text.replace('Full113 validation is complete.','Full114 validation is complete.');frontier.write_text(text,encoding='utf-8')
readme=root.parent/'README.md';text=readme.read_text(encoding='utf-8-sig').replace('checkpoint 100','checkpoint 101').replace('**113 regression test programs**','**114 regression test programs**');text=re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{n} entries with structured address-backed evidence**',text);text=re.sub(r'\*\*\d+ bounded',f'**{mapped} bounded',text)
note='Genuine CQM/NPC ownership now replaces the earlier owning destructor test boundary. Populated AArgScript construction and full VM/game dispatch remain outside the verified runtime scope.'
if note not in text:text+='\n'+note+'\n'
readme.write_text(text,encoding='utf-8')
resume=f'''# Metin2 reconstruction restart — checkpoint101, {date}

Project: PROJECT_ROOT\\Reversed Source
Reference: REFERENCE_PROJECT\\Source\\srv\\server
Workspace: {cwd}
Original ELF SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Never execute or modify it.

## Verified101: actual ownership, former boundary retired

Full114 exact ordered runtime commands plus ABI passed. Focused4: ownership, PC lookup, flags/timers and Lua queries plus ABI. Strict32 actual manager/NPC/test/ABI; five distinct new O2 SQL/observer objects and actual app graph with NPC.cpp exactly once passed. Helper9 includes SDK reuse and sealed PROOF_INDEX. Archive reconstruction/quest_ownership_evidence/RECORD_MANIFEST.json,COPY_INDEX.json/package/proofs/procedure. Source8/support67, independent static review hash archived. Ledger:{n} structured evidence/{mapped} bounded mappings/0warnings over7238; not completion percentage.

Eight genuine addresses represent four source semantics:
- CQM Destroy081a0720 (16instructions).
- CQM destructor D2 0819fa20 (272), D1 0819fe70 (272), D0 081a02c0 (274).
- NPC default ctor C2 081adf50 (95), C1 081ae0c0 (52).
- NPC destructor D2 081ae180 (72), D1 081ae260 (72).
NPC081ab460 (45instructions) is implicit COPY context only, NOT default ctor or a new standalone implementation/mapping. Source field set and original NPC484/CQM216 metadata remain complete; native STL binary layout equivalence is not asserted.

Actual Destroy checks L, calls actual SDK lua_close on current initial L, then writes L0 only after finalizers return. Replacement L assigned by a finalizer is overwritten by original final store; test retains and closes replacement separately. No added PC/NPC/script/context clear or timer cancellation. CQM destructor body Destroy then complete reverse member/base teardown. Singleton resets after fields. Raw server-map event pointers remain nonowned; tests explicitly cancel externally after manager destruction.

NPC default body m_vnum0 runs AFTER all20 plainmaps+20 argmaps construct. NPC destructor user body is genuinely empty, but compiler destroys all20 argmaps then20 plainmaps and nested owned values. This is actual production lifecycle, not a no-op. Ordinary binary script containers, all event slots, nested empty arg vectors/reserved capacity, implicit deep copy, real heap deletion and actual PC registered timer cancellation tested. Seed PC LastState and both QuestState counters BEFORE copies; no production zero repair.

The old quest_flag_fixture CQM destructor/Destroy replacements are removed unconditionally. Every13 manager literal (shared assignment/six recipes/six prerequisites) pairs actual questnpc.cpp; all old graphs use real ownership. Other unrelated OS/lifecycle fixtures remain explicit. Earlier PREPARED/UNAPPLIED/old-boundary-active notes are historical; retirement is supported only by this full114 source/proof record.

AArgScript original default constructor remains UNPROVEN. No populated byvalue AArgScript objects are manufactured for tests; empty vectors with capacity are legal storage-only inputs. QuestState chat_scripts owns pointer-vector storage, not pointees; NULL entries only in tested setup. No invented deleteArgScript or custom copyctor. Invalid/reentrant close, borrowed/corrupted context, allocation and concurrency domains remain excluded without repair.

## SDK and earlier constraints

SDK28 actual O2 objects/two archives reused under hashes/all59 current source/header/build/provenance guards;58 supplied source files still reference-identical. Old query application hashes ignored ONLY for SDK reuse; current app/fixture/global recipe bytes independently guarded and rebuilt. Actual Lua5.0.3 public opaque compatibility is not exact original advertised5.0 revision/internal ABI recovery. MinGW32 state96/TObject16(value8) vsold92/12(value4); Number8/CallInfo24 match. Failed internal probes remain diagnostic, no packing repair or SDK target mappings/fake LuaClose.

100whole ConfirmWithMsg/style leaves/full74packet retained; no full Confirm dispatch/timeout proof.99yield/predicate/diagnostic: CanStart missingtrue, ignored dobuffer status, current top truthiness and restore; actualSDK aux_do error consumption; emptyvector &v[0] unsafe excluded. QuestError does NOT set error/script flags.98genuine CQM methods in questlua.cpp per raw owner audit; OpenState STATICONLY original direct sret vsoptional NRVO/default indeterminate counter copies; unsuppressed empty &script[0] excluded.97PC query consumers/network;96CQM query/event/full37record, repeatedsingleton loads static only;95Build matching STATICONLY even duplicates/defaultcountercopies, runtimeempty/nonmatching; queue producers do not grant rewards.94Save/static vector tails/DB reload/liveclear;93questinfo header.size6;92queries/hash/storage/loader/register retained. ClearQuest live-key UAF/default counters/text-mode host differences remain.

## Remaining goal / next work

Do not reapply101 payloads or rerun its finalizer. Fresh original full-body/API/data/callee review and current guards required before new changes. Full NPC matching/dispatch/LevelUp, Confirm/RunState/InitializeLua/coroutine execution, Reward/EndRunning, broad SetEventFlag effects, complete PointChange116cases127targets/stat/item/removal/affects/combat/Safebox/Boot/liveDB/server and VisualStudio linking pending. No partial gameplay switches, fake VM or invented AArgScript constructor. Native Windows production support deferred; preserve file layout, complete owners/data/API and original bugs.

## Tools and continuation

Python USER_HOME\\.cache\\codex-runtimes\\codex-primary-runtime\\dependencies\\python\\python.exe; GCC/G++/gmake C:\\Strawberry\\c\\bin. PYTHONPATH work/python-deps forELFtools. Canonical reconstruction/tools/validate.py run explicit toolchain/unique log --suite NAME --abi or --full. Actual terminal exit + completed receipt required. No overlapping builds or source mutation. NativeF outputs work; nativeC creation unreliable. Reverse-engineer-anything skill applied; first liveGhidra get_program_info, reuse hash-matched saved originals. Old low-level diagnostic cyberflag was not malware finding; do not repeat blocked diagnostic.

Prior allowance-stop notes were superseded by userContinue and this101 work. Read current primary/weekly limits independently before expansion, keep both restart files identical and save exact active process/receipt state near limits. User authorizes autonomous work/subagents; ask only genuine missing input. No reset-credit/account changes authorized.
'''
(cwd/'outputs/METIN2_RESUME.md').write_text(resume,encoding='utf-8');(rec/'RESUME_CHECKPOINT.md').write_text(resume,encoding='utf-8');print('Updated101/full114 docs + identical restart files; real owning fixture retirement and constructor/static/SDK limits explicit.')
