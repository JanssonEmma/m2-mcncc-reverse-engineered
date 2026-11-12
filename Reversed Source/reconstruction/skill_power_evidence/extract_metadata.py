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
import json
from pathlib import Path
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in ('skill_power.cpp','main.cpp'):continue
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name') or ''
   if not (name=='CTableBySkill' or 'singleton<CTableBySkill>' in name or (unit=='skill_power.cpp' and d.tag=='DW_TAG_subprogram')):continue
   x={'unit':unit,'node':desc(d),'attrs':{k:val(d,k) for k in ('DW_AT_decl_file','DW_AT_decl_line','DW_AT_low_pc','DW_AT_MIPS_linkage_name','DW_AT_virtuality')},'children':[]}
   for c in d.iter_children():
    if c.tag not in ('DW_TAG_member','DW_TAG_inheritance','DW_TAG_subprogram','DW_TAG_formal_parameter','DW_TAG_variable'):continue
    a=c.attributes.get('DW_AT_data_member_location')
    x['children'].append({'tag':c.tag,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value,list) else a.value if a else None,'line':val(c,'DW_AT_decl_line'),'linkage':val(c,'DW_AT_MIPS_linkage_name'),'low_pc':val(c,'DW_AT_low_pc'),'virtuality':val(c,'DW_AT_virtuality'),'access':val(c,'DW_AT_accessibility'),'params':[{'name':val(a,'DW_AT_name'),'type':desc(typ(a))} for a in c.iter_children() if a.tag=='DW_TAG_formal_parameter']})
   out.append(x)
Path('work/skill_power/metadata.json').write_text(json.dumps(out,indent=2))
print(json.dumps([(r['unit'],r['node']['name'],r['node']['size'],r['attrs']['DW_AT_low_pc']) for r in out]))
