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
import json,hashlib
from pathlib import Path
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
out={'target_sha256':hashlib.sha256(Path(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')).read_bytes()).hexdigest(),'records':[],'typedefs':[],'methods':[],'enums':[]}
names={'SGuildMember','TGuildMember','SGuildGrade','TGuildGrade','SGuildData','TGuildData','SGuildWar','TGuildWar','SGuildCreateParameter','TGuildCreateParameter','CGuild','SPacketGuildChangeMemberData','TPacketGuildChangeMemberData','packet_guild','TPacketGCGuild','TGuildMemberContainer','TGuildMemberOnlineContainer'}
methods={'LevelChange','GetMember','GetID','GetLevel','GetPlayerID','SGuildMember','DBPacket','DBPacketHeader','BufferedPacket','Packet'}
units={'guild.cpp','char.cpp','desc_client.cpp','desc.cpp'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 def loc(d):
  a=d.attributes.get('DW_AT_data_member_location')
  return [{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value,list) else a.value if a else None
 def declared_file(d,cu):
  i=val(d,'DW_AT_decl_file');lp=dw.line_program_for_CU(cu)
  if not i or not lp:return None
  return lp['file_entry'][i-1].name.decode('utf8','replace')
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in units:continue
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name')
   if name in names and d.tag in ('DW_TAG_structure_type','DW_TAG_class_type','DW_TAG_typedef'):
    row={'name':name,'unit':unit,'die':hex(d.offset),'tag':d.tag,'decl_file':declared_file(d,cu),'decl_line':val(d,'DW_AT_decl_line'),'size':val(d,'DW_AT_byte_size'),'type':desc(typ(d))}
    if d.tag in ('DW_TAG_structure_type','DW_TAG_class_type') and val(d,'DW_AT_byte_size'):
     row['members']=[{'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':loc(c)} for c in d.iter_children() if c.tag in ('DW_TAG_member','DW_TAG_inheritance')]
     out['records'].append(row)
    elif d.tag=='DW_TAG_typedef':out['typedefs'].append(row)
   if name in methods and d.tag=='DW_TAG_subprogram':
    out['methods'].append({'name':name,'unit':unit,'die':hex(d.offset),'decl_file':declared_file(d,cu),'decl_line':val(d,'DW_AT_decl_line'),'linkage':val(d,'DW_AT_MIPS_linkage_name'),'address':val(d,'DW_AT_low_pc'),'return':desc(typ(d)),'parameters':[{'name':val(c,'DW_AT_name'),'type':desc(typ(c))} for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']})
   if d.tag=='DW_TAG_enumeration_type':
    values=[{'name':val(c,'DW_AT_name'),'value':val(c,'DW_AT_const_value')} for c in d.iter_children() if c.tag=='DW_TAG_enumerator']
    if any(v['name'] in ('GUILD_SUBHEADER_GC_LIST','HEADER_GD_GUILD_CHANGE_MEMBER_DATA','HEADER_GC_GUILD') for v in values):out['enums'].append({'name':name,'unit':unit,'die':hex(d.offset),'decl_file':declared_file(d,cu),'values':values})
p=Path('work/guild_level');(p/'old-types-signatures.json').write_text(json.dumps(out,indent=2))
print(json.dumps({'records':[(r['name'],r['size'],r['decl_file']) for r in out['records'] if r['unit']=='guild.cpp'],'methods':[(r['name'],r['linkage'],r['address'],r['decl_file'],r['decl_line']) for r in out['methods'] if (r['name'] in ('LevelChange','GetMember','SGuildMember') and r['address']) or (r['unit']=='guild.cpp' and r['name'] in ('GetLevel','GetPlayerID'))],'enums':[(e['name'],len(e['values'])) for e in out['enums']]}))
