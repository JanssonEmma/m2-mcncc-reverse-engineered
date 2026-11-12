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
import json,struct,hashlib
from elftools.elf.elffile import ELFFile
p=Path(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')); b=p.read_bytes(); digest=hashlib.sha256(b).hexdigest(); assert digest=='61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c'
phoff=struct.unpack_from('<I',b,28)[0]; entsize,n=struct.unpack_from('<HH',b,42)
def offset(addr):
 for i in range(n):
  t,off,va,pa,filesz,*_=struct.unpack_from('<8I',b,phoff+i*entsize)
  if t==1 and va<=addr<va+filesz:return off+addr-va
 raise ValueError(addr)
strings={hex(a):b[offset(a):b.index(b'\0',offset(a))].decode() for a in (0x82314e3,0x8231940,0x82314f3,0x8231960,0x82319a0,0x82319e0,0x8231a20,0x8231a60,0x8231a80,0x8231aa0,0x8231ac0,0x8231511)}
result={'sha256':digest,'strings':strings,'float_divisor':struct.unpack_from('<f',b,offset(0x081f7248))[0],'float_bits':b[offset(0x081f7248):offset(0x081f7248)+4].hex()}
with p.open('rb') as f:
 e=ELFFile(f); dw=e.get_dwarf_info()
 for cu in dw.iter_CUs():
  if cu.get_top_DIE().attributes.get('DW_AT_name').value!=b'item_manager.cpp':continue
  for d in cu.iter_DIEs():
   name=d.attributes.get('DW_AT_name')
   if d.tag=='DW_TAG_structure_type' and name and name.value==b'DropEvent_CharStone':
    def attrs(y):return {k:v.value.decode() if isinstance(v.value,bytes) else v.value for k,v in y.attributes.items()}
    result['record']={'attrs':attrs(d),'members':[{'attrs':attrs(c),'type':attrs(c.get_DIE_from_attribute('DW_AT_type'))} for c in d.iter_children() if c.tag=='DW_TAG_member']}
Path('work/drop_event_char_stone/elf-data.json').write_text(json.dumps(result,indent=2))
print(json.dumps(result)[:1700])
