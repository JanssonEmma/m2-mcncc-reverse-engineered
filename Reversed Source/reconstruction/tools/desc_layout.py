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
def desc(d,seen=None):
 if d is None:return {'name':'void'}
 out={'die':hex(d.offset),'tag':d.tag,'name':value(d,'DW_AT_name'),'size':value(d,'DW_AT_byte_size')}
 t=typ(d)
 if t:out['underlying']=desc(t) if d.tag not in ('DW_TAG_structure_type','DW_TAG_class_type') else hex(t.offset)
 if d.tag=='DW_TAG_array_type':out['bounds']=[{k: value(c,k) for k in ('DW_AT_upper_bound','DW_AT_count')} for c in d.iter_children() if c.tag=='DW_TAG_subrange_type']
 return out
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info(); ex=DWARFExprParser(dw.structs)
 cu=next(c for c in dw.iter_CUs() if value(c.get_top_DIE(),'DW_AT_name')=='desc.cpp')
 target=next(d for d in cu.iter_DIEs() if d.tag in ('DW_TAG_class_type','DW_TAG_structure_type') and value(d,'DW_AT_name')=='DESC' and value(d,'DW_AT_byte_size'))
 nodes=[]
 for d in target.iter_children():
  x={'tag':d.tag,'name':value(d,'DW_AT_name'),'die':hex(d.offset),'type':desc(typ(d))}
  for k in ('DW_AT_virtuality','DW_AT_accessibility','DW_AT_external','DW_AT_artificial','DW_AT_declaration','DW_AT_MIPS_linkage_name'):
   if value(d,k) is not None:x[k]=value(d,k)
  for k in ('DW_AT_data_member_location','DW_AT_vtable_elem_location'):
   a=d.attributes.get(k)
   if a:x[k]=[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if isinstance(a.value,list) else a.value
  if d.tag=='DW_TAG_subprogram':x['parameters']=[{'name':value(p,'DW_AT_name'),'type':desc(typ(p))} for p in d.iter_children() if p.tag=='DW_TAG_formal_parameter']
  nodes.append(x)
 dependencies={}
 def collect(d):
  if d is None or hex(d.offset) in dependencies:return
  if d.tag in ('DW_TAG_typedef','DW_TAG_const_type','DW_TAG_volatile_type','DW_TAG_array_type'):
   collect(typ(d));return
  if d.tag not in ('DW_TAG_structure_type','DW_TAG_class_type'):return
  if value(d,'DW_AT_name') not in ('SAccountTable','TSimplePlayer','SSimplePlayer','sockaddr_in','in_addr'):return
  rec={'type':desc(d),'members':[]};dependencies[hex(d.offset)]=rec
  for c in d.iter_children():
   if c.tag!='DW_TAG_member':continue
   a=c.attributes.get('DW_AT_data_member_location')
   rec['members'].append({'name':value(c,'DW_AT_name'),'type':desc(typ(c)),'location':[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value,list) else a.value if a else None})
   collect(typ(c))
 for c in target.iter_children():
  if c.tag=='DW_TAG_member':collect(typ(c))
 out={'class':desc(target),'children':nodes,'embedded_dependencies':dependencies}
 Path(__file__).with_name('desc-layout.json').write_text(json.dumps(out,indent=2))
 for n in nodes:
  if n['tag'] in ('DW_TAG_member','DW_TAG_inheritance') or n.get('DW_AT_virtuality'):print(json.dumps(n))

