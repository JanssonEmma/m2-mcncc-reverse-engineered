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
import pathlib,subprocess,json
from elftools.elf.elffile import ELFFile
p=pathlib.Path('work/point_change/auto_give');names={'_ZN9CHARACTER12AutoGiveItemEmhib':'auto-give','_ZN12ITEM_MANAGER8GetTableEm':'get-table','_ZN12ITEM_MANAGER10RealNumberEm':'real-number','_ZN12ITEM_MANAGER10CreateItemEmmmbi':'create-item'};out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f)
 for s in e.get_section_by_name('.symtab').iter_symbols():
  if s.name in names:
   a=s['st_value'];sz=s['st_size'];out.append({'name':s.name,'address':hex(a),'size':sz})
   p.joinpath(names[s.name]+'-assembly.txt').write_bytes(subprocess.check_output([r'C:\Strawberry\c\bin\objdump.exe','-d','--start-address='+hex(a),'--stop-address='+hex(a+sz),_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')]))
p.joinpath('symbols.json').write_text(json.dumps(out,indent=2))
