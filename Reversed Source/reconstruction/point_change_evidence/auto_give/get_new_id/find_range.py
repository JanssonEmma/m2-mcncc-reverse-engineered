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
exec(Path('work/cleanup_types/extract.py').read_text().split('with open(')[0])
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();out=[]
 for cu in dw.iter_CUs():
  for d in cu.iter_DIEs():
   if d.tag not in ('DW_TAG_variable','DW_TAG_constant'):continue
   n=val(d,'DW_AT_name');a=d.attributes.get('DW_AT_location')
   if n=='g_dwItemIDRange' or a and a.value==[3,164,49,50,8]:
    out.append({'cu':val(cu.get_top_DIE(),'DW_AT_name'),'name':n,'type':desc(typ(d)),'attrs':{k:val(d,k) for k in ('DW_AT_location','DW_AT_external','DW_AT_decl_line','DW_AT_decl_file')}})
 Path('work/point_change/auto_give/get_new_id/range-dwarf.json').write_text(json.dumps(out,indent=2),encoding='utf8');print(json.dumps(out,indent=2))
