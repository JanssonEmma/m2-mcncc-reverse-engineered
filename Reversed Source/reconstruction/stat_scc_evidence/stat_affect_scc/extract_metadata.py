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
import json,subprocess
from elftools.elf.elffile import ELFFile
from elftools.dwarf.dwarf_expr import DWARFExprParser
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
p=Path('work/stat_affect_scc');out={'records':[],'methods':[],'strings':[]}
names={'CAffect','FFindNearVictim','packet_affect_remove','packet_remove_affect','packet_create_fly','char_event_info','SPacketGDRemoveAffect','TPacketGDRemoveAffect','TPacketGCAffectRemove','TPacketGCCreateFly','packet_point_change','TSkillUseInfo'};methods={'RemoveAffect','IsAffectFlag','CreateFly','SendAffectRemovePacket','StartMuyeongEvent','ComputeSkill','ComputeAffect','RefreshAffect','operator()'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in ('char_skill.cpp','char_affect.cpp','char_battle.cpp'):continue
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name')
   if d.tag in ('DW_TAG_structure_type','DW_TAG_class_type') and val(d,'DW_AT_byte_size') and (name in names or ('affect' in str(name).lower() and 'packet' in str(name).lower()) or ('fly' in str(name).lower() and 'packet' in str(name).lower())):
    members=[]
    for c in d.iter_children():
     if c.tag not in ('DW_TAG_member','DW_TAG_inheritance'):continue
     a=c.attributes.get('DW_AT_data_member_location');loc=None
     if a:loc=[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if isinstance(a.value,list) else a.value
     members.append({'tag':c.tag,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':loc})
    out['records'].append({'unit':unit,'record':desc(d),'members':members})
   if d.tag=='DW_TAG_subprogram' and name in methods and (val(d,'DW_AT_low_pc') or name!='operator()'):
    out['methods'].append({'unit':unit,'name':name,'die':hex(d.offset),'return':desc(typ(d)),'attrs':{k:val(d,k) for k in ('DW_AT_decl_file','DW_AT_decl_line','DW_AT_inline','DW_AT_low_pc','DW_AT_high_pc','DW_AT_MIPS_linkage_name')},'params':[{'name':val(c,'DW_AT_name'),'type':desc(typ(c))} for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']})
 for addr in [0x8227a62]:
  for s in elf.iter_sections():
   if s['sh_addr']<=addr<s['sh_addr']+s['sh_size']:
    raw=s.data()[addr-s['sh_addr']:].split(b'\0')[0];out['strings'].append({'address':hex(addr),'raw_hex':raw.hex(),'escaped':''.join('\\x%02x'%b for b in raw)});break
p.joinpath('original-metadata.json').write_text(json.dumps(out,indent=2)+'\n')
for name,a,b in [('affect',0x813d2d0,0x813d340),('remove',0x813e780,0x813eba0),('muyoung',0x8073610,0x8073900),('near-victim',0x8075430,0x8075540),('createfly',0x8147740,0x81477b0)]:
 r=subprocess.run([r'C:\Strawberry\c\bin\objdump.exe','-d','--start-address='+hex(a),'--stop-address='+hex(b),_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')],capture_output=True,text=True,check=True);p.joinpath('evidence/'+name+'-assembly.txt').write_text(r.stdout)
print([(r['record']['name'],r['record']['size']) for r in out['records']]);print('No build/ELF execution')
