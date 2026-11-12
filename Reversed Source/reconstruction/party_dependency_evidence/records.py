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
import pathlib,json
exec(pathlib.Path('work/cleanup_types/extract.py').read_text().split('with open(')[0])
out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f);dw=e.get_dwarf_info()
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name')!='party.cpp':continue
  def dump(d):
   return {'node':desc(d),'attrs':{k:val(d,k) for k in d.attributes if k not in ('DW_AT_sibling','DW_AT_type')},'children':[{'node':desc(c),'attrs':{k:val(c,k) for k in c.attributes if k not in ('DW_AT_sibling','DW_AT_type')}} for c in d.iter_children()]}
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name') or ''
   if n in ('TPacketPartySetMemberLevel','TPacketGCPartyUpdate') and d.tag=='DW_TAG_typedef':out.append(dump(typ(d)))
   if n=='BuildUpdatePartyPacket' and d.tag=='DW_TAG_subprogram':out.append(dump(d))
   if d.tag=='DW_TAG_enumerator' and n.startswith(('PARTY_','PM_')):out.append(dump(d))
pathlib.Path('work/party_dependency/records-enums-dwarf.json').write_text(json.dumps(out,indent=2))
for x in out:
 print(x['node']['name'],x['node']['size'],[(c['node']['name'],c['attrs']) for c in x['children']])
