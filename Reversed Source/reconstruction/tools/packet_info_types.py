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
from elftools.dwarf.dwarf_expr import DWARFExprParser

def val(d,k):
 a=d.attributes.get(k)
 if a:return a.value.decode('utf8','replace') if isinstance(a.value,bytes) else a.value
 for r in ('DW_AT_specification','DW_AT_abstract_origin'):
  if r in d.attributes:
   v=val(d.get_DIE_from_attribute(r),k)
   if v is not None:return v

def typ(d):
 if 'DW_AT_type' in d.attributes:return d.get_DIE_from_attribute('DW_AT_type')
 for r in ('DW_AT_specification','DW_AT_abstract_origin'):
  if r in d.attributes:return typ(d.get_DIE_from_attribute(r))

def desc(d,depth=0):
 if d is None:return {'name':'void'}
 x={'die':hex(d.offset),'tag':d.tag,'name':val(d,'DW_AT_name'),'size':val(d,'DW_AT_byte_size')}
 if depth<5:
  t=typ(d)
  if t:x['type']=desc(t,depth+1)
 if d.tag=='DW_TAG_array_type':x['bounds']=[val(c,'DW_AT_upper_bound') for c in d.iter_children()]
 return x
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();ex=DWARFExprParser(dw.structs);out=[]
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name') not in ('packet_info.cpp',):continue
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name')
   if name not in ('CPacketInfo','~CPacketInfo','CPacketInfoCG','CPacketInfoGG','SPacketElement','TPacketElement','Set','Get','Start','End','Log','GetElement','IsSequence','IsSequencePacket','SetSequence'):continue
   
   x={'cu':val(cu.get_top_DIE(),'DW_AT_name'),'node':desc(d),'attrs':{k:val(d,k) for k in ('DW_AT_external','DW_AT_declaration','DW_AT_accessibility','DW_AT_low_pc','DW_AT_MIPS_linkage_name','DW_AT_virtuality','DW_AT_accessibility','DW_AT_inline','DW_AT_decl_file','DW_AT_decl_line')}}
   x['children']=[]
   for c in d.iter_children():
    if c.tag not in ('DW_TAG_member','DW_TAG_formal_parameter','DW_TAG_inheritance','DW_TAG_subprogram'):continue
    a=c.attributes.get('DW_AT_data_member_location');loc=None
    if a:loc=[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if isinstance(a.value,list) else a.value
    x['children'].append({'tag':c.tag,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':loc,'low_pc':val(c,'DW_AT_low_pc'),'linkage':val(c,'DW_AT_MIPS_linkage_name'),'access':val(c,'DW_AT_accessibility')})
   out.append(x)
 print(json.dumps(out,indent=2))





