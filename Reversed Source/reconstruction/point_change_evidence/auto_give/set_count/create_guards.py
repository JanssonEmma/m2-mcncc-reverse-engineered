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
import hashlib,json
p=Path('work/point_change/auto_give/set_count');root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
files=['game/src/char.h','game/src/char_quickslot.cpp']
p.joinpath('apply-guards.json').write_text(json.dumps({'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','files':{n:hashlib.sha256(root.joinpath(n).read_bytes()).hexdigest() for n in files},'candidate_sha256':hashlib.sha256(p.joinpath('quickslot-chain.cpp.candidate').read_bytes()).hexdigest()},indent=2))
