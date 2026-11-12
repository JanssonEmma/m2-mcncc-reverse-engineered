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
import json,subprocess
from pathlib import Path
from elftools.elf.elffile import ELFFile
p=Path('work/point_change/auto_give/set_count');out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 for s in ELFFile(f).get_section_by_name('.symtab').iter_symbols():
  if s['st_size'] and any(n in s.name for n in ('SyncQuickslot','ChainQuickslot','RemoveFromCharacter','DestroyItem','SetCount','FindSpecifyItem')) and s['st_info']['type']=='STT_FUNC':
   a=s['st_value'];out.append({'name':s.name,'address':hex(a),'size':s['st_size']});p.joinpath(hex(a)+'-assembly.txt').write_bytes(subprocess.check_output([r'C:\Strawberry\c\bin\objdump.exe','-d','--start-address='+hex(a),'--stop-address='+hex(a+s['st_size']),_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')]))
p.joinpath('symbols.json').write_text(json.dumps(out,indent=2));print(json.dumps(out,indent=2))
