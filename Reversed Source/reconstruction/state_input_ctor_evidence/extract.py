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
 dw=ELFFile(f).get_dwarf_info()
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name')!='questpc.cpp':continue
  for d in cu.iter_DIEs():
   if d.tag not in ('DW_TAG_subprogram','DW_TAG_inlined_subroutine') or val(d,'DW_AT_name') not in ('RewardData','QuestState'):continue
   o=d
   for k in ('DW_AT_abstract_origin','DW_AT_specification'):
    if k in o.attributes:o=o.get_DIE_from_attribute(k)
   if val(o.get_parent(),'DW_AT_name') not in ('RewardData','QuestState'):continue
   out.append({'cu':'questpc.cpp','die':hex(d.offset),'tag':d.tag,'attrs':{k:val(d,k) for k in ('DW_AT_name','DW_AT_low_pc','DW_AT_high_pc','DW_AT_inline','DW_AT_decl_line','DW_AT_MIPS_linkage_name')},'parameters':[{'name':val(c,'DW_AT_name'),'type':desc(typ(c))} for c in o.iter_children() if c.tag=='DW_TAG_formal_parameter']})
Path(__file__).with_name('ctor-dwarf.json').write_text(json.dumps(out,indent=2))
print(json.dumps([{'die':x['die'],**x['attrs']} for x in out],indent=2))
