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
import json,sys,struct
p=Path(__file__).parent
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
exec(Path(_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\cleanup_types\\extract.py')).read_text().split("with open(r'F:")[0])
methods={'HorseSummon','SetRider','GetRider','GetMyHorseVnum','SendHorseInfo','ClearHorseInfo','SetRotation','Goto','SendMovePacket','SpawnMob','Show','GetMasterPID','EncodeMovePacket','SetEmpire','SetProto','StartAffectEvent','GetVictim','MonsterChat','GetEmpire','Get','GetEmpireFromMapIndex','GetMap','Find'}
with Path(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')).open('rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs);records=[];free=[]
 for cu in dw.iter_CUs():
  name=val(cu.get_top_DIE(),'DW_AT_name')
  if name not in ('char.cpp','char_horse.cpp','char_manager.cpp','guild.cpp','char_battle.cpp','char_affect.cpp','sectree_manager.cpp'):continue
  for d in cu.iter_DIEs():
   dn=val(d,'DW_AT_name')
   if d.tag in ('DW_TAG_structure_type','DW_TAG_class_type') and dn in ('CHARACTER','CHARACTER_MANAGER','CGuild','CEntity','CMobInstance','TPacketGCDead','packet_dead','TPacketGCMove','packet_move','char_event_info','SECTREE_MANAGER','SECTREE_MAP','TMapRegion','SSetting','SMapRegion','TAreaInfo','npc_info') and val(d,'DW_AT_byte_size'):
    row={'cu':name,'record':desc(d),'members':[],'methods':[]}
    for c in d.iter_children():
     if c.tag in ('DW_TAG_member','DW_TAG_inheritance'):
      a=c.attributes.get('DW_AT_data_member_location')
      row['members'].append({'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'access':val(c,'DW_AT_accessibility'),'location':[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value,list) else a.value if a else None})
     elif c.tag=='DW_TAG_subprogram' and (val(c,'DW_AT_name') in methods or dn in ('SECTREE_MANAGER','SECTREE_MAP','SSetting','SMapRegion','TAreaInfo','npc_info')):
      row['methods'].append({'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'linkage':val(c,'DW_AT_MIPS_linkage_name'),'access':val(c,'DW_AT_accessibility'),'virtuality':val(c,'DW_AT_virtuality'),'params':[{'name':val(a,'DW_AT_name'),'type':desc(typ(a))} for a in c.iter_children() if a.tag=='DW_TAG_formal_parameter']})
    records.append(row)
   if d.tag=='DW_TAG_subprogram' and dn in ('EncodeMovePacket','horse_dead_event'):
    free.append({'cu':name,'name':dn,'type':desc(typ(d)),'low_pc':val(d,'DW_AT_low_pc'),'linkage':val(d,'DW_AT_MIPS_linkage_name'),'params':[{'name':val(c,'DW_AT_name'),'type':desc(typ(c))} for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']})
 def read(addr,n):
  for sec in elf.iter_sections():
   start=sec['sh_addr'];end=start+sec['sh_size']
   if start<=addr and addr+n<=end:return sec.data()[addr-start:addr-start+n]
 def string(addr):
  b=read(addr,100).split(b'\0',1)[0]
  return {'address':hex(addr),'hex':b.hex(),'cpp':'"'+''.join('\\%03o'%x for x in b)+'"','cp949':b.decode('cp949','replace')}
 data={'suffix':string(0x822c7b1),'summon_error':string(0x822c7b9),'goto_victim':string(0x82267f2),'goto_no_victim':string(0x82267fb),'rotation_add':{'address':'081f9f70','hex':read(0x81f9f70,4).hex(),'float':struct.unpack('<f',read(0x81f9f70,4))[0]}}
 syms=[]
 for s in elf.get_section_by_name('.symtab').iter_symbols():
  if s['st_value'] in (0x816d940,0x816d8c0,0x816df10,0x816e000,0x816e060,0x812d390,0x816d890):syms.append({'name':s.name,'address':hex(s['st_value']),'size':s['st_size']})
 p.joinpath('full-records-api.json').write_text(json.dumps({'records':records,'free_functions':free},indent=2))
 p.joinpath('strings-floats.json').write_text(json.dumps(data,indent=2))
 p.joinpath('symbols.json').write_text(json.dumps(syms,indent=2))
 print(json.dumps(data,ensure_ascii=True))
 print('records',len(records),'free',len(free))
