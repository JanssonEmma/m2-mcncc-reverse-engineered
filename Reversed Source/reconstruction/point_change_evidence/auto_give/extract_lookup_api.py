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
from elftools.elf.elffile import ELFFile
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\\\Original Binaries\\\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info()
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit!='item_manager.cpp':continue
  for d in cu.iter_DIEs():
   if d.tag=='DW_TAG_subprogram' and val(d,'DW_AT_name') in ('GetTable','RealNumber'):
    out.append({'name':val(d,'DW_AT_name'),'unit':unit,'tag':d.tag,'parent_name':val(d.get_parent(),'DW_AT_name'),'parent_tag':d.get_parent().tag,'die':hex(d.offset),'access':val(d,'DW_AT_accessibility'),'linkage':val(d,'DW_AT_MIPS_linkage_name'),'low_pc':val(d,'DW_AT_low_pc'),'return':desc(typ(d)),'parameters':[{'name':val(c,'DW_AT_name'),'type':desc(typ(c))} for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']})
Path('work/point_change/auto_give/lookup-api-dwarf.json').write_text(json.dumps(out,indent=2))
for x in out:print(x['name'],x['parent_name'],x['access'],x['linkage'],x['low_pc'],x['return'])
