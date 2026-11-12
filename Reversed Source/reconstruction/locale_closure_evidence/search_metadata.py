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
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info()
 for cu in dw.iter_CUs():
  name=val(cu.get_top_DIE(),'DW_AT_name')
  if name not in ('constants.cpp','locale_service.cpp'):continue
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name') or ''
   if any(k in n for k in ('PercentBy','ChainLightning','check_name','is_twobyte','QuestDir','QuestObject','LocaleFilename')):
    out.append({'unit':name,'tag':d.tag,'name':n,'die':hex(d.offset),'type':desc(typ(d)),'attrs':str(d.attributes)})
Path('work/locale_closure/metadata-search.json').write_text(json.dumps(out,indent=2))
print([(r['unit'],r['tag'],r['name']) for r in out])
