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
 e=ELFFile(f); dw=e.get_dwarf_info(); out=[]
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name')!='db.cpp':continue
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_name')!='DBManager' or d.tag not in ('DW_TAG_class_type','DW_TAG_structure_type'):continue
   x={'node':desc(d),'children':[]}
   for c in d.iter_children():
    z={'tag':c.tag,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'attrs':{k:val(c,k) for k in ('DW_AT_accessibility','DW_AT_virtuality','DW_AT_vtable_elem_location','DW_AT_data_member_location','DW_AT_decl_file','DW_AT_decl_line','DW_AT_MIPS_linkage_name')}}
    z['parameters']=[{'name':val(p,'DW_AT_name'),'type':desc(typ(p))} for p in c.iter_children() if p.tag=='DW_TAG_formal_parameter'];x['children'].append(z)
   out.append(x)
 addr=0x0822ddf9
 for sec in e.iter_sections():
  if sec['sh_addr']<=addr<sec['sh_addr']+sec['sh_size']:
   b=sec.data()[addr-sec['sh_addr']:].split(b'\0')[0];Path('work/point_change/auto_give/chat-format.json').write_text(json.dumps({'address':hex(addr),'hex':b.hex(),'cpp_octal':''.join('\\%03o'%v for v in b),'cp949':b.decode('cp949')},ensure_ascii=False,indent=2),encoding='utf8');break
 Path('work/point_change/auto_give/dbmanager-dwarf.json').write_text(json.dumps(out,indent=2),encoding='utf8')
 print([(x['node']['size'],len(x['children'])) for x in out])

