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
import struct,json,zlib
from elftools.elf.elffile import ELFFile
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 e=ELFFile(f)
 for s in e.get_section_by_name('.symtab').iter_symbols():
  if 'CRCTable' not in s.name or s['st_size'] != 1024 or s['st_info']['type'] != 'STT_OBJECT':continue
  sec=e.get_section(s['st_shndx']);o=s['st_value']-sec['sh_addr'];raw=sec.data()[o:o+s['st_size']];words=list(struct.unpack('<256I',raw))
  table=[]
  for n in range(256):
   x=n
   for bit in range(8):x=(x>>1)^ (0xedb88320 if x&1 else 0)
   table.append(x)
  print(json.dumps({'symbol':s.name,'address':hex(s['st_value']),'size':s['st_size'],'matches_reflected_polynomial_edb88320':table==words,'words':words,'golden_crc':{x.decode('ascii'):hex(zlib.crc32(x)) for x in (b'123456789',b'abcdefghijklmnopqrstuvwxyz0123456789',b'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789')},'vector_constants':{'epsilon_double':struct.unpack('<d',bytes.fromhex('11ea2d819997713d'))[0],'radians_to_degrees_float':struct.unpack('<f',bytes.fromhex('e02e6542'))[0],'degrees_to_radians_float':struct.unpack('<f',bytes.fromhex('35fa8e3c'))[0]}},indent=2))

