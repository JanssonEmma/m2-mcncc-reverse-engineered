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
from elftools.elf.elffile import ELFFile
exec(Path('work/cleanup_types/extract.py').read_text(encoding='utf-8-sig').split("with open(r'F:")[0])
import json
out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info()
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name') not in ('questpc.cpp','questmanager.cpp'):continue
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name')
   if n and n.startswith('QUEST_SEND_'):out.append({'unit':val(cu.get_top_DIE(),'DW_AT_name'),'tag':d.tag,'die':hex(d.offset),'name':n,'value':val(d,'DW_AT_const_value'),'type':desc(typ(d))})
Path('work/pc_quest_info/send-mask-metadata.json').write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8');print([(r['tag'],r['name'],r['value']) for r in out[:10]])
