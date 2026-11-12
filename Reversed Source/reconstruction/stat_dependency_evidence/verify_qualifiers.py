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
rows=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info()
 for sec in elf.iter_sections():
  if sec['sh_type'] not in ('SHT_SYMTAB','SHT_DYNSYM'):continue
  for sym in sec.iter_symbols():
   if sym.name not in ('JobInitialPoints','aApplyInfo'):continue
   section=elf.get_section(sym['st_shndx'])
   rows.append({'kind':'ELF_symbol','name':sym.name,'section':section.name,'section_flags':int(section['sh_flags']),'writable':bool(section['sh_flags']&1),'address':hex(sym['st_value']),'size':sym['st_size']})
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in ('char.cpp','constants.cpp','item.cpp'):continue
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name')
   if name in ('JobInitialPoints','aApplyInfo','TJobInitialPoints','TApplyInfo','SJobInitialPoints','SApplyInfo') and d.tag in ('DW_TAG_variable','DW_TAG_typedef','DW_TAG_structure_type'):
    rows.append({'kind':'DWARF','name':name,'unit':unit,'tag':d.tag,'die':hex(d.offset),'type':desc(typ(d)),'size':val(d,'DW_AT_byte_size'),'external':val(d,'DW_AT_external'),'declaration':val(d,'DW_AT_declaration')})
p=Path('work/stat_dependency')
(p/'constant-qualifiers-provenance.json').write_text(json.dumps(rows,indent=2))
d=json.loads((p/'old-data-metadata.json').read_text())
e=next(e for e in d['enums'] if e['name']=='EMisc')
(p/'common-length-emisc.inc').write_text('// Complete old EMisc. Replace conflicting anonymous CHAT_MAX_LEN, not append a duplicate.\nenum EMisc {\n'+''.join('    '+v['name']+' = '+str(v['value'])+',\n' for v in e['values'])+'};\n')
print(json.dumps(rows[:2]));print('EMisc enumerators',len(e['values']))
