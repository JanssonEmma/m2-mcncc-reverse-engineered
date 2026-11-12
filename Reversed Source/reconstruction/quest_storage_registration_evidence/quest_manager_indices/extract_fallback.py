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
sys.path.insert(0,str(Path('work/python-deps').resolve()))
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info()
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name')!='questmanager.cpp':continue
  for d in cu.iter_DIEs():
   if d.tag!='DW_TAG_variable' or val(d,'DW_AT_name')!='st':continue
   names=[];par=d.get_parent()
   while par is not None:
    names.append(val(par,'DW_AT_name'));par=par.get_parent()
   if 'GetQuestNameByIndex' not in names:continue
   out.append({'die':hex(d.offset),'owner_chain':names,'type':desc(typ(d)),'attrs':{k:val(d,k) for k in ('DW_AT_declaration','DW_AT_external','DW_AT_decl_line','DW_AT_location','DW_AT_MIPS_linkage_name')}})
 syms=[{'name':s.name,'address':format(s['st_value'],'08x'),'size':s['st_size']} for s in elf.get_section_by_name('.symtab').iter_symbols() if 'GetQuestNameByIndex' in s.name]
p=Path('work/quest_manager_indices');(p/'fallback-static-dwarf-symbols.json').write_text(json.dumps({'variables':out,'symbols':syms},indent=2));print(json.dumps(out,indent=2))
q=p/'questmanager.cpp.fragment';s=q.read_text().replace('static std::string empty;\n        return empty;','static std::string st="";\n        return st;');q.write_text(s)
