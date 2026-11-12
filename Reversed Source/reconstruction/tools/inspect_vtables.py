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
import struct,json
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f);out=[]
 for s in e.get_section_by_name('.symtab').iter_symbols():
  if s.name in ('_ZTV6CState','_ZTV4CFSM','_ZTV14CStateTemplateI4CFSME','__cxa_pure_virtual'):
   rec={'symbol':s.name,'address':hex(s['st_value']),'size':s['st_size'],'section':s['st_shndx']}
   if isinstance(s['st_shndx'],int):
    sec=e.get_section(s['st_shndx']);off=s['st_value']-sec['sh_addr'];raw=sec.data()[off:off+s['st_size']];rec['words']=[hex(x[0]) for x in struct.iter_unpack('<I',raw)]
   out.append(rec)
 print(json.dumps(out,indent=2))
