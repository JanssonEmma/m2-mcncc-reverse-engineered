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
import pathlib,json,re,subprocess
exec(pathlib.Path('work/cleanup_types/extract.py').read_text().split('with open(')[0])
p=pathlib.Path('work/point_change');out=[]
names={'PointChange','LevelUp','LevelChange','AutoGiveItem','Save','Query','CharLog','ComputePoints','SetPolymorph','MountVnum'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f);dw=e.get_dwarf_info()
 def dump(d,depth=0):return {'node':desc(d),'attrs':{k:val(d,k) for k in d.attributes if k not in ('DW_AT_sibling','DW_AT_type')},'children':[dump(c,depth+1) for c in d.iter_children()] if depth<3 else []}
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name')!='char.cpp':continue
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_name') in names and d.tag=='DW_TAG_subprogram':out.append(dump(d))
   if val(d,'DW_AT_name') in ('packet_point_change','e_overtime') and d.tag in ('DW_TAG_structure_type','DW_TAG_enumeration_type'):out.append(dump(d))
 for s in e.get_section_by_name('.symtab').iter_symbols():
  if s.name=='_ZN9CHARACTER11PointChangeEhibb':
   a=s['st_value'];sz=s['st_size'];p.joinpath('assembly.txt').write_bytes(subprocess.check_output([r'C:\Strawberry\c\bin\objdump.exe','-d','--start-address='+hex(a),'--stop-address='+hex(a+sz),_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')]));p.joinpath('symbol.json').write_text(json.dumps({'name':s.name,'address':hex(a),'size':sz,'end':hex(a+sz)},indent=2))
p.joinpath('dwarf.json').write_text(json.dumps(out,indent=2));print('records',len(out))
for x in out:
 if x['attrs'].get('DW_AT_low_pc')==0x812e340:
  def walk(y):
   if y['node']['tag'] in ('DW_TAG_variable','DW_TAG_formal_parameter'):print(y['node']['name'],y['node'].get('type'))
   for c in y['children']:walk(c)
  walk(x)
