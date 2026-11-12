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
from elftools.dwarf.dwarf_expr import DWARFExprParser
import json
p=_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')
with open(p,'rb') as f:
 e=ELFFile(f);d=e.get_dwarf_info();expr=DWARFExprParser(d.structs);out={'structures':[],'functions':[],'variables':[],'symbols':[]}
 def val(x,k):
  if k not in x.attributes:return None
  v=x.attributes[k].value
  return v.decode('utf8','replace') if isinstance(v,bytes) else v
 def attrs(x):
  r={k[6:]:val(x,k) for k in x.attributes}
  for k in ('DW_AT_location','DW_AT_data_member_location','DW_AT_vtable_elem_location'):
   v=val(x,k)
   if isinstance(v,list):r[k[6:]+'_ops']=[{'op':o.op_name,'args':o.args} for o in expr.parse_expr(v)]
  return r
 def chain(x,seen=None):
  seen=set() if seen is None else seen
  if x.offset in seen:return []
  seen.add(x.offset);a=x.attributes
  return [{'die':hex(x.offset),'tag':x.tag,**{k[6:]:val(x,k) for k in ('DW_AT_name','DW_AT_byte_size') if k in a}}]+(chain(x.get_DIE_from_attribute('DW_AT_type'),seen) if 'DW_AT_type' in a else [])
 def record(x):return {'die':hex(x.offset),'tag':x.tag,'attrs':attrs(x),'type_chain':chain(x.get_DIE_from_attribute('DW_AT_type')) if 'DW_AT_type' in x.attributes else [],'children':[record(c) for c in x.iter_children()]}
 for cu in d.iter_CUs():
  top=cu.get_top_DIE()
  if val(top,'DW_AT_name')!='buffer_manager.cpp':continue
  out['cu']=attrs(top)
  for x in cu.iter_DIEs():
   n=val(x,'DW_AT_name')
   if x.tag in ('DW_TAG_class_type','DW_TAG_structure_type') and n in ('BUFFER_MANAGER','TEMP_BUFFER','singleton<BUFFER_MANAGER>','stack<buffer*, std::deque<buffer*, std::allocator<buffer*> > >'):out['structures'].append(record(x))
   elif x.tag=='DW_TAG_subprogram' and 'DW_AT_low_pc' in x.attributes:out['functions'].append(record(x))
   elif x.tag=='DW_TAG_variable':out['variables'].append(record(x))
 for s in e.get_section_by_name('.symtab').iter_symbols():
  if any(t in s.name for t in ('BUFFER_MANAGER','buffer_manager','TEMP_BUFFER')):out['symbols'].append({'name':s.name,'address':hex(s['st_value']),'size':s['st_size'],'type':s['st_info']['type'],'bind':s['st_info']['bind'],'section':s['st_shndx']})
 print(json.dumps(out,indent=2))
