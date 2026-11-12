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
 dw=ELFFile(f).get_dwarf_info()
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in ('constants.cpp','main.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name') or ''
   if name!='CTableBySkill' or not val(d,'DW_AT_byte_size'):continue
   idx=val(d,'DW_AT_decl_file')
   x={'unit':unit,'file':lp['file_entry'][idx-1].name.decode(),'size':val(d,'DW_AT_byte_size'),'methods':[]}
   for c in d.iter_children():
    if c.tag!='DW_TAG_subprogram':continue
    x['methods'].append({'name':val(c,'DW_AT_name'),'return':desc(typ(c)),'line':val(c,'DW_AT_decl_line'),'linkage':val(c,'DW_AT_MIPS_linkage_name'),'parameters':[desc(typ(a)) for a in c.iter_children() if a.tag=='DW_TAG_formal_parameter']})
   out.append(x)
Path('work/skill_power/class-api.json').write_text(json.dumps(out,indent=2))
print(json.dumps(out))
