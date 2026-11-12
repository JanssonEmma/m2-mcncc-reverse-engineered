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
p=Path(__file__).parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
f=root/'game/src/char_horse.cpp';s=f.read_text()
fixed=s.replace('m_Horse.bLevel','GetHorseLevel()').replace('m_Horse.sHealth','GetHorseHealth()').replace('m_Horse.sStamina','GetHorseStamina()').replace('m_Horse.bRiding','IsHorseRiding()')
assert 'm_Horse' not in fixed and fixed!=s
dest=p/'char_horse-private-access-fixed.cpp';dest.write_text(fixed)
m={'target':str(f),'before_sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'payload':str(dest),'after_sha256':hashlib.sha256(dest.read_bytes()).hexdigest(),'reason':'Use existing real inline accessors, no base visibility/data change'}
(p/'horse-access-fix-manifest.json').write_text(json.dumps(m,indent=2))
print(json.dumps(m))
