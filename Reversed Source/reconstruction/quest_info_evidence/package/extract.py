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
from elftools.elf.elffile import ELFFile
from elftools.dwarf.dwarf_expr import DWARFExprParser
exec(Path('work/cleanup_types/extract.py').read_text(encoding='utf-8-sig').split("with open(r'F:")[0])
import json,subprocess
p=Path('work/pc_quest_info');out={'records':[],'enums':[],'methods':[],'inline':[]};names={'SetCurrentQuestStartFlag','SetCurrentQuestDoneFlag','ClearCurrentQuestBeginFlag','SetCurrentQuestBeginFlag','GetCurrentQuestBeginFlag','SetCurrentQuestTitle','SetQuestTitle','SetCurrentQuestClockName','SetCurrentQuestClockValue','SetCurrentQuestCounterName','SetCurrentQuestCounterValue','SetCurrentQuestIconFile','ClearSendFlag','SendQuestInfoPakcet','GetCurrentCharacterPtr'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 dw=ELFFile(f).get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in ('questpc.cpp','questmanager.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  def place(d):
   i=val(d,'DW_AT_decl_file');return {'file':lp['file_entry'][i-1].name.decode() if i else None,'line':val(d,'DW_AT_decl_line')}
  for d in cu.iter_DIEs():
   n=val(d,'DW_AT_name')
   if d.tag=='DW_TAG_enumeration_type':
    vv=[{'name':val(c,'DW_AT_name'),'value':val(c,'DW_AT_const_value')} for c in d.iter_children() if c.tag=='DW_TAG_enumerator']
    if any(str(v['name']).startswith('QUEST_SEND_') for v in vv):out['enums'].append({'unit':unit,'name':n,'size':val(d,'DW_AT_byte_size'),'placement':place(d),'values':vv})
   if d.tag in ('DW_TAG_structure_type','DW_TAG_class_type') and n=='packet_quest_info' and val(d,'DW_AT_byte_size'):
    r={'unit':unit,'record':desc(d),'placement':place(d),'fields':[]}
    for c in d.iter_children():
     if c.tag!='DW_TAG_member':continue
     a=c.attributes.get('DW_AT_data_member_location');loc=[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if a and isinstance(a.value,list) else (a.value if a else None);r['fields'].append({'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':loc})
    out['records'].append(r)
   if d.tag in ('DW_TAG_subprogram','DW_TAG_inlined_subroutine') and n in names:
    r={'unit':unit,'name':n,'tag':d.tag,'die':hex(d.offset),'placement':place(d),'return':desc(typ(d)),'attrs':{k:val(d,k) for k in ('DW_AT_inline','DW_AT_low_pc','DW_AT_high_pc','DW_AT_MIPS_linkage_name')},'params':[{'name':val(c,'DW_AT_name'),'type':desc(typ(c))} for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']};out['inline' if d.tag=='DW_TAG_inlined_subroutine' else 'methods'].append(r)
p.joinpath('metadata.json').write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
for n,a,b in [('sender',0x805f020,0x805f4d0),('setters',0x80603e0,0x8060770),('start-done',0x805efc0,0x805f020)]:
 r=subprocess.run([r'C:\Strawberry\c\bin\objdump.exe','-d','--start-address='+hex(a),'--stop-address='+hex(b),_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')],capture_output=True,text=True,check=True);p.joinpath('evidence/'+n+'-assembly.txt').write_text(r.stdout,encoding='utf-8')
print('records',[(r['record']['name'],r['record']['size'],[(q['name'],q['location']) for q in r['fields']]) for r in out['records'][:1]]);print('enum',out['enums'][:1]);print('getter inline',[(hex(r['attrs']['DW_AT_low_pc'] or 0),hex(r['attrs']['DW_AT_high_pc'] or 0),r['placement']) for r in out['inline'] if r['name'] in ('GetCurrentCharacterPtr','ClearSendFlag')][:5])
