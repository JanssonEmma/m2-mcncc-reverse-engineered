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
out={'target_sha256':hashlib.sha256(Path(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')).read_bytes()).hexdigest(),'records':[],'typedefs':[],'methods':[],'enums':[],'character_members':[]}
names={'SPlayerSkill','TPlayerSkill','packet_point_change','TPacketGCPointChange','character_point','character_point_instant','time_t','__time_t'}
methods={'RaceToJob','GetJob','GetName','IsDead','IsStun','NewPlayerTable2'}
units={'char.cpp','char_battle.cpp','input_login.cpp','constants.cpp','item.cpp'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in units:continue
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name')
   if name in names and d.tag in ('DW_TAG_structure_type','DW_TAG_typedef'):
    row={'name':name,'unit':unit,'die':hex(d.offset),'tag':d.tag,'size':val(d,'DW_AT_byte_size'),'type':desc(typ(d))}
    if d.tag=='DW_TAG_structure_type':
     row['members']=[]
     for c in d.iter_children():
      if c.tag!='DW_TAG_member':continue
      a=c.attributes.get('DW_AT_data_member_location')
      row['members'].append({'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value,list) else a.value if a else None})
     out['records'].append(row)
    else:out['typedefs'].append(row)
   if name in methods and d.tag=='DW_TAG_subprogram':
    out['methods'].append({'name':name,'unit':unit,'die':hex(d.offset),'linkage':val(d,'DW_AT_MIPS_linkage_name'),'address':val(d,'DW_AT_low_pc'),'return':desc(typ(d)),'parameters':[{'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'artificial':val(c,'DW_AT_artificial')} for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']})
   if d.tag=='DW_TAG_enumeration_type':
    values=[{'name':val(c,'DW_AT_name'),'value':val(c,'DW_AT_const_value')} for c in d.iter_children() if c.tag=='DW_TAG_enumerator']
    if any((v['name'] or '').startswith(('MAIN_RACE_','POS_')) for v in values):out['enums'].append({'name':name,'unit':unit,'size':val(d,'DW_AT_byte_size'),'values':values})
   if name=='CHARACTER' and d.tag in ('DW_TAG_class_type','DW_TAG_structure_type') and val(d,'DW_AT_byte_size'):
    for c in d.iter_children():
     if c.tag!='DW_TAG_member' or val(c,'DW_AT_name') not in ('m_points','m_pointsInstant'):continue
     a=c.attributes.get('DW_AT_data_member_location')
     out['character_members'].append({'unit':unit,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value,list) else a.value if a else None})
Path('work/stat_primitives/old-types-signatures.json').write_text(json.dumps(out,indent=2))
print(json.dumps({'hash':out['target_sha256'],'records':[(r['name'],r['size'],r['unit']) for r in out['records']],'typedefs':[(r['name'],r['unit']) for r in out['typedefs']],'enums':[(r['name'],len(r['values'])) for r in out['enums']]}))
