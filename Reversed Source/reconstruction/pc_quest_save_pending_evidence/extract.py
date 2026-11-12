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
import sys,json
from pathlib import Path
sys.path.insert(0,str(Path('work/python-deps').resolve()))
exec(Path('work/cleanup_types/extract.py').read_text(encoding='utf-8').split("with open(r'F:")[0])
p=Path(__file__).resolve().parent;out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name')!='questpc.cpp':continue
  lp=dw.line_program_for_CU(cu)
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name')
   if n not in ('SQuestTable','TQuestTable','s_table','lValue','stComp','iPos','r') or (n not in ('SQuestTable','TQuestTable') and not (d.get_parent() and val(d.get_parent(),'DW_AT_name')=='Save')):continue
   row={'cu':'questpc.cpp','node':desc(d),'file':val(d,'DW_AT_decl_file'),'line':val(d,'DW_AT_decl_line'),'children':[]}
   fi=val(d,'DW_AT_decl_file')
   if fi:row['file_name']=lp['file_entry'][fi-1].name.decode(errors='replace')
   for c in d.iter_children():
    a=c.attributes.get('DW_AT_data_member_location');loc=None
    if a:loc=[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if isinstance(a.value,list) else a.value
    row['children'].append({'name':val(c,'DW_AT_name'),'tag':c.tag,'type':desc(typ(c)),'location':loc})
   out.append(row)
 (p/'save-record-dwarf.json').write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
 print(json.dumps(out,indent=2))
