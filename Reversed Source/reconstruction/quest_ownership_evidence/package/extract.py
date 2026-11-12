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
import sys,json,hashlib
from pathlib import Path
sys.path.insert(0,str(Path('work/python-deps').resolve()))
exec(Path('work/cleanup_types/extract.py').read_text(encoding='utf-8').split("with open(r'F:")[0])
p=Path('work/quest_ownership');addresses={int(a,16) for a in ['081a0720','0819fa20','0819fe70','081a02c0','081adf50','081ae0c0','081ae180','081ae260','081ab460']};report=dict(target_sha256='61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c',symbols=[],records=[],api=[])
full=json.loads(Path('work/quest_level/full-quest-api.json').read_text(encoding='utf-8-sig'))
for name in ['CQuestManager','NPC']:
 row=next(x for x in full if x['record']['name']==name);report['api'].append(row)
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as stream:
 elf=ELFFile(stream);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for sec in elf.iter_sections():
  if sec.header.sh_type=='SHT_SYMTAB':
   for sym in sec.iter_symbols():
    if sym['st_value'] in addresses:report['symbols'].append(dict(address='%08x'%sym['st_value'],name=sym.name,size=sym['st_size']))
 seen=set()
 for cu in dw.iter_CUs():
  if val(cu.get_top_DIE(),'DW_AT_name') not in ['questmanager.cpp','questnpc.cpp']:continue
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name')
   if name not in ['CQuestManager','NPC'] or not val(d,'DW_AT_byte_size') or name in seen:continue
   seen.add(name);row=dict(cu=val(cu.get_top_DIE(),'DW_AT_name'),record=desc(d),members=[])
   for c in d.iter_children():
    if c.tag not in ['DW_TAG_member','DW_TAG_inheritance']:continue
    loc=c.attributes.get('DW_AT_data_member_location');location=[dict(op=o.op_name,args=o.args) for o in ex.parse_expr(loc.value)] if loc and isinstance(loc.value,list) else (loc.value if loc else None)
    row['members'].append(dict(name=val(c,'DW_AT_name'),tag=c.tag,type=desc(typ(c)),location=location,access=val(c,'DW_AT_accessibility')))
   report['records'].append(row)
(p/'ELF_OWNERSHIP_API_AUDIT.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8');print('Owners:',[(r['record']['name'],r['record']['size'],len(r['members'])) for r in report['records']]);print('NPCAPI',sum(x['tag']=='DW_TAG_subprogram' for x in next(r for r in report['api'] if r['record']['name']=='NPC')['children']))
