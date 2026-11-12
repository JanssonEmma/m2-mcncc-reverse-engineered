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
ledger=load(rec/'function-coverage-ledger.json');manifest=load(rec/'quest_save_evidence/RECORD_MANIFEST.json');statusjson=load(rec/'validation-status.json')
assert manifest['checkpoint']==94 and manifest['status']=='passed' and not ledger['warnings']
assert statusjson['checkpoint94_quest_save']['status']=='passed' and statusjson['runtime_suite_count']==104
full=load(w/'checkpoint-94-validation.receipt.json');assert full['status']=='passed' and full['exit_code']==0 and full['suites']==full['executed_runtime_commands'] and len(full['suites'])==104
evidence=ledger['evidence_available'];mapped=ledger['summary']['reviewed_implementation_test_mapping'];date=full['finished_utc'][:10]
status=rec/'CURRENT_STATUS.md';text=status.read_text(encoding='utf-8-sig');history=text[text.index('---'):] if '---' in text else '\n---\n'+text
head=f'''# Current reconstruction status — {date}

Latest verified checkpoint: **94**. All **104** registered runtime commands and ABI checks passed. Proof: `quest_save_evidence/proofs/checkpoint-94-validation.receipt.json`. This batch restores the whole original PC::Save08060770 and full packed106-byte SQuestTable/TQuestTable. Focused3+ABI, strict C++03/32-bit syntax and optimized actual PC/event/manager/CLIENT_DESC/buffer graph with5 distinct optimized SQL/observer objects passed.

Save preserves the original static vector, stale row tails/zero-grown rows, length-vs-strcpy behavior, diagnostics, cached pre-log value/current PID reload, separate actual DB header/body calls with late global pointer reload and final live pending-map clear. The record retains original DWORD/long32 fields,33/65-byte arrays and offsets0/4/37/102. Undefined callback iterator/vector/reentry domains remain unchanged and are not forced by tests. No invented named quest/packet enum or production repair.

Coverage ledger: {evidence} structured address-backed evidence entries and {mapped} bounded implementation/test mappings over7238 entries,0 warnings. Counts are not a completion percentage or whole-server equivalence proof.

Full server compilation/linking, Lua/VM dispatch and Visual Studio integration remain unfinished. Native Windows production support is deferred. The original ELF remains unexecuted and unchanged. Read `DEPENDENCY_FRONTIER.md` and `RESUME_CHECKPOINT.md` for the next closure.

'''
status.write_text(head+history,encoding='utf-8')
frontier=rec/'DEPENDENCY_FRONTIER.md';text=frontier.read_text(encoding='utf-8-sig');text=text.replace('after checkpoint93','after checkpoint94').replace('Verified baseline:103','Verified baseline:104')
text=re.sub(r'ledger\d+ entries/\d+',f'ledger{evidence} entries/{mapped}',text)
start=text.find('Next useful investigation:');end=text.find('Name/index queries,',start)
if start>=0 and end>start:text=text[:start]+'Next useful investigation: whole PC::Build, CancelRunning or SetLoaded after fresh original instruction/inline evidence and complete dependency review. PC::Save checkpoint94 is verified; do not reapply stale staging payload. '+text[end:]
text=text.replace('Build/Save','Build').replace('Full103 validation is complete.','Full104 validation is complete.')
frontier.write_text(text,encoding='utf-8')
readme=root.parent/'README.md';text=readme.read_text(encoding='utf-8-sig');text=text.replace('checkpoint 93','checkpoint 94').replace('**103 regression test programs**','**104 regression test programs**');text=re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{evidence} entries with structured address-backed evidence**',text);text=re.sub(r'\*\*\d+ bounded',f'**{mapped} bounded',text);readme.write_text(text,encoding='utf-8')
resume=f'''# Metin2 reconstruction restart — checkpoint94, {date}

Project: PROJECT_ROOT\\Reversed Source
Reference: REFERENCE_PROJECT\\Source\\srv\\server
Workspace: {cwd}
Original ELF SHA256:61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Never execute/modify it.

## Verified state
Checkpoint94 passed104 exact ordered runtime commands plus ABI. Focused3+ABI, strict32 and5distinct optimized SQL/observer objects plus actual graph O2 build/runtime passed. Proof archive: reconstruction/quest_save_evidence/RECORD_MANIFEST.json and COPY_INDEX.json. Root source-guards explicitly review6final paths/5dependencies and test-only/helper/plan preparation changes; original stale MANIFEST retained as provenance. Current ledger:{evidence} structured evidence/{mapped} bounded mappings/0warnings over7238. These counts are not a completion percentage.

Whole PC::Save08060770 and full original packed106-byte SQuestTable/TQuestTable integrated. Record DWORD/long4 at0/102 and char33/65 at4/37 preserved, no host type substitution or invented named packet enum. Static vector persists across calls/PC instances; shrink/regrow preserves prefix and zero-grows rows. First-dot parsing uses full string lengths, later strcpy remains original. Diagnostics precede field acceptance; value cached before callback, PID reloaded later. Actual CLIENT_DESC::DBPacketHeader numeric11/handle0 followed by actual Packet reloads global descriptor and static vector base. Entire live pending map clears after Packet, main flag map retained. Unsafe map/vector invalidation/reentrant/overflow/null domains preserved, not forced. No full old COW/SDK/FreeBSD equivalence claim.

Checkpoint93 whole13 quest-info methods/packed6-byte packet/inline nonconst manager context getter remains verified. Sender wire size6/live callback reads/fixed widths/pool/mask clear retained. No fake enum/ClearSendFlag. Checkpoint92 full4manager queries/hash,7PC storage+4inline helpers, LoadStartQuest/RegisterQuest retained. Original ClearQuest live-key UAF/default uninitialized counters/text-mode host differences remain. Full PC61 API/data, complete CQMdata216 and original97-entry API metadata retained; CQM header API remains partial declarations.

## Next work
Review work/quest_next_frontier.md and fresh original evidence. PC::Build, CancelRunning or SetLoaded are potential next bounded leaves; no implementation until complete original body/inline/constructor/callee scope established. Do not rerun work/pc_quest_save apply/prepare or checkpoint94 finalize scripts: this module already applied/recorded. Production sources require fresh guards, serialized builds and actual callee graph; one new canonical suite would raise105 only after proof.

Reward/EndRunning/LevelUp still require full stat/item/Lua closure. Full PointChange116cases/127targets, ApplyPoint/ModifyPoints/ComputePoints/ComputeBattlePoints/affect/combat/removal candidates staged; no partial switch/application stubs. Safebox populated destructor/remove/move, complete Boot/startup/liveDB/server linking and VisualStudio integration pending. Native Windows production support deferred.

## Workflow/tools/allowance
Preserve source layout and complete original owners/API/data/bugs. Test-only unrelated lifecycle/OS/MySQL/fail-fast boundaries must remain explicit. No fake gameplay/VM/loader/stat methods. No overlapping builds or source/header changes during builds. Original binary remains unexecuted.

Python USER_HOME\\.cache\\codex-runtimes\\codex-primary-runtime\\dependencies\\python\\python.exe; GCC/G++/gmake C:\\Strawberry\\c\\bin. PYTHONPATH workspace work/python-deps forELFtools. Canonical runner reconstruction/tools/validate.py run --suite NAME --abi or --full with explicit make/cc/cxx and unique log; inspect final receipts and terminalexit. NativeF output works; nativeC creation unreliable, prefer Python/PowerShell workspace writes. Reverse-engineer-anything skill applied; first liveGhidra call get_program_info. Reuse hash-matched saved evidence. Old lowlevel memory-diagnostic cyberflag is not a malware finding; do not repeat blocked diagnostic.

Check current tool allowance before new expansion; previous weekly allowance was close, and5h reset does not reset weekly limit. No reset credit/account change authorized. User authorizes autonomous work/subagents, asks to save state near allowance limit, and only needs questions for real missing input.
'''
(cwd/'outputs/METIN2_RESUME.md').write_text(resume,encoding='utf-8');(rec/'RESUME_CHECKPOINT.md').write_text(resume,encoding='utf-8')
print('Updated current status/frontier/README and identical checkpoint94 restart files.')
