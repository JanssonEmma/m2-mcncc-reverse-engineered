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
import json,hashlib
from pathlib import Path
from elftools.elf.elffile import ELFFile
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
names={'exp_table','exp_table_euckr','exp_table_gb2312','exp_table_sjis','exp_table_hk','exp_table_newcibn'}
out={'symbols':[],'dwarf':[]}
with open(_resolve_portable_path('PROJECT_ROOT\\\\Original Binaries\\\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info()
 for sec in elf.iter_sections():
  if sec['sh_type']!='SHT_SYMTAB':continue
  for sym in sec.iter_symbols():
   if sym.name not in names:continue
   s=elf.get_section(sym['st_shndx']); off=sym['st_value']-s['sh_addr']; raw=s.data()[off:off+sym['st_size']] if s['sh_type']!='SHT_NOBITS' else bytes(sym['st_size'])
   out['symbols'].append({'name':sym.name,'address':hex(sym['st_value']),'size':sym['st_size'],'section':s.name,'flags':s['sh_flags'],'raw_hex':raw.hex(),'sha256':hashlib.sha256(raw).hexdigest(),'u32':[int.from_bytes(raw[i:i+4],'little') for i in range(0,len(raw),4)]})
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in ('constants.cpp','locale_service.cpp'):continue
  for d in cu.iter_DIEs():
   if d.tag=='DW_TAG_variable' and val(d,'DW_AT_name') in names:out['dwarf'].append({'name':val(d,'DW_AT_name'),'unit':unit,'type':desc(typ(d))})
Path('work/exp_locale/data.json').write_text(json.dumps(out,indent=2))
print([(s['name'],s['address'],s['section'],s['size']) for s in out['symbols']])
