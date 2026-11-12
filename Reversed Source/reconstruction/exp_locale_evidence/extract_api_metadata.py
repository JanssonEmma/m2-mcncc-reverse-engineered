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
out={'target_sha256':hashlib.sha256(Path(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')).read_bytes()).hexdigest(),'methods':[],'records':[],'inline_ranges':[]}
names={'GetNextExp','LocaleService_Init','LocaleService_TransferDefaultSetting','LocaleService_GetBasePath','LocaleService_GetMapPath','LocaleService_GetQuestPath','Check','SetSkillPowerByLevelFromType','DeleteSkillPowerByLevelFromType'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 def file(d,cu):
  idx=val(d,'DW_AT_decl_file');lp=dw.line_program_for_CU(cu)
  return lp['file_entry'][idx-1].name.decode('utf8','replace') if idx and lp else None
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in ('char.cpp','locale_service.cpp','constants.cpp','config.cpp'):continue
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name')
   if name in names and d.tag=='DW_TAG_subprogram':
    out['methods'].append({'name':name,'unit':unit,'die':hex(d.offset),'decl_file':file(d,cu),'decl_line':val(d,'DW_AT_decl_line'),'linkage':val(d,'DW_AT_MIPS_linkage_name'),'return':desc(typ(d)),'address':val(d,'DW_AT_low_pc'),'parameters':[{'name':val(c,'DW_AT_name'),'type':desc(typ(c))} for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']})
   if name=='GetNextExp' and d.tag=='DW_TAG_inlined_subroutine':
    out['inline_ranges'].append({'unit':unit,'die':hex(d.offset),'low_pc':val(d,'DW_AT_low_pc'),'high_pc':val(d,'DW_AT_high_pc'),'ranges':val(d,'DW_AT_ranges'),'origin':d.attributes.get('DW_AT_abstract_origin').value if 'DW_AT_abstract_origin' in d.attributes else None})
   if name=='CTableBySkill' and d.tag in ('DW_TAG_structure_type','DW_TAG_class_type') and val(d,'DW_AT_byte_size'):
    row={'unit':unit,'name':name,'size':val(d,'DW_AT_byte_size'),'decl_file':file(d,cu),'members':[]}
    for c in d.iter_children():
     if c.tag not in ('DW_TAG_member','DW_TAG_inheritance'):continue
     a=c.attributes.get('DW_AT_data_member_location')
     row['members'].append({'name':val(c,'DW_AT_name'),'tag':c.tag,'type':desc(typ(c)),'location':[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value,list) else a.value if a else None})
    out['records'].append(row)
Path('work/exp_locale/api-metadata.json').write_text(json.dumps(out,indent=2))
print(json.dumps({'GetNextExp':[r for r in out['methods'] if r['name']=='GetNextExp'][:2],'CTableBySkill':[(r['unit'],r['size'],[(m['name'],m['location']) for m in r['members']]) for r in out['records']],'inline_ranges':len(out['inline_ranges']),'LocaleAPI':[(r['name'],r['linkage']) for r in out['methods'] if r['unit']=='locale_service.cpp' and r['address']]}))
