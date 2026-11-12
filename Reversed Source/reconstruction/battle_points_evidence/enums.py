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
 dw=ELFFile(f).get_dwarf_info()
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name')!='char.cpp':continue
  for d in cu.iter_DIEs():
   if d.tag!='DW_TAG_enumeration_type':continue
   kids=list(d.iter_children());names={val(c,'DW_AT_name') for c in kids}
   if not names.intersection({'ITEM_ARMOR','ARMOR_BODY','WEAR_BODY','SKILL_POLYMORPH'}):continue
   out.append({'node':desc(d),'children':[{'name':val(c,'DW_AT_name'),'value':val(c,'DW_AT_const_value')} for c in kids]})
pathlib.Path('work/battle_points/enums-dwarf.json').write_text(json.dumps(out,indent=2));print([(x['node']['name'],len(x['children'])) for x in out])
