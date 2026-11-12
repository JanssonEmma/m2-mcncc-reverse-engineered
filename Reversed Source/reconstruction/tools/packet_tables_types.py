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
exec(open('work/cleanup_types/extract.py').read().split("with open(r'F:")[0])
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();ex=DWARFExprParser(dw.structs);out=[]
 for cu in dw.iter_CUs():
  cun=val(cu.get_top_DIE(),'DW_AT_name')
  if cun not in ('input.cpp','input_p2p.cpp','input_udp.cpp','packet_info.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  def fname(d):
   i=val(d,'DW_AT_decl_file')
   if i and lp:
    fe=lp.header.file_entry[i-1];return fe.name.decode('utf8','replace')
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name') or ''
   if name not in ('CPacketInfoCG','~CPacketInfoCG','CPacketInfoGG','~CPacketInfoGG','CPacketInfoUDP','~CPacketInfoUDP') and not (d.tag=='DW_TAG_structure_type' and ('Packet' in name or 'packet' in name) and val(d,'DW_AT_byte_size')):continue
   if d.tag=='DW_TAG_structure_type' and name not in ('CPacketInfoCG','CPacketInfoGG','CPacketInfoUDP'):
    out.append({'cu':cun,'node':desc(d),'file':fname(d),'line':val(d,'DW_AT_decl_line')});continue
   x={'cu':cun,'node':desc(d),'file':fname(d),'attrs':{k:val(d,k) for k in ('DW_AT_declaration','DW_AT_low_pc','DW_AT_inline','DW_AT_decl_line','DW_AT_virtuality','DW_AT_accessibility')}}
   x['children']=[]
   for c in d.iter_children():
    a=c.attributes.get('DW_AT_data_member_location');loc=None
    if a:loc=[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if isinstance(a.value,list) else a.value
    x['children'].append({'tag':c.tag,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':loc,'file':fname(c),'line':val(c,'DW_AT_decl_line'),'inline':val(c,'DW_AT_inline')})
   out.append(x)
 print(json.dumps(out,indent=2))
