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
exec(Path('work/cleanup_types/extract.py').read_text().split('with open(')[0])
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f);out={'symbols':[],'dwarf':[]}
 for s in e.get_section_by_name('.symtab').iter_symbols():
  if (s['st_value']<=0x083231a8<s['st_value']+s['st_size']) or 'SetMaxItemID' in s.name or 'GetNewID' in s.name:out['symbols'].append({'name':s.name,'address':hex(s['st_value']),'size':s['st_size'],'bind':s['st_info']['bind'],'section':str(s['st_shndx'])})
 dw=e.get_dwarf_info()
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name') not in ('item_manager.cpp','config.cpp','input_db.cpp'):continue
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name')
   if n not in ('GetNewID','SetMaxItemID','g_dwItemIDRange') and not (d.tag=='DW_TAG_variable' and 'DW_AT_location' in d.attributes and d.attributes['DW_AT_location'].value==[3,168,49,50,8]):continue
   out['dwarf'].append({'cu':val(cu.get_top_DIE(),'DW_AT_name'),'name':n,'tag':d.tag,'type':desc(typ(d)),'attrs':{k:val(d,k) for k in ('DW_AT_low_pc','DW_AT_external','DW_AT_location','DW_AT_decl_line','DW_AT_accessibility','DW_AT_MIPS_linkage_name')},'params':[desc(typ(c)) for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']})
 for sec in e.iter_sections():
  if sec['sh_addr']<=0x083231a8<sec['sh_addr']+sec['sh_size']:
   off=0x083231a8-sec['sh_addr'];out['initial']={'section':sec.name,'kind':sec['sh_type'],'hex':sec.data()[off:off+4].hex()};break
 Path('work/point_change/auto_give/get_new_id/metadata.json').write_text(json.dumps(out,indent=2),encoding='utf8');print(json.dumps(out,indent=2))


