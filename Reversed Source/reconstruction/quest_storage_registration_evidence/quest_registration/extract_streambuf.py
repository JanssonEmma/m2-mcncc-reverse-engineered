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
p=Path('work/quest_registration');out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name')!='questmanager.cpp':continue
  for d in cu.iter_DIEs():
   if d.tag not in ('DW_TAG_structure_type','DW_TAG_class_type') or not (val(d,'DW_AT_name') or '').startswith('basic_streambuf<char,'):continue
   for c in d.iter_children():
    if c.tag!='DW_TAG_subprogram':continue
    a=c.attributes.get('DW_AT_vtable_elem_location')
    if a:out.append({'owner':val(d,'DW_AT_name'),'name':val(c,'DW_AT_name'),'virtuality':val(c,'DW_AT_virtuality'),'vtable_location':[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)],'type':desc(typ(c))})
(p/'streambuf-virtual-layout.json').write_text(json.dumps(out,indent=2));print(json.dumps([x for x in out if x['name']=='underflow'],indent=2))
