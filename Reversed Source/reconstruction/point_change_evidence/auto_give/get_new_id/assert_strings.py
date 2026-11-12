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
from elftools.elf.elffile import ELFFile
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f)
 for a in (0x823126e,0x82313d7,0x82313ea):
  for s in e.iter_sections():
   if s['sh_addr']<=a<s['sh_addr']+s['sh_size']:print(hex(a),repr(s.data()[a-s['sh_addr']:].split(b'\0')[0]))
