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
import hashlib,json
from pathlib import Path
from elftools.elf.elffile import ELFFile
from elftools.dwarf.dwarf_expr import DWARFExprParser
binary=Path(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'))
def val(d,k,seen=None):
 seen=set() if seen is None else seen
 if d.offset in seen:return None
 seen.add(d.offset)
 a=d.attributes.get(k)
 if a:return a.value.decode('utf8','replace') if isinstance(a.value,bytes) else a.value
 for ref in ('DW_AT_specification','DW_AT_abstract_origin'):
  if ref in d.attributes:
   v=val(d.get_DIE_from_attribute(ref),k,seen)
   if v is not None:return v
 return None
def tref(d):
 if 'DW_AT_type' in d.attributes:return hex(d.get_DIE_from_attribute('DW_AT_type').offset)
 for ref in ('DW_AT_specification','DW_AT_abstract_origin'):
  if ref in d.attributes:return tref(d.get_DIE_from_attribute(ref))
 return None
def dec(v):return v.decode('utf8','replace') if isinstance(v,bytes) else v
units=[]; layouts=[];types={}
wanted={'CFSM','CState','CStateTemplate<CFSM>','TQueueElement','event','event_info_data','CEventQueue','EVENT','event_queue','quickslot','TQuickslot','CEntity','PIXEL_POSITION','SPosition'}
with binary.open('rb') as stream:
 dwarf=ELFFile(stream).get_dwarf_info();expr=DWARFExprParser(dwarf.structs)
 for cu in dwarf.iter_CUs():
  top=cu.get_top_DIE();source=val(top,'DW_AT_name');line=dwarf.line_program_for_CU(cu);files=[]
  if line:
   dirs=[dec(d) for d in line.header['include_directory']]
   for f in line.header['file_entry']:
    prefix=dirs[f.dir_index-1] if f.dir_index else val(top,'DW_AT_comp_dir')
    file_name=dec(f.name)
    files.append(file_name if file_name.startswith('/') else (prefix.rstrip('/')+'/' if prefix else '')+file_name)
  functions=[]
  focused=source in ('FSM.cpp','event.c','event_queue.c','char_quickslot.cpp','entity_view.cpp')
  for d in cu.iter_DIEs():
   if d.tag=='DW_TAG_subprogram' and val(d,'DW_AT_low_pc') is not None:
    index=val(d,'DW_AT_decl_file');high=d.attributes.get('DW_AT_high_pc')
    functions.append({'die':hex(d.offset),'name':val(d,'DW_AT_name'),'linkage_name':val(d,'DW_AT_MIPS_linkage_name') or val(d,'DW_AT_linkage_name'),'low_pc':hex(val(d,'DW_AT_low_pc')),'high_pc_raw':val(d,'DW_AT_high_pc'),'high_pc_form':high.form if high else None,'decl_file':files[index-1] if isinstance(index,int) and 0<index<=len(files) else None,'decl_line':val(d,'DW_AT_decl_line'),'return_type_die':tref(d),'parameters':[{'name':val(p,'DW_AT_name'),'type_die':tref(p)} for p in d.iter_children() if p.tag=='DW_TAG_formal_parameter']})
   if d.tag in ('DW_TAG_class_type','DW_TAG_structure_type','DW_TAG_union_type') and val(d,'DW_AT_name') in wanted and 'DW_AT_byte_size' in d.attributes:
    members=[]
    for c in d.iter_children():
     if c.tag not in ('DW_TAG_member','DW_TAG_inheritance'):continue
     a=c.attributes.get('DW_AT_data_member_location');raw=a.value if a else None
     ops=[{'op':o.op_name,'args':o.args} for o in expr.parse_expr(raw)] if isinstance(raw,list) else None
     members.append({'kind':c.tag,'name':val(c,'DW_AT_name'),'type_die':tref(c),'location_raw':raw,'location_ops':ops})
    layouts.append({'die':hex(d.offset),'compilation_unit':source,'name':val(d,'DW_AT_name'),'byte_size':val(d,'DW_AT_byte_size'),'members':members})
   if focused and ((d.tag or '').endswith('_type') or d.tag=='DW_TAG_typedef'):
    types[hex(d.offset)]={'tag':d.tag,'name':val(d,'DW_AT_name'),'byte_size':val(d,'DW_AT_byte_size'),'type_die':tref(d)}
  units.append({'source':source,'die':hex(top.offset),'files':files,'functions':functions})
print(json.dumps({'binary':str(binary),'sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'scope':'All compilation units and addressed subprograms; selected layouts; type graph for five focused units.','limitations':['No-low_pc declarations and inline-only records excluded. Addressed records may include compiler variants.','high_pc is raw with form recorded. DIE identifiers are binary-local.','Layout records retain duplicate observations across compilation units. File indices use legacy DWARF one-based indexing.'],'units':units,'layouts':layouts,'type_nodes':types},indent=2))

