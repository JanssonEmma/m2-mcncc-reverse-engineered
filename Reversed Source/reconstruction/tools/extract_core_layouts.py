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
records=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dwarf=ELFFile(f).get_dwarf_info(); expr=DWARFExprParser(dwarf.structs)
 for cu in dwarf.iter_CUs():
  src=cu.get_top_DIE().attributes['DW_AT_name'].value.decode()
  if src not in ('FSM.cpp','event_queue.c','event.c','vector.cpp','file_loader.cpp'):continue
  for d in cu.iter_DIEs():
   name=d.attributes.get('DW_AT_name'); name=name.value.decode('utf8','replace') if name else None
   if name not in ('CFSM','CState','CStateTemplate<CFSM>','SQueueElement','CEventQueue','event','SVector','CMemoryTextFileLoader') or d.tag not in ('DW_TAG_class_type','DW_TAG_structure_type') or 'DW_AT_byte_size' not in d.attributes:continue
   record={'unit':src,'die':hex(d.offset),'name':name,'byte_size':d.attributes['DW_AT_byte_size'].value,'members':[],'methods':[]}
   for c in d.iter_children():
    cn=c.attributes.get('DW_AT_name'); cn=cn.value.decode('utf8','replace') if cn else None
    if c.tag in ('DW_TAG_member','DW_TAG_inheritance'):
     location=c.attributes.get('DW_AT_data_member_location');raw=location.value if location else None
     ops=[{'op':o.op_name,'args':o.args} for o in expr.parse_expr(raw)] if isinstance(raw,list) else raw
     record['members'].append({'name':cn,'kind':c.tag,'type_die':hex(c.get_DIE_from_attribute('DW_AT_type').offset) if 'DW_AT_type' in c.attributes else None,'location':ops})
    elif c.tag=='DW_TAG_subprogram':
     record['methods'].append({'name':cn,'virtuality':c.attributes['DW_AT_virtuality'].value if 'DW_AT_virtuality' in c.attributes else None,'linkage_name':c.attributes['DW_AT_MIPS_linkage_name'].value.decode() if 'DW_AT_MIPS_linkage_name' in c.attributes else None})
   records.append(record)
print(json.dumps(records,indent=2))
