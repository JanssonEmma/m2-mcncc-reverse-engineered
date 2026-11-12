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
 e=ELFFile(f);d=e.get_dwarf_info();cu=next(d.iter_CUs());out=[]
 def show(die,depth=0):
  if depth>4:return {'die':hex(die.offset),'depth_limit':True}
  attrs={k:(v.value.decode('utf8','replace') if isinstance(v.value,bytes) else v.value) for k,v in die.attributes.items() if k in ('DW_AT_name','DW_AT_byte_size','DW_AT_upper_bound','DW_AT_lower_bound','DW_AT_external','DW_AT_location','DW_AT_type')}
  r={'die':hex(die.offset),'tag':die.tag,'attributes':attrs}
  if 'DW_AT_type' in die.attributes:r['type']=show(die.get_DIE_from_attribute('DW_AT_type'),depth+1)
  if die.tag in ('DW_TAG_subroutine_type','DW_TAG_array_type'):r['children']=[show(c,depth+1) for c in die.iter_children()]
  return r
 for die in cu.iter_DIEs():
  n=die.attributes.get('DW_AT_name');name=n.value.decode('utf8','replace') if n else ''
  if name in ('s_pool_event','s_event_info_count','g_event_info','cxx_q','TEVENTFUNC','LPEVENTINFO') and die.tag in ('DW_TAG_variable','DW_TAG_typedef'):out.append(show(die))
  if name=='event' and die.tag=='DW_TAG_structure_type':
   for c in die.iter_children():
    if c.attributes.get('DW_AT_name') and c.attributes['DW_AT_name'].value==b'file':out.append(show(c))
 print(json.dumps(out,indent=2))
