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
from elftools.elf.elffile import ELFFile
from elftools.dwarf.dwarf_expr import DWARFExprParser
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
out={'records':[],'enums':[],'globals':[],'methods':[]}
recordnames={'character_point','character_point_instant','SJobInitialPoints','SApplyInfo','packet_point_change','SPlayerSkill','TMobTable','SMobTable'}
methodnames={'PointChange','ApplyPoint','ComputeBattlePoints','ComputePoints','GetPoint','SetPoint','GetRealPoint','SetRealPoint','GetPolymorphPoint','GetLimitPoint','CheckMaximumPoints','ModifyPoints'}
globalnames={'JobInitialPoints','aApplyInfo','exp_table','exp_table_common','exp_table_euckr','exp_table_newcibn','exp_table_cibn'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for sec in elf.iter_sections():
  if sec['sh_type'] not in ('SHT_SYMTAB','SHT_DYNSYM'):continue
  for sym in sec.iter_symbols():
   if sym.name not in globalnames:continue
   row={'name':sym.name,'address':hex(sym['st_value']),'size':sym['st_size']}
   for s in elf.iter_sections():
    if s['sh_addr']<=sym['st_value'] and sym['st_value']+sym['st_size']<=s['sh_addr']+s['sh_size'] and s['sh_type']!='SHT_NOBITS':
     raw=s.data()[sym['st_value']-s['sh_addr']:sym['st_value']-s['sh_addr']+sym['st_size']]
     row.update(raw_hex=raw.hex(),sha256=hashlib.sha256(raw).hexdigest())
     if sym.name=='aApplyInfo':row['byte_values']=list(raw)
     if sym.name=='JobInitialPoints':row['signed_int_rows']=[[int.from_bytes(raw[i+j:i+j+4],'little',signed=True) for j in range(0,64,4)] for i in range(0,len(raw),64)]
     break
   out['globals'].append(row)
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in ('char.cpp','constants.cpp','item.cpp'):continue
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name')
   if d.tag=='DW_TAG_enumeration_type':
    children=[{'name':val(c,'DW_AT_name'),'value':val(c,'DW_AT_const_value')} for c in d.iter_children() if c.tag=='DW_TAG_enumerator']
    if any((c['name'] or '').startswith(('POINT_','APPLY_','JOB_')) for c in children):out['enums'].append({'name':name,'unit':unit,'size':val(d,'DW_AT_byte_size'),'values':children})
   if name in recordnames and d.tag in ('DW_TAG_structure_type','DW_TAG_class_type') and val(d,'DW_AT_byte_size'):
    members=[]
    for c in d.iter_children():
     if c.tag!='DW_TAG_member':continue
     a=c.attributes.get('DW_AT_data_member_location')
     members.append({'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value,list) else a.value if a else None})
    out['records'].append({'name':name,'unit':unit,'size':val(d,'DW_AT_byte_size'),'members':members})
   if name in methodnames and d.tag=='DW_TAG_subprogram':
    out['methods'].append({'name':name,'unit':unit,'address':val(d,'DW_AT_low_pc'),'return':desc(typ(d)),'parameters':[{'name':val(p,'DW_AT_name'),'type':desc(typ(p))} for p in d.iter_children() if p.tag=='DW_TAG_formal_parameter']})
Path('work/stat_dependency/old-data-metadata.json').write_text(json.dumps(out,indent=2))
print(json.dumps({'records':[(x['name'],x['size']) for x in out['records']],'globals':[(x['name'],x['address'],x['size']) for x in out['globals']],'enum_count':len(out['enums'])}))
