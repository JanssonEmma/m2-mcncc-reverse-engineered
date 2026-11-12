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
import hashlib
import json
import re
import shutil
import subprocess
import sys

root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
rec=root/'reconstruction'
work=Path(__file__).resolve().parent
proof=work.parent/'checkpoint-106-retry.receipt.json'

def read(p):
    return json.loads(p.read_text(encoding='utf-8-sig'))

def write(p, data):
    p.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()

receipt=read(proof)
suites=['quest_lua_queries_regression','quest_registration_regression','quest_npc_set_regression']
assert receipt['status']=='passed' and receipt['scope']=='focused' and receipt['exit_code']==0 and receipt['abi_requested']
assert receipt['suites']==receipt['executed_runtime_commands']==suites
applied=read(work/'applied.json')
assert all(sha(root/row['path'])==row['after'] for row in applied)
status=read(rec/'validation-status.json')
assert status['checkpoint105_npc_set_callers']['status']=='passed_focused'
assert status['registered_runtime_suite_count']==118 and status['last_full_validation']['checkpoint']==103
archive=rec/'quest_lua_startup_helpers_evidence'
assert not archive.exists()
shutil.copytree(work,archive)
shutil.copy2(proof,archive/proof.name)
shutil.copy2(work.parent/'checkpoint-106-focused.receipt.json',archive/'initial-link-failure.receipt.json')
shutil.copy2(Path(receipt['log']),archive/'checkpoint-106-retry.log')
for name in ('CURRENT_STATUS.md','DEPENDENCY_FRONTIER.md','RESUME_CHECKPOINT.md','function-coverage-overrides.json'):
    shutil.copy2(rec/name,archive/(name+'.before106'))
overrides_path=rec/'function-coverage-overrides.json'
overrides=read(overrides_path)
target=overrides['target_sha256']
originals=[]
for address, method in [('0805a5e0','AddLuaFunctionTable'),('0805a690','BuildStateIndexToName')]:
    original=read(work/(address+'-decompile.json'))
    assert original['function']['address']==address and not original['truncated']
    originals.append(original)
    assert not any(row['address']==address for row in overrides['reviews'])
    overrides['reviews'].append({
        'address':address,
        'source_refs':[{'path':'game/src/questlua.cpp','anchor':'CQuestManager::'+method+'('}],
        'test_refs':[{'path':'reconstruction/tests/quest_lua_queries_regression.cpp','anchor':'static void TestStartupHelpers('}],
        'review_scope':'Whole original Lua binding table installation and reverse state mapping with the actual Lua SDK.',
        'reviewer':'Checkpoint 106 full assembly review and focused tests',
        'limitations':'Tests use existing reverse slots to avoid changing the key set during Lua traversal. Full startup and registration callbacks remain pending.'
    })
write(rec/'quest-lua-startup-helpers-ghidra.json',{'target_sha256':target,'functions':originals})
write(overrides_path,overrides)
subprocess.run([sys.executable,str(rec/'tools/update_coverage.py'),'--inventory',str(rec/'target-function-inventory.json')],check=True)
ledger=read(rec/'function-coverage-ledger.json')
assert not ledger['warnings']
manifest={'checkpoint':106,'status':'passed_focused','validation_scope':'focused_only',
          'focused_runtime_commands':suites,'registered_runtime_commands':118,'abi':True,'source_hashes':applied,
          'receipt_sha256':sha(proof),'last_full_validation':status['last_full_validation'],'coverage':ledger['summary']}
write(archive/'RECORD_MANIFEST.json',manifest)
status.update(latest_validation_scope='focused_only',latest_validation_receipt=str(proof.resolve()),
              latest_focused_receipt=str(proof.resolve()),latest_focused_suites=suites,
              latest_focused_check_exit_code=0,checkpoint106_quest_lua_startup_helpers=manifest)
write(rec/'validation-status.json',status)
summary=f'Checkpoint 106 restored AddLuaFunctionTable and BuildStateIndexToName. Three focused runtime suites and ABI checks passed. There are 118 registered suites. Checkpoint 103 remains the latest full run with 117 suites. The ledger contains {ledger["evidence_available"]} functions with evidence and {ledger["summary"]["reviewed_implementation_test_mapping"]} bounded source/test mappings.'
for name in ('CURRENT_STATUS.md','DEPENDENCY_FRONTIER.md'):
    p=rec/name
    p.write_text('# Checkpoint 106\n\n'+summary+'\n\nHistorical checkpoint notes follow.\n\n'+p.read_text(encoding='utf-8-sig'),encoding='utf-8')
p=rec/'RESUME_CHECKPOINT.md'
text='# Metin2 reconstruction restart — checkpoint 106\n\n'+summary+'''

The Lua startup helpers now use the supplied SDK to publish fresh callback tables and build reverse quest-state lookups. The tests check duplicate and sentinel entries, ordinary global metamethod dispatch, numeric and coercible-string reverse keys, stack preservation and the missing-table QuestError. Source, evidence and proof are archived under reconstruction/quest_lua_startup_helpers_evidence.

Next review the config map_allow cluster staged for SetEventFlag, then continue complete registration callbacks and VM startup dependencies. Full dispatch, reward/stat/item chains and server linking remain in scope. Keep the original executable unmodified and unexecuted. Follow reconstruction/VALIDATION_POLICY.md for batching.
'''
p.write_text(text,encoding='utf-8')
(work.parents[1]/'outputs/METIN2_RESUME.md').write_text(text,encoding='utf-8')
p=root.parent/'README.md'
s=p.read_text(encoding='utf-8-sig')
s=re.sub(r'As of \*\*[^*]+\*\*, the latest recorded milestone is .*?:', 'As of **2026-11-09**, the latest recorded milestone is **checkpoint 106 (focused validation)**.',s,count=1)
s=re.sub(r'- \*\*118 regression programs[^\n]*', '- **118 regression programs are registered.** Checkpoint 106 passed **3 focused runtime suites** and ABI checks. The latest full run passed **117 suites** at checkpoint 103.',s,count=1,flags=re.S)
s=re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{ledger["evidence_available"]} entries with structured address-backed evidence**',s)
s=re.sub(r'\*\*\d+ bounded',f'**{ledger["summary"]["reviewed_implementation_test_mapping"]} bounded',s)
p.write_text(s,encoding='utf-8')
print(summary)

