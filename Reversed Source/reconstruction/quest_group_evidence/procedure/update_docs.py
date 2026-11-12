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
ledger=load(rec/'function-coverage-ledger.json');manifest=load(rec/'quest_group_evidence/RECORD_MANIFEST.json');statusjson=load(rec/'validation-status.json')
assert manifest['checkpoint']==95 and manifest['status']=='passed' and not ledger['warnings']
assert manifest['runtime_scope']['Build_matching_suffix']=='static_only_indeterminate_counter_copy'
assert statusjson['checkpoint95_quest_group']['status']=='passed' and statusjson['runtime_suite_count']==106
full=load(w/'checkpoint-95-validation.receipt.json');assert full['status']=='passed' and full['exit_code']==0 and full['suites']==full['executed_runtime_commands'] and len(full['suites'])==106
evidence=ledger['evidence_available'];mapped=ledger['summary']['reviewed_implementation_test_mapping'];date=full['finished_utc'][:10]
status=rec/'CURRENT_STATUS.md';text=status.read_text(encoding='utf-8-sig');history=text[text.index('---'):] if '---' in text else '\n---\n'+text
head=f'''# Current reconstruction status — {date}

Latest verified checkpoint: **95**. All **106** registered runtime commands and ABI checks passed. Proof: `quest_group_evidence/proofs/checkpoint-95-validation.receipt.json`. This group restores four complete original addressed methods: PC::Build, CancelRunning, GiveItem and GiveExp, plus the original true-byte SetLoaded inline. Focused4+ABI, strict C++03/32-bit syntax and optimized actual graphs with5 distinct optimized SQL/observer objects pergroup passed.

**Build matching branches are static-only.** The whole original body remains present, including .__status matching/query/state-copy/insert logic; copying the default QuestState reads original indeterminate counters even for duplicate keys. Runtime checks cover only empty/nonmatching gates. No zero initialization, replacement copy or narrowed body was added. CancelRunning preserves its exact three stores. SetLoaded stores true and does not call Build. GiveItem/GiveExp queue actual RewardData after original log/live lookup/callback ordering; duplicate flags set a sticky bool. They do not execute rewards, grant items/XP or run Lua.

Coverage ledger: {evidence} structured address-backed evidence entries and {mapped} bounded implementation/test mappings over7238 entries,0 warnings. Counts are not completion percentages or whole-server equivalence proof.

Full server compilation/linking, Lua/VM/stat/reward execution and Visual Studio integration remain unfinished. Native Windows production support is deferred. The original ELF remains unexecuted and unchanged. See `DEPENDENCY_FRONTIER.md` and `RESUME_CHECKPOINT.md`.

'''
status.write_text(head+history,encoding='utf-8')
frontier=rec/'DEPENDENCY_FRONTIER.md';text=frontier.read_text(encoding='utf-8-sig');text=text.replace('after checkpoint94','after checkpoint95').replace('Verified baseline:104','Verified baseline:106');text=re.sub(r'ledger\d+ entries/\d+',f'ledger{evidence} entries/{mapped}',text)
text=text.replace('Build/EndRunning/reward/LevelUp/NPC dispatch and VM ownership','Build matching runtime remains unproven; EndRunning/reward execution/LevelUp/NPC dispatch and VM ownership')
text=text.replace('and13 quest-info setters/protocol bodies verified','and13 quest-info setters/protocol bodies verified; whole Save/Build/CancelRunning, SetLoaded inline and GiveItem/GiveExp producers integrated (Build matching static-only)')
start=text.find('Next useful investigation:');end=text.find('Name/index queries,',start)
if start>=0 and end>start:text=text[:start]+'Next useful investigation: complete remaining quest/combat/stat/Lua dependency closure after full original callgraph review; queue producers are integrated, actual Reward/EndRunning remain unresolved. Build matching branches are static-only due original indeterminate-counter copy; do not force them or add zero repairs. All95 staging has been applied/recorded; do not reapply stale whole-source plans. '+text[end:]
text=text.replace('No current build/agent is active after checkpoint94.','Full106 validation is complete. Consult restart state for subsequent work.');frontier.write_text(text,encoding='utf-8')
readme=root.parent/'README.md';text=readme.read_text(encoding='utf-8-sig');text=text.replace('checkpoint 94','checkpoint 95').replace('**104 regression test programs**','**106 regression test programs**');text=re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{evidence} entries with structured address-backed evidence**',text);text=re.sub(r'\*\*\d+ bounded',f'**{mapped} bounded',text)
limit='Build matching branches are supported by complete static evidence only; runtime tests cover empty/nonmatching branches because the original default-state copy reads indeterminate counters.'
if limit not in text:text+='\n'+limit+'\n'
readme.write_text(text,encoding='utf-8')
resume=f'''# Metin2 reconstruction restart — checkpoint95, {date}

Project: PROJECT_ROOT\\Reversed Source
Reference: REFERENCE_PROJECT\\Source\\srv\\server
Workspace: {cwd}
Original ELF SHA256:61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Never execute/modify it.

## Verified95 state and proof boundary
Checkpoint95 passed106 exact ordered runtime commands plus ABI. Focused4 (producer/build/Save/statehelpers)+ABI, both complete strict32 checks and actual O2 graph runtime checks with5distinct optimized SQL/observer objects pergroup passed. Archive reconstruction/quest_group_evidence/RECORD_MANIFEST.json and COPY_INDEX.json preserve both packages, grouped final plan/sourceguards, original4complete exports+instruction evidence, trueinline evidence and all receipts/logs. Actual ledger:{evidence} structured evidence/{mapped} bounded mappings/0warnings over7238. Counts are not completion percentages.

Whole PC::Build08061310(323instructions), CancelRunning080603c0(8), GiveItem08060e60(106) and GiveExp08060fd0(103), plus original SetLoaded inline080e9b5e..080e9b62 integrated. SetLoaded is true-byte store PC+0x5c, separate from caller's Build call; no invented standalone address. CancelRunning only runningNULL/send0/donefalse stores in order, unrelated state preserved.

BUILD MATCHING IS STATIC-ONLY: every qualifying .__status branch constructs default QuestState then copies its original indeterminate counters, even on duplicate index. Whole query/value reload/state/copy/uniqueinsert body preserved. Runtime executes only empty/nonmatching gates; no unknown0/duplicate/state-insert runtime proof or copy portability claim. Never add zero fields, replacement copyctor, selective copy or empty-only implementation to force tests.

GiveItem/GiveExp preserve original log-before-live quest/label query, actual GetFlag callback reads, actual RewardData vector queueing forzero and sticky rewardbool fornonzero. Original numeric/label domains, duplicate queueing and absence of flagwrites preserved. Producers do NOT execute Reward/AutoGiveItem/PointChange/Lua or grant items/XP. NativeOS/MySQL/log observers and unrelated lifecycle boundaries stay explicit; no shadow gameplay/VM methods.

## Prior verified state
94: full Save08060770, original packed106-byte SQuestTable/TQuestTable with DWORD/long4 at0/102 and char33/65 at4/37. Static vector stale tails/zero-grown rows, first-dot full-string lengths vsstrcpy, pre-log cached value/currentPID reload, real separate DB header/body with globaldesc reload, final livepending-map clear preserved. Unsafe callback iterator/vector/reentry/null/overflow domains not forced.
93: whole13 quest-info methods, packed6-byte packet, realinline current-char getter. Original wireheader.size6/live callback reads/fixed widths/pool/maskclear retained; no fake enum/ClearSendFlag.
92:4manager queries/hash,7PCstorage+4inline helpers, whole LoadStartQuest/RegisterQuest. ClearQuest live-key UAF/default uninitialized counters/text-mode SDK hostdifferences preserved. Full PC61 API/data, CQM data216 and original97-entry API metadata retained; CQM header remains partialAPI declarations.

## Next and remaining closures
Do not rerun95 packageprepare/apply or checkpoint95 finalize scripts: grouped9paths already applied/recorded; individual105draft whole-source plans are stale. Fresh guarded changes must preserve full original owners/source structure/behavior and serialize builds. Next modules require complete original body/inline/data/callee review before implementation. No full105run was needed:104→106 grouped two suites.

Reward/EndRunning/LevelUp still require complete stat/item/Lua/NPC/coroutine ownership closure. Full PointChange116cases/127targets, ApplyPoint/ModifyPoints/ComputePoints/ComputeBattlePoints/affect/combat/removal candidates staged; no partial switches or applicationstubs. Safebox populated destructor/remove/move, complete Boot/startup/liveDB/fullserver and VisualStudio link pending. Native Windows production support deferred. Original unsafe domains are evidence, not invitations for silent modern fixes.

## Tools/workflow/allowance
Python USER_HOME\\.cache\\codex-runtimes\\codex-primary-runtime\\dependencies\\python\\python.exe; GCC/G++/gmake C:\\Strawberry\\c\\bin. PYTHONPATH workspace work/python-deps forELFtools. Canonical reconstruction/tools/validate.py run --suite NAME --abi or --full uses explicit make/cc/cxx and unique logs. Inspect finalreceipts and terminalexit; no overlapping build/source mutation. NativeF outputs work; nativeC creation unreliable. Reverse-engineer-anything skill applied; firstliveGhidra get_program_info, reuse saved hash-matched originals. Don't repeat blocked lowlevelmemorydiagnostic; oldcyberflag was not malwarefinding.

Read current allowance tool before newexpansion.5h reset does not reset weeklywindow. No resetcredit/account change authorized. User authorizes autonomous work/subagents and asks to save state nearallowance limits; ask only real missinginput.
'''
(cwd/'outputs/METIN2_RESUME.md').write_text(resume,encoding='utf-8');(rec/'RESUME_CHECKPOINT.md').write_text(resume,encoding='utf-8')
print('Updated current status/frontier/README and identical checkpoint95 restart files; Build static-only limitation explicit.')
