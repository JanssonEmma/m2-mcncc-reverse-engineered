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
import pathlib
exec(pathlib.Path('work/cleanup_types/extract.py').read_text().split('with open(')[0])
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();out=[]
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name') != 'questlua_global.cpp': continue
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name')
   if n not in ('GetCurrentPC','GetCurrentCharacterPtr'): continue
   out.append({'node':desc(d),'return':desc(typ(d)),'attrs':{k:val(d,k) for k in ('DW_AT_low_pc','DW_AT_high_pc','DW_AT_decl_file','DW_AT_decl_line','DW_AT_inline','DW_AT_MIPS_linkage_name')}})
 pathlib.Path('work/quest_lua_global_registration/inline-accessors-dwarf.json').write_text(json.dumps(out,indent=2))
 print(json.dumps(out))
