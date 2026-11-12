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
import pathlib,json
p=pathlib.Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\stat-dependency-addressed-ghidra.json'));a=json.loads(p.read_text())
for r in a['functions']:
 f=r.get('function',{});n=f.get('name','')
 if n.endswith('PointChange'):
  pathlib.Path('work/point_change/saved-ghidra.json').write_text(json.dumps(r,indent=2));pathlib.Path('work/point_change/decompiled.txt').write_text(r['code']);print(len(r['code']),list(r))
