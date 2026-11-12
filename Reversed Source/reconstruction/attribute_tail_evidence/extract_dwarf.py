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
sys.path.insert(0,str(Path(__file__).resolve().parents[3]/'python-deps'))
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
exec(Path(r'work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
wanted={'CItem','TPlayerItemAttribute','SPlayerItemAttribute','TItemTable','SItemTable'}
methods={'ChangeAttribute','ClearAttribute','CopyAttributeTo','SetAttributes','RemoveAttributeAt','AddAttribute','HasRareAttr','GetRareAttrCount','AddRareAttribute','ChangeRareAttribute'}
out=[]
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  name=val(cu.get_top_DIE(),'DW_AT_name')
  if name not in ('item.cpp','item_attribute.cpp'):continue
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name')
   if n not in wanted|methods:continue
   if n in methods and d.tag!='DW_TAG_subprogram':continue
   if n in wanted and d.tag not in ('DW_TAG_class_type','DW_TAG_structure_type','DW_TAG_typedef'):continue
   x={'cu':name,'node':desc(d),'pc':val(d,'DW_AT_low_pc'),'high_pc':val(d,'DW_AT_high_pc'),'children':[]}
   for c in d.iter_children():
    a=c.attributes.get('DW_AT_data_member_location');loc=None
    if a:loc=[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if isinstance(a.value,list) else a.value
    x['children'].append({'tag':c.tag,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':loc,'pc':val(c,'DW_AT_low_pc'),'linkage':val(c,'DW_AT_MIPS_linkage_name')})
   out.append(x)
Path('work/attribute_tail/full-records-api-dwarf.json').write_text(json.dumps(out,indent=2))
print([(x['cu'],x['node']['name'],x['pc']) for x in out if x['pc']])
