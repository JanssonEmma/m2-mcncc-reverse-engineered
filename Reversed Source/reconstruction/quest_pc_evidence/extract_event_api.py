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
import sys,json
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
exec(Path(r'work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();cu=next(c for c in dw.iter_CUs() if val(c.get_top_DIE(),'DW_AT_name')=='questevent.cpp')
 out=[]
 for d in cu.iter_DIEs():
  if d.tag!='DW_TAG_subprogram' or val(d,'DW_AT_name') not in ('quest_create_timer_event','quest_create_server_timer_event','CancelTimerEvent'):continue
  out.append({'name':val(d,'DW_AT_name'),'pc':val(d,'DW_AT_low_pc'),'type':desc(typ(d)),'params':[{'name':val(q,'DW_AT_name'),'type':desc(typ(q))} for q in d.iter_children() if q.tag=='DW_TAG_formal_parameter']})
 p=Path(__file__).resolve().parent;(p/'event-api-dwarf.json').write_text(json.dumps(out,indent=2));print([(r['name'],r['pc'],[(q['name'],q['type'].get('name'),q['type'].get('tag')) for q in r['params']]) for r in out])
