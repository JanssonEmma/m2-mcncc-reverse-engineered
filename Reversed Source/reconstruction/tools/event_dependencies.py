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
import json
from elftools.elf.elffile import ELFFile
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f);d=e.get_dwarf_info();cu=next(d.iter_CUs())
 for die in cu.iter_DIEs():
  a=die.attributes;n=a.get('DW_AT_name');n=n.value.decode('utf8','replace') if n else None
  if (n in ('DWORD','TEVENTFUNC','heart','event_info','timeval') and die.tag in ('DW_TAG_typedef','DW_TAG_structure_type')) or die.offset in (0x37cd,0x2dc,0x1fc04):
   print(hex(die.offset),die.tag,{k:str(v.value) for k,v in a.items()})
   if 'DW_AT_type' in a:
    t=die.get_DIE_from_attribute('DW_AT_type'); print('TYPE',hex(t.offset),{k:str(v.value) for k,v in t.attributes.items()})
   for c in die.iter_children():
    if c.tag in ('DW_TAG_member','DW_TAG_formal_parameter'):
     print(' CHILD',c.tag,{k:str(v.value) for k,v in c.attributes.items()})
 for s in e.get_section_by_name('.symtab').iter_symbols():
  if any(x in s.name for x in ('AllocEventInfo','FindEventInfo','FreeEventInfo','event_create_ex','s_event_info_count')):print('SYMBOL',s.name,hex(s['st_value']))
