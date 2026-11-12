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
out={'units':[],'functions':[],'inline_constructors':[]}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info()
 for cu in dw.iter_CUs():
  top=cu.get_top_DIE();name=val(top,'DW_AT_name');lo=val(top,'DW_AT_low_pc');hi=val(top,'DW_AT_high_pc')
  out['units'].append({'name':name,'low_pc':lo,'high_pc':hi})
  if not ((lo is not None and hi is not None and lo<=0x80e6820<hi) or name in ('main.cpp','constants.cpp')):continue
  lp=dw.line_program_for_CU(cu)
  def file(d):
   idx=val(d,'DW_AT_decl_file')
   return lp['file_entry'][idx-1].name.decode('utf8','replace') if idx else None
  for d in cu.iter_DIEs():
   low=val(d,'DW_AT_low_pc')
   if d.tag=='DW_TAG_subprogram' and low is not None and 0x80e6800<=low<=0x80e6a30:
    out['functions'].append({'unit':name,'die':hex(d.offset),'name':val(d,'DW_AT_name'),'file':file(d),'line':val(d,'DW_AT_decl_line'),'low_pc':low,'high_pc':val(d,'DW_AT_high_pc'),'linkage':val(d,'DW_AT_MIPS_linkage_name')})
   if d.tag=='DW_TAG_inlined_subroutine' and val(d,'DW_AT_name')=='CTableBySkill':
    out['inline_constructors'].append({'unit':name,'die':hex(d.offset),'file':file(d),'line':val(d,'DW_AT_decl_line'),'low_pc':low,'high_pc':val(d,'DW_AT_high_pc'),'ranges':val(d,'DW_AT_ranges')})
Path('work/skill_power/source-placement.json').write_text(json.dumps(out,indent=2))
print(json.dumps({k:v for k,v in out.items() if k!='units'}))
