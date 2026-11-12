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
from elftools.elf.elffile import ELFFile
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
import json
out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info()
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name') not in ('char_affect.cpp','char_skill.cpp','char_battle.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name')
   if n not in ('SPacketGDRemoveAffect','packet_fly','TPacketGCAffectRemove','FFindNearVictim') or not val(d,'DW_AT_byte_size'):continue
   idx=val(d,'DW_AT_decl_file');entry=lp['file_entry'][idx-1];out.append({'name':n,'unit':val(cu.get_top_DIE(),'DW_AT_name'),'file':entry.name.decode(),'line':val(d,'DW_AT_decl_line'),'die':hex(d.offset)})
Path('work/stat_affect_scc/original-placement.json').write_text(json.dumps(out,indent=2)+'\n');print(out[:4])
