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
proof=work.parent/'checkpoint-105-focused.receipt.json'

def read(p):
    return json.loads(p.read_text(encoding='utf-8-sig'))

def write(p, data):
    p.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()

receipt=read(proof)
suites=['quest_npc_set_regression','quest_registration_regression','quest_ownership_regression']
assert receipt['status']=='passed' and receipt['scope']=='focused' and receipt['exit_code']==0 and receipt['abi_requested']
assert receipt['suites']==receipt['executed_runtime_commands']==suites
applied=read(work/'applied.json')
assert all(sha(root/row['path'])==row['after'] for row in applied)
status=read(rec/'validation-status.json')
assert status['checkpoint104_npc_set']['status']=='passed_focused'
assert status['registered_runtime_suite_count']==118 and status['last_full_validation']['checkpoint']==103
archive=rec/'npc_set_callers_evidence'
assert not archive.exists()
shutil.copytree(work,archive)
shutil.copy2(proof,archive/proof.name)
shutil.copy2(Path(receipt['log']),archive/'checkpoint-105-focused.log')
for name in ('CURRENT_STATUS.md','DEPENDENCY_FRONTIER.md','RESUME_CHECKPOINT.md','function-coverage-overrides.json'):
    shutil.copy2(rec/name,archive/(name+'.before105'))
overrides_path=rec/'function-coverage-overrides.json'
overrides=read(overrides_path)
target=overrides['target_sha256']
originals=[]
for address, method in [('081a7780','RegisterNPCVnum'),('081a5430','LoadTimerScript')]:
    original=read(work/(address+'-decompile.json'))
    assert original['function']['address']==address and not original['truncated']
    originals.append(original)
    assert not any(row['address']==address for row in overrides['reviews'])
    overrides['reviews'].append({
        'address':address,
        'source_refs':[{'path':'game/src/questmanager.cpp','anchor':'CQuestManager::'+method+'('}],
        'test_refs':[{'path':'reconstruction/tests/quest_npc_set_regression.cpp','anchor':'int main()'}],
        'review_scope':'Whole original directory registration or timer script loader with actual NPC::Set, native directories and Lua.',
        'reviewer':'Checkpoint 105 full assembly review and focused tests',
        'limitations':'Tests use ordinary valid paths. Original loop and callback-visible insertion order reviewed against assembly.'
    })
write(rec/'npc-set-callers-ghidra.json',{'target_sha256':target,'functions':originals})
write(overrides_path,overrides)
subprocess.run([sys.executable,str(rec/'tools/update_coverage.py'),'--inventory',str(rec/'target-function-inventory.json')],check=True)
ledger=read(rec/'function-coverage-ledger.json')
assert not ledger['warnings']
manifest={'checkpoint':105,'status':'passed_focused','validation_scope':'focused_only',
          'focused_runtime_commands':suites,'registered_runtime_commands':118,'abi':True,'source_hashes':applied,
          'receipt_sha256':sha(proof),'last_full_validation':status['last_full_validation'],'coverage':ledger['summary']}
write(archive/'RECORD_MANIFEST.json',manifest)
status.update(latest_validation_scope='focused_only',latest_validation_receipt=str(proof.resolve()),
              latest_focused_receipt=str(proof.resolve()),latest_focused_suites=suites,
              latest_focused_check_exit_code=0,checkpoint105_npc_set_callers=manifest)
write(rec/'validation-status.json',status)
summary=f'Checkpoint 105 restored RegisterNPCVnum and LoadTimerScript. Three focused runtime suites and ABI checks passed. There are 118 registered suites. Checkpoint 103 remains the latest full run with 117 suites. The ledger contains {ledger["evidence_available"]} functions with evidence and {ledger["summary"]["reviewed_implementation_test_mapping"]} bounded source/test mappings.'
for name in ('CURRENT_STATUS.md','DEPENDENCY_FRONTIER.md'):
    p=rec/name
    p.write_text('# Checkpoint 105\n\n'+summary+'\n\nHistorical checkpoint notes follow.\n\n'+p.read_text(encoding='utf-8-sig'),encoding='utf-8')
p=rec/'RESUME_CHECKPOINT.md'
text='# Metin2 reconstruction restart — checkpoint 105\n\n'+summary+'''

The two manager callers now use actual NPC::Set, directory APIs and the recovered Lua-backed state loader. Verification and hashes are in reconstruction/npc_set_callers_evidence. Set proof is in reconstruction/npc_set_evidence and the full checkpoint 103 proof is in reconstruction/npc_loader_evidence.

Next review InitializeLua and SetEventFlag dependency closures for the whole quest manager startup. The Initialize callsite inventory is in work/npc_set_callers/081a0760-calls.json. Full dispatch, VM startup, reward/stat/item chains and server linking remain in scope. Keep the original executable unmodified and unexecuted. Follow reconstruction/VALIDATION_POLICY.md for batching.
'''
p.write_text(text,encoding='utf-8')
(work.parents[1]/'outputs/METIN2_RESUME.md').write_text(text,encoding='utf-8')
p=root.parent/'README.md'
s=p.read_text(encoding='utf-8-sig')
s=re.sub(r'As of \*\*[^*]+\*\*, the latest recorded milestone is .*?:', 'As of **2026-11-09**, the latest recorded milestone is **checkpoint 105 (focused validation)**.',s,count=1)
s=re.sub(r'- \*\*116 regression programs.*?pass is claimed\.', '- **118 regression programs are registered.** Checkpoint 105 passed **3 focused runtime suites** and ABI checks. The latest full run passed **117 suites** at checkpoint 103.',s,count=1,flags=re.S)
s=re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{ledger["evidence_available"]} entries with structured address-backed evidence**',s)
s=re.sub(r'\*\*\d+ bounded',f'**{ledger["summary"]["reviewed_implementation_test_mapping"]} bounded',s)
p.write_text(s,encoding='utf-8')
print(summary)
