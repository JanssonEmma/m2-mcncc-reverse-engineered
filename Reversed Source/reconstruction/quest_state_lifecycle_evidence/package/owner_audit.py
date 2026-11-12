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
exec(Path('work/cleanup_types/extract.py').read_text(encoding='utf-8').split("with open(r'F:")[0])
addresses={0x0805cd10,0x0805cd80,0x0805d000,0x081a4ee0,0x081a4f30,0x081a4f50};out={'symbols':[],'dies':[]}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f)
 for s in elf.get_section_by_name('.symtab').iter_symbols():
  if s['st_value'] in addresses:out['symbols'].append({'name':s.name,'address':'%08x'%s['st_value'],'size':s['st_size'],'type':s['st_info']['type']})
 dw=elf.get_dwarf_info()
 for cu in dw.iter_CUs():
  cuname=val(cu.get_top_DIE(),'DW_AT_name')
  if cuname not in ('quest.cpp','questlua.cpp','questmanager.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_low_pc') not in addresses:continue
   fi=val(d,'DW_AT_decl_file');parents=[];o=d.get_parent()
   while o is not None:parents.append({'tag':o.tag,'name':val(o,'DW_AT_name'),'die':hex(o.offset)});o=o.get_parent()
   out['dies'].append({'cu':cuname,'name':val(d,'DW_AT_name'),'tag':d.tag,'die':hex(d.offset),'low_pc':'%08x'%val(d,'DW_AT_low_pc'),'linkage':val(d,'DW_AT_MIPS_linkage_name'),'decl_file':lp['file_entry'][fi-1].name.decode(errors='replace')if fi else None,'decl_line':val(d,'DW_AT_decl_line'),'parents':parents,'type':desc(typ(d)),'parameters':[{'name':val(c,'DW_AT_name'),'artificial':val(c,'DW_AT_artificial'),'type':desc(typ(c))}for c in d.iter_children()if c.tag=='DW_TAG_formal_parameter']})
Path('work/quest_state_lifecycle/ELF_OWNER_AUDIT.json').write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8');print(json.dumps(out,indent=2))
