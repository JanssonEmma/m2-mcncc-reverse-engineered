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
import json,sys
sys.path.insert(0,str(Path('work/python-deps').resolve()))
exec(Path('work/cleanup_types/extract.py').read_text(encoding='utf-8').split("with open(r'F:")[0])
p=Path(__file__).resolve().parent;out=[];wanted={'packet_script','OpenState','CloseState','ClearScript','AddScript','SendScript','GotoEndState'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  cuname=val(cu.get_top_DIE(),'DW_AT_name')
  if cuname not in ('questmanager.cpp','questlua.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_name') not in wanted:continue
   fi=val(d,'DW_AT_decl_file');row={'cu':cuname,'node':desc(d),'access':val(d,'DW_AT_accessibility'),'line':val(d,'DW_AT_decl_line'),'file':lp['file_entry'][fi-1].name.decode(errors='replace')if fi else None,'children':[]}
   for c in d.iter_children():
    a=c.attributes.get('DW_AT_data_member_location');loc=None
    if a:loc=[{'op':o.op_name,'args':o.args}for o in ex.parse_expr(a.value)]if isinstance(a.value,list)else a.value
    row['children'].append({'name':val(c,'DW_AT_name'),'tag':c.tag,'type':desc(typ(c)),'location':loc})
   out.append(row)
 (p/'original-api-packet-dwarf.json').write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8');print(json.dumps([x for x in out if x['node']['name']=='packet_script' or x['access'] is not None],indent=2))
