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
import json
p=_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')
with open(p,'rb') as f:
 d=ELFFile(f).get_dwarf_info()
 for cu in d.iter_CUs():
  n=cu.get_top_DIE().attributes.get('DW_AT_name')
  if not n or n.value!=b'utils.c': continue
  for die in cu.iter_DIEs():
   n=die.attributes.get('DW_AT_name')
   if not n or n.value!=b'core_dump_unix': continue
   print('function',hex(die.offset))
   for ch in die.iter_children():
    if ch.tag!='DW_TAG_formal_parameter': continue
    chain=[];x=ch
    while x is not None:
     chain.append({'tag':x.tag,'offset':hex(x.offset),'name':str(x.attributes.get('DW_AT_name')),'size':str(x.attributes.get('DW_AT_byte_size'))})
     x=x.get_DIE_from_attribute('DW_AT_type') if 'DW_AT_type' in x.attributes else None
    print(json.dumps(chain))
