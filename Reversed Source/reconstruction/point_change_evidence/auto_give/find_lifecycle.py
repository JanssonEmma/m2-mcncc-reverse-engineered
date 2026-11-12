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
 for s in ELFFile(f).get_section_by_name('.symtab').iter_symbols():
  if s['st_size'] and ('9DBManagerC' in s.name or '9DBManagerD' in s.name or '12ITEM_MANAGER8GetNewID' in s.name): print(s.name,hex(s['st_value']),s['st_size'])
