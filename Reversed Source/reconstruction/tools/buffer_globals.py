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
import json
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f);out=[]
 for s in e.get_section_by_name('.symtab').iter_symbols():
  if 'buffer_' in s.name and ('poll' in s.name or 'pool' in s.name):out.append({'name':s.name,'address':hex(s['st_value']),'size':s['st_size'],'section':s['st_shndx'],'bind':s['st_info']['bind']})
 print(json.dumps(out,indent=2))
