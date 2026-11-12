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
import json,hashlib
p=_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')
def dec(v):return v.decode('utf8','replace') if isinstance(v,bytes) else v
with open(p,'rb') as f:
 e=ELFFile(f);d=e.get_dwarf_info();expr=DWARFExprParser(d.structs);out={'binary':p,'sha256':hashlib.sha256(open(p,'rb').read()).hexdigest(),'functions':[],'structures':[],'variables':[]}
 def chain(x,seen=None):
  seen=set() if seen is None else seen
  if x is None or x.offset in seen:return []
  seen.add(x.offset);a=x.attributes
  item={'die':hex(x.offset),'tag':x.tag,**{k[6:]:dec(a[k].value) for k in ['DW_AT_name','DW_AT_byte_size','DW_AT_encoding'] if k in a}}
  if x.tag=='DW_TAG_array_type':item['bounds']=[{k[6:]:dec(v.value) for k,v in c.attributes.items()} for c in x.iter_children() if c.tag=='DW_TAG_subrange_type']
  return [item]+(chain(x.get_DIE_from_attribute('DW_AT_type'),seen) if 'DW_AT_type' in a else [])
 def attrs(x):
  r={}
  for k,a in x.attributes.items():
   v=a.value
   r[k[6:]]=dec(v)
   if k in ('DW_AT_location','DW_AT_data_member_location') and isinstance(v,list):r[k[6:]+'_ops']=[{'op':o.op_name,'args':o.args} for o in expr.parse_expr(v)]
  return r
 for cu in d.iter_CUs():
  top=cu.get_top_DIE()
  if top.attributes.get('DW_AT_name') is None or top.attributes['DW_AT_name'].value!=b'buffer.c':continue
  out['cu']=attrs(top)
  for x in cu.iter_DIEs():
   if x.tag=='DW_TAG_subprogram':out['functions'].append({'attributes':attrs(x),'return_chain':chain(x.get_DIE_from_attribute('DW_AT_type')) if 'DW_AT_type' in x.attributes else [],'parameters':[{'attributes':attrs(c),'type_chain':chain(c.get_DIE_from_attribute('DW_AT_type')) if 'DW_AT_type' in c.attributes else []} for c in x.iter_children() if c.tag=='DW_TAG_formal_parameter']})
   if x.tag=='DW_TAG_structure_type' and x.attributes.get('DW_AT_name') and x.attributes['DW_AT_name'].value==b'buffer':out['structures'].append({'attributes':attrs(x),'members':[{'attributes':attrs(c),'type_chain':chain(c.get_DIE_from_attribute('DW_AT_type')) if 'DW_AT_type' in c.attributes else []} for c in x.iter_children()]})
   if x.tag=='DW_TAG_variable':out['variables'].append({'attributes':attrs(x),'type_chain':chain(x.get_DIE_from_attribute('DW_AT_type')) if 'DW_AT_type' in x.attributes else []})
 print(json.dumps(out,indent=2))
