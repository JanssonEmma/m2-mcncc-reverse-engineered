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
from elftools.elf.elffile import ELFFile
import json,struct
names={'core_dump_unix','thecore_msleep','rdtsc','get_float_time'}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f);d=e.get_dwarf_info();rows=[]
 def chain(x):
  out=[]
  while x is not None:
   a=x.attributes;out.append({'die':hex(x.offset),'tag':x.tag,'name':a['DW_AT_name'].value.decode('utf8','replace') if 'DW_AT_name' in a else None,'byte_size':a['DW_AT_byte_size'].value if 'DW_AT_byte_size' in a else None})
   x=x.get_DIE_from_attribute('DW_AT_type') if 'DW_AT_type' in a else None
  return out
 for cu in d.iter_CUs():
  a=cu.get_top_DIE().attributes
  if a.get('DW_AT_name') is None or a['DW_AT_name'].value!=b'utils.c':continue
  for x in cu.iter_DIEs():
   a=x.attributes;n=a.get('DW_AT_name')
   if x.tag!='DW_TAG_subprogram' or n is None or n.value.decode('utf8','replace') not in names:continue
   rows.append({'name':n.value.decode(),'die':hex(x.offset),'return_chain':chain(x.get_DIE_from_attribute('DW_AT_type')) if 'DW_AT_type' in a else [],'parameters':[chain(c) for c in x.iter_children() if c.tag=='DW_TAG_formal_parameter']})
 raw=None
 for s in e.iter_sections():
  if s['sh_addr']<=0x8202920<s['sh_addr']+s['sh_size']:
   raw=s.data()[0x8202920-s['sh_addr']:0x8202920-s['sh_addr']+4]
 print(json.dumps({'types':rows,'float_divisor':{'address':'0x08202920','bytes':raw.hex(),'float_le':struct.unpack('<f',raw)[0]}},indent=2))
