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
from pathlib import Path
import json,sys
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
exec(Path(_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\cleanup_types\\extract.py')).read_text().split("with open(r'F:")[0])
out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info()
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name') not in ('guild.cpp','guild_war.cpp'):continue
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_name')=='SGuildWar' and d.tag in ('DW_TAG_subprogram','DW_TAG_inlined_subroutine'):
    out.append({'cu':val(cu.get_top_DIE(),'DW_AT_name'),'tag':d.tag,'die':hex(d.offset),'attrs':{k:val(d,k) for k in ('DW_AT_name','DW_AT_inline','DW_AT_low_pc','DW_AT_high_pc','DW_AT_ranges','DW_AT_abstract_origin','DW_AT_MIPS_linkage_name','DW_AT_decl_file','DW_AT_decl_line')},'children':[{'tag':c.tag,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'attrs':{k:str(a.value) for k,a in c.attributes.items()}} for c in d.iter_children()]})
Path(__file__).with_name('sguildwar-constructor-dwarf.json').write_text(json.dumps(out,indent=2))
print(json.dumps([{k:v for k,v in r.items() if k!='children'} for r in out],indent=2))
