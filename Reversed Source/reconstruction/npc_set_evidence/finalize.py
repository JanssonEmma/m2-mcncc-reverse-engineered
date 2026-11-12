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
import shutil
import subprocess
import sys

root = Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
rec = root / 'reconstruction'
work = Path(__file__).resolve().parent
workspace = work.parents[1]
proof = work.parent / 'checkpoint-104-focused.receipt.json'
archive = rec / 'npc_set_evidence'

def read(path):
    return json.loads(path.read_text(encoding='utf-8-sig'))

def write(path, value):
    path.write_text(json.dumps(value, indent=2) + '\n', encoding='utf-8')

def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

receipt = read(proof)
assert receipt['status'] == 'passed' and receipt['exit_code'] == 0
assert receipt['scope'] == 'focused' and receipt['abi_requested']
actual = ['quest_npc_set_regression', 'quest_npc_loader_regression', 'logging_regression',
          'arg_script_constructor_regression', 'desc_output_unit']
assert receipt['suites'] == receipt['executed_runtime_commands'] == actual
applied = read(work/'applied.json')
for row in applied:
    p = root/row['path']
    assert (digest(p) if p.exists() else None) == row['after']
status_path = rec/'validation-status.json'
status = read(status_path)
last_full = status['last_full_validation']
assert last_full['checkpoint'] == 103 and status['runtime_command_count_verified'] == 117
registry = next(line.split()[2:] for line in (rec/'tests/Makefile').read_text().splitlines() if line.startswith('RUNTIME_TESTS ='))
assert registry == ['quest_npc_set_regression'] + status['runtime_suites']
assert not archive.exists()
archive.mkdir()
for path in (proof, Path(receipt['log']), work/'applied.json', work/'HANDOFF.md', work/'finalize.py'):
    shutil.copy2(path, archive/path.name)
for name in ('RESUME_CHECKPOINT.md','CURRENT_STATUS.md','DEPENDENCY_FRONTIER.md','function-coverage-overrides.json'):
    shutil.copy2(rec/name, archive/(name+'.before104'))
overrides_path = rec/'function-coverage-overrides.json'
overrides = read(overrides_path)
target = overrides['target_sha256']
original = read(work/'081ae340-decompile.json')
assert original['function']['address'] == '081ae340' and not original['truncated']
write(rec/'npc-set-ghidra.json', {'target_sha256': target, 'functions': [original]})
assert not any(row['address']=='081ae340' for row in overrides['reviews'])
overrides['reviews'].append({
    'address':'081ae340',
    'source_refs':[{'path':'game/src/questnpc.cpp','anchor':'void NPC::Set('}],
    'test_refs':[{'path':'reconstruction/tests/quest_npc_set_regression.cpp','anchor':'int main()'}],
    'review_scope':'Whole directory traversal mapped to original instructions and calls. Native directories and real Lua loader exercised.',
    'reviewer':'Checkpoint 104 original ELF and focused test review',
    'limitations':'Unsafe original 1024-byte sprintf writes and filename format interpretation remain. Tests use defined filenames and avoid unspecified order among eligible entries in one directory.'
})
write(overrides_path, overrides)
subprocess.run([sys.executable,str(rec/'tools/update_coverage.py'),'--inventory',str(rec/'target-function-inventory.json')],check=True)
ledger = read(rec/'function-coverage-ledger.json')
assert not ledger['warnings']
manifest = {
    'checkpoint':104, 'status':'passed_focused', 'validation_scope':'focused_only', 'registered_runtime_commands':118,
    'focused_runtime_commands':actual, 'abi':True, 'source_hashes':applied, 'last_full_validation':last_full,
    'receipt_sha256':digest(proof), 'coverage':ledger['summary']
}
write(archive/'RECORD_MANIFEST.json',manifest)
status.update(date='2026-11-09', latest_validation_scope='focused_only', latest_validation_receipt=str(proof.resolve()),
              registered_runtime_suites=registry, registered_runtime_suite_count=118, latest_focused_suites=actual,
              latest_focused_check_exit_code=0, latest_focused_receipt=str(proof.resolve()), checkpoint104_npc_set=manifest)
write(status_path,status)
summary = f'Checkpoint 104 restored NPC::Set. Five focused suites and ABI checks passed. The current registry contains 118 suites. The last full run passed 117 suites at checkpoint 103. The ledger records {ledger["evidence_available"]} functions with evidence and {ledger["summary"]["reviewed_implementation_test_mapping"]} bounded source/test mappings.'
for name in ('CURRENT_STATUS.md','DEPENDENCY_FRONTIER.md'):
    p=rec/name
    p.write_text('# Checkpoint 104\n\n'+summary+'\n\nNext are the complete RegisterNPCVnum and LoadTimerScript callers. Historical checkpoint notes follow.\n\n'+p.read_text(encoding='utf-8-sig'),encoding='utf-8')
resume = f"""# Metin2 reconstruction restart — checkpoint 104

{summary}

The recovered NPC::Set at 081ae340 uses native directory traversal, both original formatting calls and the recovered state-script loader. The logger-only fake directory header now lives in its own test include path. Test coverage includes event/root iteration, dot and CVS-prefix filtering, missing directories and log order. Unsafe original path and format inputs are preserved in source and excluded from execution.

Proofs and source hashes are in reconstruction/npc_set_evidence. Complete original evidence is in reconstruction/npc_set_pending_evidence. Checkpoint 103 full proof and historical restart notes are in reconstruction/npc_loader_evidence.

Next integrate the whole RegisterNPCVnum at 081a7780 and LoadTimerScript at 081a5430. Their matching binary evidence and candidate bodies are in work/npc_set_callers. Check the exact signatures and guards. Test repeated registration across roots, registration retained after failed probes, timer ID allocation and no reload on a hit. Initialize still requires InitializeLua and SetEventFlag. Full VM, dispatch, reward/stat/item chains and server linking remain in scope. Follow reconstruction/VALIDATION_POLICY.md.
"""
(rec/'RESUME_CHECKPOINT.md').write_text(resume,encoding='utf-8')
(workspace/'outputs/METIN2_RESUME.md').write_text(resume,encoding='utf-8')
print(summary)
