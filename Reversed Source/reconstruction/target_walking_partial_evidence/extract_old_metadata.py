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
out={'target_sha256':hashlib.sha256(Path(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')).read_bytes()).hexdigest(),'records':[],'typedefs':[],'methods':[],'enums':[],'character_fields':[],'elf_strings':[]}
names={'packet_target','TPacketGCTarget','SPacketGCWalkMode','TPacketGCWalkMode'}
methods={'BroadcastTargetPacket','SetNowWalking','PacketView','MonsterLog','PacketMonsterLog'}
fields={'m_bCharType','m_bNowWalking','m_bWalking','m_dwWalkStartTime','m_set_pkChrTargetedBy','m_points','m_pointsInstant','m_vid'}
units={'char.cpp','entity.cpp','char_manager.cpp','input_main.cpp','item.cpp'}
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
   if name in names and d.tag in ('DW_TAG_structure_type','DW_TAG_typedef'):
    row={'name':name,'unit':unit,'die':hex(d.offset),'decl_file':declared_file(d,cu),'decl_line':val(d,'DW_AT_decl_line'),'tag':d.tag,'size':val(d,'DW_AT_byte_size'),'type':desc(typ(d))}
    if d.tag=='DW_TAG_structure_type':
     row['members']=[{'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':loc(c)} for c in d.iter_children() if c.tag=='DW_TAG_member'];out['records'].append(row)
    else:out['typedefs'].append(row)
   if name in methods and d.tag=='DW_TAG_subprogram':
    out['methods'].append({'name':name,'unit':unit,'die':hex(d.offset),'decl_file':declared_file(d,cu),'decl_line':val(d,'DW_AT_decl_line'),'linkage':val(d,'DW_AT_MIPS_linkage_name'),'address':val(d,'DW_AT_low_pc'),'return':desc(typ(d)),'parameters':[{'name':val(c,'DW_AT_name'),'type':desc(typ(c))} for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']})
   if d.tag=='DW_TAG_enumeration_type':
    values=[{'name':val(c,'DW_AT_name'),'value':val(c,'DW_AT_const_value')} for c in d.iter_children() if c.tag=='DW_TAG_enumerator']
    if any(v['name'] in ('CHAR_TYPE_PC','WALKMODE_RUN','HEADER_GC_TARGET','HEADER_GC_WALK_MODE') for v in values):out['enums'].append({'name':name,'unit':unit,'die':hex(d.offset),'decl_file':declared_file(d,cu),'decl_line':val(d,'DW_AT_decl_line'),'size':val(d,'DW_AT_byte_size'),'values':values})
   if name=='CHARACTER' and d.tag in ('DW_TAG_structure_type','DW_TAG_class_type') and val(d,'DW_AT_byte_size'):
    out['character_fields'] += [{'unit':unit,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':loc(c)} for c in d.iter_children() if c.tag=='DW_TAG_member' and val(c,'DW_AT_name') in fields]
 for name,address in [('walking_log',0x8226bc5),('running_log',0x8226bcc),('target_error_format',0x8226b2a),('target_error_function',0x82249e3)]:
  for sec in elf.iter_sections():
   if sec['sh_addr']<=address<sec['sh_addr']+sec['sh_size'] and sec['sh_type']!='SHT_NOBITS':
    raw=sec.data()[address-sec['sh_addr']:address-sec['sh_addr']+128].split(b'\0')[0]
    out['elf_strings'].append({'name':name,'address':hex(address),'hex':raw.hex(),'escaped_bytes':''.join('\\x%02x'%b for b in raw),'utf8_or_euckr':raw.decode('euc_kr','replace')})
    break
p=Path('work/target_walking');(p/'old-types-signatures-strings.json').write_text(json.dumps(out,indent=2))
print(json.dumps({'records':[(r['name'],r['size']) for r in out['records']],'enums':[(e['name'],len(e['values'])) for e in out['enums']],'strings':out['elf_strings']}))
