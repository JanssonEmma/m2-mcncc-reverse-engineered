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
  cn=val(cu.get_top_DIE(),'DW_AT_name')
  if cn not in ('log.cpp','questmanager.cpp','char.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_name') not in ('Query','LevelUp','CharLog','LogManager') or d.tag not in ('DW_TAG_subprogram','DW_TAG_structure_type'):continue
   def dump(d,dep=0):
    fi=val(d,'DW_AT_decl_file');fn=lp['file_entry'][fi-1].name.decode('utf8','replace') if fi else None
    return {'node':desc(d),'file':fn,'attrs':{k:val(d,k) for k in d.attributes if k not in ('DW_AT_sibling','DW_AT_type')},'children':[dump(c,dep+1) for c in d.iter_children()] if dep<2 else []}
   out.append({'cu':cn,**dump(d)})
pathlib.Path('work/point_change/logger-quest-dwarf.json').write_text(json.dumps(out,indent=2))
for x in out:
 if x['node']['tag']=='DW_TAG_subprogram': print(x['cu'],x['node']['name'],x['node'].get('type'),x['attrs'],[(c['node'].get('type'),c['attrs']) for c in x['children'] if c['node']['tag']=='DW_TAG_formal_parameter'])
