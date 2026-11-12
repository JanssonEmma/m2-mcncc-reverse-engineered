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
import json,hashlib,shutil,argparse
p=Path(__file__).resolve().parent;ap=argparse.ArgumentParser();ap.add_argument('--root',default=_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));a=ap.parse_args();root=Path(a.root);m=json.loads((p/'manifest.json').read_text(encoding='utf-8'));h=lambda q:hashlib.sha256(q.read_bytes()).hexdigest()
for rel,d in m['files'].items():
 q=root/rel;assert (h(q) if q.exists() else None)==d['before_sha256'],('changed baseline',rel);assert h(p/'integration-ready'/rel)==d['after_sha256'],('changed staged',rel)
for rel in m['files']:
 q=root/rel
 if q.exists():b=p/'parent-apply-backup'/rel;b.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(q,b)
for rel in m['files']:
 q=root/rel;q.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p/'integration-ready'/rel,q)
print('Applied6 preflighted additive tail files; no tests run by this script')
