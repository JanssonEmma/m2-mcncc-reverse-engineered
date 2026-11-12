"""Build/resume a source-organized Ghidra export index, reusing saved evidence.

This does not call Ghidra or run the server. The chat uses its attached Ghidra
tools for missing results. Output is a compact queue of remaining addresses.
"""
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
import json
from pathlib import Path, PurePosixPath

ROOT = Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction'))
DESTINATION = ROOT / 'decompiled'

def read(path):
    with path.open(encoding='utf-8-sig') as stream:
        return json.load(stream)

inventory = read(ROOT / 'dwarf-inventory.json')
placements = {p['original_source']: p['proposed_path'] for p in read(ROOT / 'file-placement.json')}
identity = inventory['sha256']
cache = {}
for path in ROOT.glob('*ghidra.json'):
    saved = read(path)
    if isinstance(saved, dict) and isinstance(saved.get('functions'), list):
        if saved.get('target_sha256', saved.get('targetSha256')) not in (None, identity):
            raise ValueError('Batch export has a different target identity: ' + str(path))
        # Legacy wrapped RPC batches require their own decoder; do not infer identity.
        records = saved['functions'] if saved.get('target_sha256', saved.get('targetSha256')) == identity else []
    else:
        records = [saved] if isinstance(saved, dict) and 'function' in saved else saved.values() if isinstance(saved, dict) else saved if isinstance(saved, list) else []
    for record in records:
        if isinstance(record, dict) and isinstance(record.get('code'), str) and record.get('function', {}).get('address'):
            cache[int(record['function']['address'], 16)] = record

index = {}
for unit in inventory['units']:
    module = placements[unit['source']]
    relative = PurePosixPath(module)
    if relative.is_absolute() or '..' in relative.parts or ':' in module:
        raise ValueError('Unsafe module path: ' + module)
    for function in unit['functions']:
        address = int(function['low_pc'], 16)
        if not 0x0804CB00 <= address <= 0x081F5C63:
            continue
        key = f'{address:08x}'
        row = index.setdefault(key, {'requested_address': '0x' + key,
            'path': str(relative / (key + '.json')), 'compilation_units': [], 'declarations': []})
        if unit['source'] not in row['compilation_units']:
            row['compilation_units'].append(unit['source'])
        row['declarations'].append({k: function.get(k) for k in ('die', 'name', 'linkage_name', 'decl_file', 'decl_line')})

DESTINATION.mkdir(exist_ok=True)
pending = {}
cached = existing = partial = failed = 0
for key, row in index.items():
    destination = DESTINATION / row['path']
    if not destination.resolve().is_relative_to(DESTINATION.resolve()):
        raise ValueError('Output escaped selected destination')
    record = None
    if destination.exists():
        saved = read(destination)
        if saved.get('target_sha256') != identity:
            raise ValueError('Existing export has a different target identity: ' + str(destination))
        record = saved.get('result')
        if saved.get('error'):
            failed += 1
            continue
        if not isinstance(record, dict) or not isinstance(record.get('code'), str):
            raise ValueError('Malformed existing export: ' + str(destination))
        existing += 1
    elif int(key, 16) in cache:
        record = cache[int(key, 16)]
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(json.dumps({'target_sha256': identity,
            'requested_address': row['requested_address'], 'origin': 'previously retained Ghidra result',
            'result': record}, indent=2), encoding='utf-8')
        cached += 1
    else:
        module = str(PurePosixPath(row['path']).parent)
        pending.setdefault(module, []).append(row['requested_address'])
    if record and record.get('truncated'):
        partial += 1

(DESTINATION / 'index.json').write_text(json.dumps({'target_sha256': identity,
    'scope': 'Addressed DWARF subprogram records within the original ELF text range; aliases and compiler-generated functions retained.',
    'limitations': ['Pseudocode is evidence, not compilable recovered source.',
        'Primary module placement is provisional; additional compilation-unit observations are retained.',
        'The requested address may lie inside the canonical function returned by Ghidra.',
        'This inventory has different coverage from Ghidra function discovery.'],
    'functions': index}, indent=2), encoding='utf-8')
summary = {'total': len(index), 'cached': cached, 'existing': existing,
    'partial': partial, 'failed': failed, 'remaining': sum(map(len, pending.values()))}
(ROOT / 'decompilation-status.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
print(json.dumps({'target_sha256': identity, 'summary': summary,
    'pending_groups': [{'module': module, 'addresses': addresses} for module, addresses in pending.items()]}))
