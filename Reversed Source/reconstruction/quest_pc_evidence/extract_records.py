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
sys.path.insert(0,_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\python-deps'))
exec(Path(r'work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
p=Path(__file__).resolve().parent;out=[]
wanted={'PC','RewardData','TQuestStateChangeInfo','QuestState','quest_event_info','quest_server_event_info'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  name=val(cu.get_top_DIE(),'DW_AT_name')
  if name not in ('questpc.cpp','questevent.cpp','questmanager.cpp','questlua.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name')
   if n not in wanted or d.tag not in ('DW_TAG_structure_type','DW_TAG_class_type') or not val(d,'DW_AT_byte_size'):continue
   x={'cu':name,'record':desc(d),'children':[]}
   for c in d.iter_children():
    if c.tag not in ('DW_TAG_member','DW_TAG_subprogram','DW_TAG_typedef','DW_TAG_enumeration_type','DW_TAG_inheritance'):continue
    a=c.attributes.get('DW_AT_data_member_location');loc=None
    if a:loc=[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if isinstance(a.value,list) else a.value
    fi=val(c,'DW_AT_decl_file')
    row={'tag':c.tag,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'access':val(c,'DW_AT_accessibility'),'linkage':val(c,'DW_AT_MIPS_linkage_name'),'artificial':val(c,'DW_AT_artificial'),'location':loc,'file':lp['file_entry'][fi-1].name.decode(errors='replace') if fi else None,'line':val(c,'DW_AT_decl_line'),'inline':val(c,'DW_AT_inline')}
    if c.tag=='DW_TAG_subprogram':row['params']=[{'name':val(q,'DW_AT_name'),'type':desc(typ(q)),'artificial':val(q,'DW_AT_artificial')} for q in c.iter_children() if q.tag=='DW_TAG_formal_parameter']
    if c.tag=='DW_TAG_enumeration_type':row['enumerators']=[{'name':val(q,'DW_AT_name'),'value':val(q,'DW_AT_const_value')} for q in c.iter_children()]
    x['children'].append(row)
   out.append(x)
 symbols=[{'symbol':s.name,'address':'%08x'%s['st_value'],'size':s['st_size'],'bind':s['st_info']['bind']} for s in elf.get_section_by_name('.symtab').iter_symbols() if s['st_info']['type']=='STT_FUNC' and (s.name.startswith('_ZN5quest2PC') or s.name.startswith('_ZN5quest10QuestStateC') or s.name.startswith('_ZN5quest10RewardData') or s.name.startswith('_ZN5quest16CancelTimerEvent') or 'quest_event_info' in s.name)]
(p/'pc-full-record-api-dwarf.json').write_text(json.dumps(out,indent=2));(p/'pc-symbols.json').write_text(json.dumps(symbols,indent=2))
print([(x['cu'],x['record']['name'],x['record']['size']) for x in out])
