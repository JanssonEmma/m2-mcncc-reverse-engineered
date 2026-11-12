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
import pathlib,sys
exec(pathlib.Path('work/cleanup_types/extract.py').read_text().split('with open(')[0])
want={'CMonarch','TMonarchInfo','tMonarchCandidacy','MonarchCandidacy','TPacketChangeMonarchLord','tPacketChangeMonarchLord','tag_ChangeMonarchLord','ROUND','HEADER_GD_CHANGE_MONARCH_LORD'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();ex=DWARFExprParser(dw.structs);out=[]
 for cu in dw.iter_CUs():
  name=val(cu.get_top_DIE(),'DW_AT_name')
  if name not in ('questlua_mgmt.cpp','monarch.cpp'):continue
  def row(d):
   a=d.attributes.get('DW_AT_data_member_location');loc=None
   if a:loc=[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if isinstance(a.value,list) else a.value
   return {'tag':d.tag,'name':val(d,'DW_AT_name'),'type':desc(typ(d)),'location':loc,'low_pc':val(d,'DW_AT_low_pc'),'linkage':val(d,'DW_AT_MIPS_linkage_name'),'access':val(d,'DW_AT_accessibility'),'artificial':val(d,'DW_AT_artificial'),'virtuality':val(d,'DW_AT_virtuality')}
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_name') not in want:continue
   out.append({'cu':name,'decl_file': (dw.line_program_for_CU(cu)['file_entry'][val(d,'DW_AT_decl_file')-1].name.decode() if val(d,'DW_AT_decl_file') else None),'decl_line':val(d,'DW_AT_decl_line'),'node':desc(d),'children':[dict(row(c),args=[row(y) for y in c.iter_children() if y.tag=='DW_TAG_formal_parameter']) for c in d.iter_children()]})
 pathlib.Path('work/quest_lua_mgmt/owner-dwarf.json').write_text(json.dumps(out,indent=2))
 for r in out:
  print(r['cu'],r['node']);
  for c in r['children']:
   if c['tag']!='DW_TAG_subprogram':print(c)

