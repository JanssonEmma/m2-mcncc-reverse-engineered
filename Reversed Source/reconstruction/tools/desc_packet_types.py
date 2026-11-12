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
from pathlib import Path
from elftools.elf.elffile import ELFFile
from elftools.dwarf.dwarf_expr import DWARFExprParser

def val(d,k):
 a=d.attributes.get(k)
 if a:return a.value.decode('utf8','replace') if isinstance(a.value,bytes) else a.value

def typ(d):
 return d.get_DIE_from_attribute('DW_AT_type') if 'DW_AT_type' in d.attributes else None

def describe(d):
 if not d:return None
 x={'die':hex(d.offset),'tag':d.tag,'name':val(d,'DW_AT_name'),'size':val(d,'DW_AT_byte_size')}
 t=typ(d)
 if t and d.tag not in ('DW_TAG_structure_type','DW_TAG_class_type'):x['type']=describe(t)
 if d.tag=='DW_TAG_array_type':x['upper_bounds']=[val(c,'DW_AT_upper_bound') for c in d.iter_children() if c.tag=='DW_TAG_subrange_type']
 return x
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();xp=DWARFExprParser(dw.structs)
 cu=next(c for c in dw.iter_CUs() if val(c.get_top_DIE(),'DW_AT_name')=='desc.cpp')
 out=[]
 for d in cu.iter_DIEs():
  if val(d,'DW_AT_name') not in ('SPacketGGRelay','TPacketGGRelay','__packet_encode','get_global_time'):continue
  x={'node':describe(d),'children':[]}
  for c in d.iter_children():
   if c.tag not in ('DW_TAG_member','DW_TAG_formal_parameter'):continue
   loc=c.attributes.get('DW_AT_data_member_location')
   x['children'].append({'name':val(c,'DW_AT_name'),'type':describe(typ(c)),'offset_expr':[{'op':o.op_name,'args':o.args} for o in xp.parse_expr(loc.value)] if loc else None})
  out.append(x)
 Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\desc-packet-types.json')).write_text(json.dumps(out,indent=2))
 print(json.dumps(out,indent=2))
