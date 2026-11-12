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
p=pathlib.Path('work/level_pk');out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f);dw=e.get_dwarf_info()
 for cu in dw.iter_CUs():
  cn=val(cu.get_top_DIE(),'DW_AT_name')
  if cn not in ('char.cpp','char_battle.cpp','config.cpp'):continue
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name') or '';good=n in ('SetLevel','SetPKMode','GetGMLevel','PK_PROTECT_LEVEL','m_bPKMode','gm_level','level')
   if d.tag=='DW_TAG_enumeration_type':good=any((val(c,'DW_AT_name') or '').startswith(('PK_MODE_','GM_')) for c in d.iter_children())
   if not good:continue
   out.append({'cu':cn,'node':desc(d),'attrs':{k:val(d,k) for k in d.attributes if k not in ('DW_AT_sibling','DW_AT_type')},'children':[{'node':desc(c),'attrs':{k:val(c,k) for k in c.attributes if k not in ('DW_AT_sibling','DW_AT_type')}} for c in d.iter_children()]})
 for s in e.iter_segments():
  a=s['p_vaddr']
  if a<=0x823b488<a+s['p_filesz']:(p/'pk-config-symbol.json').write_text(json.dumps({'name':'PK_PROTECT_LEVEL','address':'0x823b488','size':1,'section':'.data','initial_byte':s.data()[0x823b488-a]},indent=2));break
(p/'dwarf.json').write_text(json.dumps(out,indent=2))
for x in out:
 if x['node']['tag'] in ('DW_TAG_enumeration_type','DW_TAG_variable'):print(json.dumps(x))
