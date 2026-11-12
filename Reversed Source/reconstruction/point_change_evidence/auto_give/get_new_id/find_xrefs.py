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
import subprocess
from pathlib import Path
s=subprocess.check_output([r'C:\Strawberry\c\bin\objdump.exe','-d',_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')]).decode()
rows=s.splitlines();out=[]
for i,r in enumerate(rows):
 if '83231a4' in r or '83231a8' in r:out.append('\n'.join(rows[max(0,i-3):i+4]))
Path('work/point_change/auto_give/get_new_id/global-xrefs.txt').write_text('\n\n'.join(out));print('\n\n'.join(out))
