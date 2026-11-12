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
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'))
m=load(rec/'quest_constructor_context_evidence/RECORD_MANIFEST.json');st=load(rec/'validation-status.json');ledger=load(rec/'function-coverage-ledger.json')
assert m['status']=='passed_focused' and m['validation_scope']=='focused_only' and m['registered_runtime_commands']==116 and m['focused_runtime_commands']==4
assert st['checkpoint102_constructor_context']['status']=='passed_focused' and st['registered_runtime_suite_count']==116 and st['runtime_suite_count']==114 and st['runtime_suites']==m['last_full_validation']['suites'] and not ledger['warnings']
assert m['last_full_validation']['checkpoint']==101 and m['last_full_validation']['runtime_commands']==114
n=ledger['evidence_available'];mapped=ledger['summary']['reviewed_implementation_test_mapping'];date=st['date'];status=rec/'CURRENT_STATUS.md';text=status.read_text(encoding='utf-8-sig');history=text[text.index('---'):]if'---'in text else'\n---\n'+text
head=f'''# Current reconstruction status — {date}

Latest recorded checkpoint: **102, focused validation passed**. **116 runtime suites are registered;4 focused suites plus relevant ABI, strict32 and optimized checks passed.** The latest **full** run remains **checkpoint101:114 suites on its prior source state**. No full116 run was performed or claimed. See `VALIDATION_POLICY.md`.

This group restores four whole NPC VID/context/race methods and the genuine inline AArgScript default constructor. Full STL members initialize; quest_index/state_index remain indeterminate. Tests set both before copying/populating vectors. Original loader default-resize copies remain an excluded unsafe domain. The inline range is not a standalone constructor address.

Focused suites cover constructor, NPC context, real ownership and PC storage. Constructor helper passed3 strict/O2 checks; NPC helper passed8 checks plus strict SDK reuse, sealed by2 serial wrapper receipts. Original double lookup/singleton assertion roles remain static evidence, not invented Find callbacks. A header-only include-order fix spells equivalent CHARACTER* in place of unavailable LPCHARACTER; initial failure and exact2 reviewed combined-package changes are archived.

Latest ledger:{n} structured evidence/{mapped} bounded mappings over7238 entries,0warnings. These are not completion percentages. SDK revision/internal layout and prior OpenState/Build static-only limits remain. Full VM/dispatch/Reward/EndRunning/stat/item/server/VisualStudio linking is unfinished; native Windows production support deferred. Original ELF unexecuted/unchanged.

''';status.write_text(head+history,encoding='utf-8')
f=rec/'DEPENDENCY_FRONTIER.md';text=f.read_text(encoding='utf-8-sig').replace('after checkpoint101','after checkpoint102');text=re.sub(r'^Verified baseline:.*$',f'Registered116. Checkpoint102 focused4+ABI/strict/O2 passed; lastFULL101/114 is historical prior-source validation. Ledger{n} evidence/{mapped} bounded mappings/0warnings. No full116 claim.',text,flags=re.M)
text=text.replace('Populated AArgScript construction remains unproven; do not invent it to force runtime coverage.','AArgScript default inline is recovered; initialize both scalar fields before copies. Original default-resize indeterminate copies remain unsafe/unforced.').replace('Full114 validation is complete.','Full114 remains the last full validation at101; current102 has focused proof and116 registered suites.');f.write_text(text,encoding='utf-8')
r=root.parent/'README.md';text=r.read_text(encoding='utf-8-sig').replace('**checkpoint 101**','**checkpoint 102 (focused validation)**');text=re.sub(r'As of \*\*[^*]+\*\*, the latest verified milestone',f'As of **{date}**, the latest recorded milestone',text)
text=text.replace('- **114 regression test programs** and the registered ABI checks passed.','- **116 regression programs are registered.** Checkpoint102 passed **4 focused runtime suites** plus relevant ABI/strict/O2 checks. The last full run remains **checkpoint101:114 suites** on its prior source state; no full116 pass is claimed.')
text=re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{n} entries with structured address-backed evidence**',text);text=re.sub(r'\*\*\d+ bounded',f'**{mapped} bounded',text)
text=text.replace('run --full --make gmake','run --suite quest_npc_context_regression --abi --make gmake').replace('Populated AArgScript construction and full VM/game dispatch remain outside the verified runtime scope.','AArgScript default construction is now recovered; tests initialize scalars before populated copies. Original default-resize unsafe copies and full VM/game dispatch remain outside the verified runtime scope.')
if 'Use focused suites for routine changes'not in text:text+='\nUse focused suites for routine changes and run full-suite checks at risk-based batch milestones. Registered suite counts and the latest full result are reported separately; see the reconstruction validation policy.\n'
r.write_text(text,encoding='utf-8');(rec/'VALIDATION_POLICY.md').write_text((p/'VALIDATION_POLICY.md').read_text(encoding='utf-8-sig'),encoding='utf-8')
resume=f'''# Metin2 reconstruction restart — checkpoint102, {date}

Project: PROJECT_ROOT\\Reversed Source
Reference: REFERENCE_PROJECT\\Source\\srv\\server
Workspace: {w.parent}
Original ELF SHA256:61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Never execute/modify it.

## Validation scope — keep these counts separate

Current registry116. Checkpoint102 PASSED FOCUSED4 normal runtime suites plus ABI, constructor strict/O2 helper3, NPC strict/O2 helper8 plus SDKreuse, and2 serial wrapper receipts/combined index. Focused proof work/quest-constructor-context-focused-retry.receipt.json. NO full116 run. Last full validation remains checkpoint101/full114 on its earlier source state, receipt work/checkpoint-101-validation.receipt.json. Legacy runtime_suites/count/full log fields retain that114 result; separate registered116/latest-focused fields describe102. User requested risk-based focused checks and batched full milestones; read reconstruction/VALIDATION_POLICY.md.

Archive reconstruction/quest_constructor_context_evidence/RECORD_MANIFEST.json records scope, hashes and reviewed fix. Ledger:{n} structured evidence/{mapped} bounded mappings/0warnings over7238 entries; no completion-percent or whole-server equivalence inference.

## Integrated102

Four whole methods: CHARACTER SetQuestNPCID08134f30(7instructions), GetQuestNPC08134f50(24), CQM GetCurrentNPCCharacterPtr081a5700(13), GetCurrentNPCRace081a5660(21). Actual DWORD VID store, CHARACTER_MANAGER Find including0, null current context, unsigned race return through existing WORD source. Original singleton assertion and two current-NPC lookups are STATIC evidence; runtime changes context/registries between ordinary calls, no invented Find callback/concurrent mutation. Full original owners/data/API remain.

AArgScript original inline default ctor quest.h99 is empty user body, complete implicit string/vectors initialize. Both scalar indices remain INDETERMINATE. Inline081af25d..081af263 in original LoadStateScript, not a standalone address; compiler destructor/copy helpers are context only. Tests initialize BOTH scalars before copy/assignment/vector insert, then exercise actual populated NPC argument containers. Original LoadStateScript default-resize indeterminate-copy domain remains unsafe/unforced. No zero defaults, replacement copyctor or explicit destructor added. Old target record36 vsmodern native STL layout is evidence, not a size-equality claim.

Combined package work/checkpoint102_quest_constructor_context remains frozen except EXACT2 root-reviewed changes: integration/plan.json and manager-header payload. Standalone header had no LPCHARACTER alias (later reference obtains it via stdafx/typedef); equivalent CHARACTER* declaration fixes include order without behavior/layout/API type change. Authored packages unchanged. Initial failed focused receipt/log and reviewed_fixes header before/after/original plan archived;4 independent reviews hashed. Current source11/support74 guards are final authority; do not rerender stale standalone115 or merged116 payloads.

## Preserved SDK/ownership/static boundaries

Actual SDK5.0.3 public opaque compatibility only, not exact original advertised5.0/internal ABI. MinGW32 state96/TObject16 vsold92/12, Number8/CallInfo24 match. SDK28objects/2archives reused strictly with all59 current source/header/build guards; old query application-source hashes ignored ONLY for SDKreuse. No SDK recompilation claim, packing repair or fake LuaClose.

101 genuine CQM Destroy/dtor and NPC ctor/dtor,8 original variants/4 semantics, full114. Former owning destructor fixture retired, global13 manager/NPC recipe pairings. NPC081ab460 is implicitCOPY context only. Destroy actualclose beforeL0; full automatic member/base teardown; raw server-map events nonowned. AArgScript constructor's earlier unproven status is now superseded only by102's genuine inline proof and initialized-copy tests.

100Confirm/style/full74packet;99yield/predicate/diagnostic and unsafe present-empty &vector[0] excluded, ignored dobuffer status/actualaux_do error consumption retained, QuestError no errorflag writes.98Open STATICONLY original direct sret vsoptional NRVO/default QuestState counters; unsuppressed empty &script[0] excluded.95Build matching STATICONLY even duplicates, source whole/runtimeempty-nonmatching only. ClearQuest live-key UAF, default QuestState counters, raw borrowed Lua lifetimes, invalid contexts and other original unsafe domains remain unrepaired. Full SDK/old STL ABI equality unestablished.

## Remaining and continuation

No checkpoint102 full-suite claim may be inferred from combined116 index names. Do not rerun applied preparation/finalization. Select affected tests from actual dependencies; broaden for failures/global ownership/ABI/recipe changes and batch full runs at milestones/before readiness claims. Preserve last-full provenance separately.

Full NPC/VM dispatch, Confirm/RunState/InitializeLua/coroutines, Reward/EndRunning/LevelUp, broad SetEventFlag, full PointChange/stat/item/removal/affect/combat/Safebox/Boot/liveDB/server/VisualStudio linking pending. Native Windows production support deferred. No partial gameplay methods or fake VM. Preserve file layout, complete owners/API/data and original bugs.

Python USER_HOME\\.cache\\codex-runtimes\\codex-primary-runtime\\dependencies\\python\\python.exe; GCC/G++/gmake C:\\Strawberry\\c\\bin. Canonical runner reconstruction/tools/validate.py run --suite NAME --abi (repeat affected suites), explicit toolchain/unique log. --full only when policy warrants. No overlapping builds/source changes. NativeF outputs work/nativeC creation unreliable. First liveGhidra get_program_info; reuse hash-matched originals. Old diagnostic cyberflag was not malware finding; do not repeat blocked low-level diagnostic.

User authorizes autonomous work/subagents; save exact state near allowance limits and ask only real missing input. Primary/weekly windows separate; no reset-credit/account changes authorized. Keep both restart files identical.
'''
(w.parent/'outputs/METIN2_RESUME.md').write_text(resume,encoding='utf-8');(rec/'RESUME_CHECKPOINT.md').write_text(resume,encoding='utf-8');print('Updated102 FOCUSED docs: registered116 vs lastFULL101/114; identical restart files.')
