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
import pathlib,json,subprocess
from elftools.elf.elffile import ELFFile
p=pathlib.Path('work/battle_points');out=[]
names={'_ZN9CHARACTER19ComputeBattlePointsEv':'compute','_ZN9CHARACTER17GetPolymorphPointEh':'polymorph','_ZN9CHARACTER17GetPolymorphPowerEv':'power','_ZN9CHARACTER13GetSkillLevelEm':'skill-level','_ZN9CHARACTER11GetMobTableEv':'mob-table'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f)
 for s in e.get_section_by_name('.symtab').iter_symbols():
  if s.name in names:
   a=s['st_value'];sz=s['st_size'];name=names[s.name]
   out.append({'name':s.name,'address':hex(a),'size':sz,'end':hex(a+sz)})
   b=subprocess.check_output([r'C:\Strawberry\c\bin\objdump.exe','-d','--start-address='+hex(a),'--stop-address='+hex(a+sz),_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')])
   (p/(name+'-assembly.txt')).write_bytes(b)
(p/'method-symbols.json').write_text(json.dumps(out,indent=2));print(out)
