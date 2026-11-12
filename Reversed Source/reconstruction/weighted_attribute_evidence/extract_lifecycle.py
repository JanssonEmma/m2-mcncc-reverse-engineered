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
import sys,json
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
exec(Path(r'work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();cu=next(c for c in dw.iter_CUs() if val(c.get_top_DIE(),'DW_AT_name')=='constants.cpp')
 out=[{'name':val(d,'DW_AT_name'),'address':'%08x'%val(d,'DW_AT_low_pc')} for d in cu.iter_DIEs() if d.tag=='DW_TAG_subprogram' and val(d,'DW_AT_low_pc') and ('__static_initialization' in str(val(d,'DW_AT_name')) or '_GLOBAL__' in str(val(d,'DW_AT_name')))]
 names={0x81681e0,0x8186ec0,0x8187060,0x81874d0}
 symbols=[{'symbol':s.name,'address':'%08x'%s['st_value'],'size':s['st_size'],'kind':s['st_info']['type']} for s in elf.get_section_by_name('.symtab').iter_symbols() if s['st_value'] in names]
 p=Path(__file__).resolve().parent;(p/'lifecycle-addresses.json').write_text(json.dumps(out,indent=2));(p/'method-symbols.json').write_text(json.dumps(symbols,indent=2));print(out)
