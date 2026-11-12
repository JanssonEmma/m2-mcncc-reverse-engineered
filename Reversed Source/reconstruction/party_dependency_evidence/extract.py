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
import json,pathlib
exec(pathlib.Path('work/cleanup_types/extract.py').read_text().split('with open(')[0])
from elftools.dwarf.dwarf_expr import DWARFExprParser
out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name')!='party.cpp':continue
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name') or ''
   if n not in ('CParty','SMember','TMember','EPartyRole','EPartyMessages','EPartyExpDistributionModes','TPacketPartySetMemberLevel','TPacketGCPartyUpdate','BuildUpdatePartyPacket','RequestSetMemberLevel','SendPartyInfoOneToAll'):continue
   if d.tag not in ('DW_TAG_class_type','DW_TAG_structure_type','DW_TAG_enumeration_type','DW_TAG_typedef','DW_TAG_subprogram'):continue
   x={'cu':'party.cpp','node':desc(d),'attrs':{k:val(d,k) for k in d.attributes if k not in ('DW_AT_sibling','DW_AT_type')}};x['children']=[]
   for c in d.iter_children():
    at={k:val(c,k) for k in c.attributes if k not in ('DW_AT_sibling','DW_AT_type')}
    x['children'].append({'node':desc(c),'attrs':at,'children':[{'node':desc(cc),'attrs':{k:val(cc,k) for k in cc.attributes if k not in ('DW_AT_sibling','DW_AT_type')}} for cc in c.iter_children()]})
   out.append(x)
pathlib.Path('work/party_dependency/dwarf.json').write_text(json.dumps(out,indent=2))
print([(x['node']['name'],x['node']['size'],x['node']['die']) for x in out])
