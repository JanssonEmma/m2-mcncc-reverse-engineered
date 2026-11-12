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
names={b'printdata',b'time_str',b'trim_and_lower',b'lower_string',b'str_dup',b'is_abbrev',b'filesize',b'parse_token'}
def typ(n):
 r={'die':hex(n.offset),'tag':n.tag}
 for k in ('DW_AT_name','DW_AT_byte_size'):
  if k in n.attributes:
   v=n.attributes[k].value;r[k]=v.decode() if isinstance(v,bytes) else v
 if 'DW_AT_type' in n.attributes:r['type']=typ(n.get_DIE_from_attribute('DW_AT_type'))
 return r
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 d=ELFFile(f).get_dwarf_info();out=[]
 for cu in d.iter_CUs():
  if cu.get_top_DIE().attributes['DW_AT_name'].value!=b'utils.c':continue
  for n in cu.iter_DIEs():
   if n.tag=='DW_TAG_subprogram' and n.attributes.get('DW_AT_name') and n.attributes['DW_AT_name'].value in names and 'DW_AT_low_pc' in n.attributes:
    out.append({'name':n.attributes['DW_AT_name'].value.decode(),'address':hex(n.attributes['DW_AT_low_pc'].value),'return':typ(n.get_DIE_from_attribute('DW_AT_type')) if 'DW_AT_type' in n.attributes else 'void','parameters':[{'name':p.attributes['DW_AT_name'].value.decode(),'type':typ(p.get_DIE_from_attribute('DW_AT_type'))} for p in n.iter_children() if p.tag=='DW_TAG_formal_parameter']})
 print(json.dumps(out,indent=2))
