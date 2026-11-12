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
import json,pathlib,struct
exec(pathlib.Path('work/cleanup_types/extract.py').read_text().split('with open(')[0])
p=pathlib.Path('work/battle_points');out=[]
names={'ComputeBattlePoints','GetPolymorphPoint','GetPolymorphPower','GetMobTable','GetSkillLevel','GetType','GetSubType','IsEquipped','IsPolymorphed','GetPolymorphVnum','IsPolyMaintainStat','GetSkillGroup','TPlayerSkill','SPlayerSkill'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f);dw=e.get_dwarf_info()
 for cu in dw.iter_CUs():
  cn=val(cu.get_top_DIE(),'DW_AT_name')
  if cn not in ('char.cpp','char_skill.cpp','item.cpp'):continue
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_name') not in names:continue
   if d.tag not in ('DW_TAG_subprogram','DW_TAG_structure_type','DW_TAG_typedef'):continue
   out.append({'cu':cn,'node':desc(d),'attrs':{k:val(d,k) for k in d.attributes if k not in ('DW_AT_sibling','DW_AT_type')},'children':[{'node':desc(c),'attrs':{k:val(c,k) for k in c.attributes if k not in ('DW_AT_sibling','DW_AT_type')}} for c in d.iter_children()]})
 constants={}
 for addr,size,name in [(0x821b540,164,'aiPolymorphPowerByLevel'),(0x8226cd8,4,'defense_divisor')]:
  for s in e.iter_segments():
   a=s['p_vaddr']
   if a<=addr and addr+size<=a+s['p_filesz']:
    b=s.data()[addr-a:addr-a+size];constants[name]={'address':hex(addr),'size':size,'hex':b.hex(),'values':list(struct.unpack('<41i',b)) if size==164 else struct.unpack('<f',b)[0]};break
(p/'dwarf.json').write_text(json.dumps(out,indent=2));(p/'constants-elf.json').write_text(json.dumps(constants,indent=2))
for x in out:
 if x['cu']=='char.cpp' and x['attrs'].get('DW_AT_declaration'):print(x['node'],[(c['node'].get('type'),c['attrs']) for c in x['children']])
print(constants)
