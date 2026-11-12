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
import json,hashlib,subprocess
from elftools.elf.elffile import ELFFile
from elftools.dwarf.dwarf_expr import DWARFExprParser
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
p=Path('work/stat_apply_scc');out={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','declarations':[],'strings':[]}
names={'ApplyPoint','ModifyPoints','AccessorySocketDegrade','StartAccessorySocketExpireEvent','SetAccessorySocketExpireEvent','StopAccessorySocketExpireEvent','accessory_socket_expire_event','item_vid_event_info'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info();ex=DWARFExprParser(dw.structs)
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in ('char.cpp','item.cpp'):continue
  for d in cu.iter_DIEs():
   if val(d,'DW_AT_name') not in names or d.tag not in ('DW_TAG_subprogram','DW_TAG_structure_type'):continue
   row={'unit':unit,'tag':d.tag,'name':val(d,'DW_AT_name'),'die':hex(d.offset),'type':desc(typ(d)),'attrs':{k:val(d,k) for k in ('DW_AT_decl_file','DW_AT_decl_line','DW_AT_inline','DW_AT_low_pc','DW_AT_high_pc','DW_AT_byte_size','DW_AT_MIPS_linkage_name','DW_AT_accessibility')},'children':[]}
   for c in d.iter_children():
    if c.tag not in ('DW_TAG_member','DW_TAG_formal_parameter'):continue
    a=c.attributes.get('DW_AT_data_member_location');loc=None
    if a:loc=[{'op':o.op_name,'args':o.args} for o in ex.parse_expr(a.value)] if isinstance(a.value,list) else a.value
    row['children'].append({'tag':c.tag,'name':val(c,'DW_AT_name'),'type':desc(typ(c)),'location':loc})
   out['declarations'].append(row)
 for addr in [0x822bd20]:
  for sec in elf.iter_sections():
   if sec['sh_addr']<=addr<sec['sh_addr']+sec['sh_size']:
    raw=sec.data()[addr-sec['sh_addr']:].split(b'\0')[0];out['strings'].append({'address':hex(addr),'raw_hex':raw.hex(),'escaped':''.join('\\x%02x'%b for b in raw),'decoded_cp949':raw.decode('cp949','replace')});break
p.joinpath('original-api.json').write_text(json.dumps(out,indent=2)+'\n')
for name,start,end in [('ApplyPoint',0x812f7d0,0x812fbb0),('ModifyPoints',0x8167420,0x81679b0),('accessory',0x8168990,0x8168d20)]:
 r=subprocess.run([r'C:\Strawberry\c\bin\objdump.exe','-d','--start-address='+hex(start),'--stop-address='+hex(end),_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')],capture_output=True,text=True,check=True);p.joinpath(name+'-assembly.txt').write_text(r.stdout)
print('Saved original metadata and static disassembly only')
