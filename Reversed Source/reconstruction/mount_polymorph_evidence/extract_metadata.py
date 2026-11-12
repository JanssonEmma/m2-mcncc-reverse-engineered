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
import json,struct,sys
p=Path(__file__).parent
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
exec(Path(_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\cleanup_types\\extract.py')).read_text().split("with open(r'F:")[0])
methods={'SetPolymorph','MountVnum','ComputePoints','ComputeSkillPoints','RefreshAffect','ComputeAffect','ModifyPoints','StopRiding','GetHorseST','GetHorseDX','GetHorseHT','GetHorseIQ','HorseSummon','StartMuyeongEvent','StopMuyeongEvent','ComputeBattlePoints','PointChange','UnderAnyWar'}
with Path(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')).open('rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs);records=[]
 for cu in dw.iter_CUs():
  name=val(cu.get_top_DIE(),'DW_AT_name')
  if name not in ('char.cpp','char_affect.cpp','char_horse.cpp','char_skill.cpp','horse_rider.cpp','item.cpp','guild.cpp'):continue
  for d in cu.iter_DIEs():
   dn=val(d,'DW_AT_name')
   if d.tag in ('DW_TAG_structure_type','DW_TAG_class_type') and dn in ('CHARACTER','CEntity','CHorseRider','CItem','THorseStat','SJobInitialPoints','CAffect','TAffectFlag','CGuild') and val(d,'DW_AT_byte_size'):
    row={'cu':name,'record':desc(d),'members':[],'methods':[]}
    for c in d.iter_children():
     if c.tag in ('DW_TAG_member','DW_TAG_inheritance'):
      a=c.attributes.get('DW_AT_data_member_location')
      row['members'].append({'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'access':val(c,'DW_AT_accessibility'),'location':[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value,list) else a.value if a else None})
     elif c.tag=='DW_TAG_subprogram' and (val(c,'DW_AT_name') in methods or val(c,'DW_AT_virtuality')):
      a=c.attributes.get('DW_AT_vtable_elem_location')
      row['methods'].append({'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'linkage':val(c,'DW_AT_MIPS_linkage_name'),'access':val(c,'DW_AT_accessibility'),'virtuality':val(c,'DW_AT_virtuality'),'vtable_location':list(a.value) if a and isinstance(a.value,list) else a.value if a else None,'params':[{'name':val(a,'DW_AT_name'),'type':desc(typ(a))} for a in c.iter_children() if a.tag=='DW_TAG_formal_parameter']})
    records.append(row)
 syms=list(elf.get_section_by_name('.symtab').iter_symbols());byaddr={}
 for s in syms:
  if s['st_value'] and s.name:byaddr.setdefault(s['st_value'],[]).append(s.name)
 tables=[]
 for s in syms:
  if s.name.startswith('_ZTV') and any(x in s.name for x in ('CHARACTER','CHorseRider','CEntity')):
   sec=elf.get_section(s['st_shndx']);off=s['st_value']-sec['sh_addr'];words=[]
   for i,(v,) in enumerate(struct.iter_unpack('<I',sec.data()[off:off+s['st_size']])):
    words.append({'index':i,'address':hex(s['st_value']+i*4),'value':hex(v),'symbols':byaddr.get(v,[])})
   tables.append({'symbol':s.name,'address':hex(s['st_value']),'size':s['st_size'],'words':words})
 p.joinpath('full-records-api-virtuals.json').write_text(json.dumps(records,indent=2))
 p.joinpath('vtables.json').write_text(json.dumps(tables,indent=2))
 print(json.dumps([{'symbol':t['symbol'],'size':t['size']} for t in tables]))
