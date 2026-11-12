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
 d=ELFFile(f).get_dwarf_info();out=[]
 def typ(node,depth=0):
  if depth>5:return {'die':hex(node.offset)}
  r={'die':hex(node.offset),'tag':node.tag}
  for k in ('DW_AT_name','DW_AT_byte_size'):
   a=node.attributes.get(k)
   if a:r[k]=a.value.decode('utf8','replace') if isinstance(a.value,bytes) else a.value
  if 'DW_AT_type' in node.attributes:r['type']=typ(node.get_DIE_from_attribute('DW_AT_type'),depth+1)
  return r
 for cu in d.iter_CUs():
  top=cu.get_top_DIE();s=top.attributes['DW_AT_name'].value
  if s not in (b'heart.c',b'utils.c'):continue
  for die in cu.iter_DIEs():
   a=die.attributes;n=a.get('DW_AT_name')
   if die.tag=='DW_TAG_subprogram' and n and n.value in (b'timediff',b'timeadd',b'thecore_sleep',b'heart_new',b'heart_idle',b'heart_delete') and 'DW_AT_low_pc' in a:
    out.append({'unit':s.decode(),'function':n.value.decode(),'address':hex(a['DW_AT_low_pc'].value),'parameters':[dict(name=c.attributes['DW_AT_name'].value.decode() if 'DW_AT_name' in c.attributes else None,type=typ(c.get_DIE_from_attribute('DW_AT_type'))) for c in die.iter_children() if c.tag=='DW_TAG_formal_parameter']})
 print(json.dumps(out,indent=2))
