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

root = Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
rec = root / 'reconstruction'
work = Path(__file__).resolve().parent
batch = work / 'quest_global_startup'
proof = work / (sys.argv[1] if len(sys.argv) > 1 else 'checkpoint-108-focused.receipt.json')
archive = rec / 'quest_startup_spatial_evidence'

def load(path):
    return json.loads(path.read_text(encoding='utf-8-sig'))

def save(path, value):
    path.write_text(json.dumps(value, indent=2) + '\n', encoding='utf-8')

def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

receipt = load(proof)
suites = ['quest_lua_global_registration_regression', 'quest_lua_queries_regression',
          'sectree_manager_regression', 'quest_state_lifecycle_regression', 'quest_npc_set_regression']
assert receipt['status'] == 'passed' and receipt['exit_code'] == 0 and receipt['abi_requested']
assert receipt['suites'] == receipt['executed_runtime_commands'] == suites
rows = load(batch / 'applied.json')
assert all(digest(root / row['path']) == row['after'] for row in rows)
status_path = rec / 'validation-status.json'
status = load(status_path)
assert status['checkpoint107_map_allow']['status'] == 'passed_focused'
assert status['last_full_validation']['checkpoint'] == 103
registry = next(s.split()[2:] for s in (rec/'tests/Makefile').read_text().splitlines() if s.startswith('RUNTIME_TESTS = '))
assert registry == [suites[0], suites[2]] + status['registered_runtime_suites']
overrides_path = rec / 'function-coverage-overrides.json'
overrides = load(overrides_path)
target = overrides['target_sha256']
direct = load(batch / 'original-ghidra.json')
assert direct['target_sha256'] == target
originals = direct['functions'] + [load(batch/'combine-decompile.json'), load(batch/'timerarg-decompile.json')]
registrar = work / 'quest_lua_global_registration'
spatial = work / 'sectree_manager'
originals += [load(registrar/(a+'-decompile.json')) for a in ('080c9090','080c9230','080c93c0','080c9570')]
sector_anchors = {
    '080b3730':'SECTREE_MAP::Find(DWORD package)', '080b3780':'SECTREE_MAP::Find(DWORD x,',
    '080b3960':'SECTREE_MANAGER::SECTREE_MANAGER(', '080b3a80':'SECTREE_MANAGER::SECTREE_MANAGER(',
    '080b3bb0':'SECTREE_MANAGER::~SECTREE_MANAGER(', '080b3dc0':'SECTREE_MANAGER::~SECTREE_MANAGER(',
    '080b3fd0':'SECTREE_MANAGER::~SECTREE_MANAGER(', '080b41f0':'SECTREE_MANAGER::GetMap(',
    '080b4240':'SECTREE_MANAGER::Get(DWORD index, DWORD package)',
    '080b4280':'SECTREE_MANAGER::Get(DWORD index, DWORD x,',
    '080b5a50':'SECTREE_MANAGER::GetMapBasePositionByMapIndex(',
}
originals += [load(spatial/(a+'-decompile.json')) for a in sector_anchors]
assert len(originals) == 26
for export in originals:
    address = export['function']['address']
    name = export['function']['name'].split('::')[-1]
    assert not export['truncated'] and not any(e['address'] == address for e in overrides['reviews'])
    anchor = name + '('
    if address in sector_anchors:
        source, anchor, test = 'sectree_manager.cpp', sector_anchors[address], suites[2]
        limits = 'Nonnull map/sector lifetime remains pending. Lifecycle and region scans are tested with the actual native manager.'
    elif address.startswith('080c'):
        source, test = 'questlua_global.cpp', suites[0]
        limits = 'Whole original registration and real Lua callbacks, with SQL and descriptor test boundaries.'
    else:
        source, test = ('questmanager.cpp' if address == '081a80e0' else 'questlua.cpp'), suites[1]
        limits = 'Direct startup callbacks and original Lua coercion. Full VM startup remains pending.'
    assert anchor in (root/'game/src'/source).read_text(), (address, anchor)
    row = dict(address=address, source_refs=[dict(path='game/src/'+source, anchor=anchor)],
               review_scope='Whole source body reviewed against the original ELF evidence.',
               reviewer='Checkpoint 108 binary and focused validation', limitations=limits)
    if address not in ('080b3730','080b3780'):
        row['test_refs'] = [dict(path='reconstruction/tests/'+test+'.cpp',
                                 anchor='static void TestDirectStartupCallbacks(' if test == suites[1] else 'int main(')]
    overrides['reviews'].append(row)

assert not archive.exists()
archive.mkdir()
for package in (batch, registrar, spatial):
    shutil.copytree(package, archive/package.name, ignore=shutil.ignore_patterns('*.exe','*.o','__pycache__'))
for p in [proof, Path(receipt['log']), Path(__file__)]:
    shutil.copy2(p, archive/p.name)
initial = work/'checkpoint-108-focused.receipt.json'
if initial != proof and initial.exists():
    shutil.copy2(initial, archive/initial.name)
    shutil.copy2(work/'checkpoint-108-focused.log', archive/'checkpoint-108-focused.log')
for name in ('CURRENT_STATUS.md','DEPENDENCY_FRONTIER.md','RESUME_CHECKPOINT.md','function-coverage-overrides.json'):
    shutil.copy2(rec/name, archive/(name+'.before108'))
save(rec/'quest-startup-spatial-ghidra.json', dict(target_sha256=target, functions=originals))
save(overrides_path, overrides)
subprocess.run([sys.executable, str(rec/'tools/update_coverage.py'), '--inventory', str(rec/'target-function-inventory.json')], check=True)
ledger = load(rec/'function-coverage-ledger.json')
assert not ledger['warnings']
manifest = dict(checkpoint=108, status='passed_focused', validation_scope='focused_only',
                registered_runtime_commands=len(registry), focused_runtime_commands=suites, abi=True,
                source_hashes=rows, receipt_sha256=digest(proof), last_full_validation=status['last_full_validation'])
save(archive/'RECORD_MANIFEST.json', manifest)
status.update(checkpoint108_startup_spatial=manifest, registered_runtime_suite_count=len(registry),
              registered_runtime_suites=registry, latest_validation_scope='focused_only',
              latest_validation_receipt=str(proof.resolve()), latest_focused_receipt=str(proof.resolve()),
              latest_focused_suites=suites, latest_focused_check_exit_code=0)
save(status_path, status)
summary = f'Checkpoint 108 restored the complete Lua logging registrar, direct quest startup callbacks, and sector-manager lifecycle and lookups. Five focused suites and ABI checks passed. The registry contains {len(registry)} suites, with the last full result at checkpoint 103. The ledger indexes {ledger["evidence_available"]} functions with evidence.'
for name in ('CURRENT_STATUS.md','DEPENDENCY_FRONTIER.md'):
    p=rec/name
    p.write_text('# Checkpoint 108\n\n'+summary+'\n\n'+p.read_text(encoding='utf-8-sig'),encoding='utf-8')
resume = f'''# Metin2 reconstruction restart — checkpoint 108

{summary}

Current user scope excludes all Monarch work. The prior investigation is historical. Continue the remaining startup callbacks and spatial dependencies without using the Monarch package. Evidence, test logs and current source guards are under reconstruction/quest_startup_spatial_evidence.

The sector manager owner and region base lookup are recovered. Whole owning map teardown still depends on SECTREE::Destroy and CHARACTER::Disconnect. Continue the region/spawn lookup closure, then the actual SpawnMob and ProcessEventFlag paths. Full VM, quest dispatch, reward/stat/item paths and server linking remain in scope.

Use focused validation for related batches and retain the latest full result separately. Keep the original ELF unexecuted and unmodified.
'''
(rec/'RESUME_CHECKPOINT.md').write_text(resume,encoding='utf-8')
(work.parent/'outputs/METIN2_RESUME.md').write_text(resume,encoding='utf-8')
p = root.parent/'README.md'
text = p.read_text(encoding='utf-8-sig')
text = re.sub(r'checkpoint 107 \(focused validation\)', 'checkpoint 108 (focused validation)', text)
text = re.sub(r'- \*\*118 regression programs[^\n]*', f'- **{len(registry)} regression programs are registered.** Checkpoint 108 passed **5 focused runtime suites** and ABI checks. The latest full run passed **117 suites** at checkpoint 103.', text)
text = re.sub(r'\*\*\d+ entries with structured address-backed evidence\*\*',f'**{ledger["evidence_available"]} entries with structured address-backed evidence**',text)
text = re.sub(r'\*\*\d+ bounded',f'**{ledger["summary"]["reviewed_implementation_test_mapping"]} bounded',text)
p.write_text(text,encoding='utf-8')
print(summary)
