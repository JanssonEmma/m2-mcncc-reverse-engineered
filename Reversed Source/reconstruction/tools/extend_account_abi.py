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
from pathlib import Path
r=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction'))
x=json.loads((r/'desc-output-types.json').read_text())
p=r/'tests/desc_abi.cpp';text=p.read_text();marker='// Packed account/player member offsets from target DWARF.'
if marker not in text:
 rows=['',marker]
 for d in x['embedded_dependencies'].values():
  n=d['type']['name']
  if n not in ('SSimplePlayer','SAccountTable'):continue
  for m in d['members']:
   ops=m['location'];assert len(ops)==1 and ops[0]['op']=='DW_OP_plus_uconst'
   off=ops[0]['args'][0];field=m['name']
   rows.append(f'typedef char offset_{n}_{field}[(offsetof({n},{field})=={off})?1:-1];')
 p.write_text(text+'\n'.join(rows)+'\n')
print('packed offsets checked:',sum(len(d['members']) for d in x['embedded_dependencies'].values() if d['type']['name'] in ('SSimplePlayer','SAccountTable')))
