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
import json,hashlib,sys
p=Path(__file__).parent
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
exec(Path(_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\cleanup_types\\extract.py')).read_text().split("with open(r'F:")[0])
path=Path(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'))
out=[]
with path.open('rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  name=val(cu.get_top_DIE(),'DW_AT_name')
  if name not in ('char.cpp','char_manager.cpp'):continue
  for d in cu.iter_DIEs():
   dn=val(d,'DW_AT_name')
   if d.tag in ('DW_TAG_structure_type','DW_TAG_class_type') and dn in ('CHARACTER','CHARACTER_MANAGER') and val(d,'DW_AT_byte_size'):
    row={'cu':name,'record':desc(d),'members':[],'methods':[]}
    for c in d.iter_children():
     if c.tag in ('DW_TAG_member','DW_TAG_inheritance'):
      a=c.attributes.get('DW_AT_data_member_location')
      row['members'].append({'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'access':val(c,'DW_AT_accessibility'),'location':[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value,list) else a.value if a else None})
     elif c.tag=='DW_TAG_subprogram' and val(c,'DW_AT_name') in ('Save','DelayedSave'):
      row['methods'].append({'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'linkage':val(c,'DW_AT_MIPS_linkage_name'),'access':val(c,'DW_AT_accessibility'),'params':[{'name':val(a,'DW_AT_name'),'type':desc(typ(a))} for a in c.iter_children() if a.tag=='DW_TAG_formal_parameter']})
    out.append(row)
 symbols=[]
 for sec in elf.iter_sections():
  if sec.header.sh_type=='SHT_SYMTAB':
   for s in sec.iter_symbols():
    if s['st_value'] in (0x8128b50,0x818e250):symbols.append({'name':s.name,'address':hex(s['st_value']),'size':s['st_size']})
p.joinpath('full-records-and-save-api.json').write_text(json.dumps(out,indent=2))
p.joinpath('symbols.json').write_text(json.dumps(symbols,indent=2))
print(json.dumps([{'cu':x['cu'],'record':x['record']['name'],'bytes':x['record']['size'],'members':len(x['members']),'methods':[m['name'] for m in x['methods']]} for x in out]))
