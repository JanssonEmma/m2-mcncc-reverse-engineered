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
def value(d,k):
 a=d.attributes.get(k)
 if a:return a.value.decode('utf8','replace') if isinstance(a.value,bytes) else a.value
 for r in ('DW_AT_specification','DW_AT_abstract_origin'):
  if r in d.attributes:
   v=value(d.get_DIE_from_attribute(r),k)
   if v is not None:return v
def typ(d):
 if 'DW_AT_type' in d.attributes:return d.get_DIE_from_attribute('DW_AT_type')
 for r in ('DW_AT_specification','DW_AT_abstract_origin'):
  if r in d.attributes:return typ(d.get_DIE_from_attribute(r))
def describe(d):
 if not d:return None
 o={'die':hex(d.offset),'tag':d.tag,'name':value(d,'DW_AT_name'),'size':value(d,'DW_AT_byte_size')}
 if d.tag in ('DW_TAG_typedef','DW_TAG_const_type','DW_TAG_pointer_type'):o['underlying']=describe(typ(d))
 return o
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info(); ex=DWARFExprParser(dw.structs); out={}
 for cu in dw.iter_CUs():
  name=value(cu.get_top_DIE(),'DW_AT_name')
  if name not in ('TrafficProfiler.cpp','desc.cpp','config.cpp','main.cpp'):continue
  rows=[]
  for d in cu.iter_DIEs():
   if value(d,'DW_AT_name')=='g_bTrafficProfileOn':rows.append({'global':describe(d),'type':describe(typ(d))})
   if d.tag in ('DW_TAG_class_type','DW_TAG_structure_type') and value(d,'DW_AT_name')=='TrafficProfiler' and value(d,'DW_AT_byte_size'):
    cls={'class':describe(d),'children':[]}
    for c in d.iter_children():
     if c.tag not in ('DW_TAG_member','DW_TAG_inheritance'):continue
     a=c.attributes.get('DW_AT_data_member_location')
     cls['children'].append({'name':value(c,'DW_AT_name'),'tag':c.tag,'type':describe(typ(c)), 'location':[{'op':x.op_name,'args':x.args} for x in ex.parse_expr(a.value)] if a else None})
    rows.append(cls)
  out[name]=rows
 (Path(__file__).resolve().parent.parent / 'traffic-profiler-types.json').write_text(json.dumps(out,indent=2))
 print(json.dumps(out,indent=2))


