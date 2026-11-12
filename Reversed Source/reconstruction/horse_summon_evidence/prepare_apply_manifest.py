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
import json,hashlib
p=Path(__file__).parent
s=json.loads((p/'INTEGRATION_STATUS.json').read_text())
rows=[]
for rel in s['planned_files']:
 f=p/'integration-ready'/rel
 rows.append({'path':rel,'before_sha256':s['before'][rel],'after_sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
manifest={'root':_resolve_portable_path('PROJECT_ROOT\\Reversed Source'),'files':rows,'protected_global_fixtures':s['global_fixtures_unchanged'],'source':'closed horse-helper original-placement batch only','validation_pending':True}
(p/'apply-manifest.json').write_text(json.dumps(manifest,indent=2))
print('SHA-guard manifest staged:',len(rows),'files')
