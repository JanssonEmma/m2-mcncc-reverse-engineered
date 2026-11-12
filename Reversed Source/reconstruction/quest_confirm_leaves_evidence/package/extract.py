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
exec(Path('work/cleanup_types/extract.py').read_text(encoding='utf-8').split("with open(r'F:")[0])
p=Path(__file__).resolve().parent;out={'symbols':[],'dies':[]};addresses={0x08136310,0x081a5400};wanted={'packet_quest_confirm','TPacketGCQuestConfirm','ConfirmWithMsg','SetSkinStyle'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f)
 for s in elf.get_section_by_name('.symtab').iter_symbols():
  if s['st_value'] in addresses:out['symbols'].append({'name':s.name,'address':'%08x'%s['st_value'],'size':s['st_size']})
 dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  cuName=val(cu.get_top_DIE(),'DW_AT_name')
  if cuName not in ('char.cpp','questmanager.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_name') not in wanted:continue
   fi=val(d,'DW_AT_decl_file');row={'cu':cuName,'node':desc(d),'access':val(d,'DW_AT_accessibility'),'file':lp['file_entry'][fi-1].name.decode(errors='replace')if fi else None,'line':val(d,'DW_AT_decl_line'),'low_pc':val(d,'DW_AT_low_pc'),'children':[]}
   for c in d.iter_children():
    loc=None;a=c.attributes.get('DW_AT_data_member_location')
    if a:loc=[{'op':o.op_name,'args':o.args}for o in ex.parse_expr(a.value)]if isinstance(a.value,list)else a.value
    row['children'].append({'tag':c.tag,'name':val(c,'DW_AT_name'),'artificial':val(c,'DW_AT_artificial'),'type':desc(typ(c)),'location':loc})
   out['dies'].append(row)
 (p/'ELF_API_PACKET_AUDIT.json').write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8');print(json.dumps({'symbols':out['symbols'],'records':[x for x in out['dies']if x['node']['tag']=='DW_TAG_structure_type']},indent=2))
