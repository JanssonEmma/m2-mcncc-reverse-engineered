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
import json,hashlib,struct
from pathlib import Path
exec(Path('work/cleanup_types/extract.py').read_text().split("with open(r'F:")[0])
exact={'check_name','is_twobyte','g_stServiceName','g_stServiceBasePath','g_stServiceMapPath','g_stLocaleFilename','g_stQuestDir','g_setQuestObjectDir','g_stLocal','g_stLocale','PK_PROTECT_LEVEL','g_iUseLocale','LC_InitLocalization'}
prefix=('aiPercentByDeltaLev','aiChainLightningCountBySkillLevel')
def wanted(n):return n in exact or n.startswith(prefix)
out={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','symbols':[],'metadata':[]}
with open(_resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game'),'rb') as f:
 elf=ELFFile(f);dw=elf.get_dwarf_info()
 for sec in elf.iter_sections():
  if sec['sh_type']!='SHT_SYMTAB':continue
  for sym in sec.iter_symbols():
   if not wanted(sym.name):continue
   idx=sym['st_shndx']
   if not isinstance(idx,int):continue
   s=elf.get_section(idx);off=sym['st_value']-s['sh_addr']
   raw=bytes(sym['st_size']) if s['sh_type']=='SHT_NOBITS' else s.data()[off:off+sym['st_size']]
   out['symbols'].append({'name':sym.name,'address':hex(sym['st_value']),'size':sym['st_size'],'section':s.name,'flags':s['sh_flags'],'raw_hex':raw.hex(),'sha256':hashlib.sha256(raw).hexdigest(),'i32':list(struct.unpack('<'+'i'*(len(raw)//4),raw)) if len(raw)%4==0 else None})
 for cu in dw.iter_CUs():
  unit=val(cu.get_top_DIE(),'DW_AT_name')
  if unit not in ('locale_service.cpp','constants.cpp','config.cpp'):continue
  lp=dw.line_program_for_CU(cu)
  for d in cu.iter_DIEs():
   name=val(d,'DW_AT_name') or ''
   if not (wanted(name) or name.startswith(('is_twobyte_','check_name_','LocaleService_Get'))):continue
   if d.tag not in ('DW_TAG_variable','DW_TAG_subprogram'):continue
   ix=val(d,'DW_AT_decl_file')
   out['metadata'].append({'name':name,'tag':d.tag,'unit':unit,'die':hex(d.offset),'file':lp['file_entry'][ix-1].name.decode('utf8','replace') if ix else None,'line':val(d,'DW_AT_decl_line'),'type':desc(typ(d)),'parameters':[{'name':val(c,'DW_AT_name'),'type':desc(typ(c))} for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter'],'low_pc':val(d,'DW_AT_low_pc'),'linkage':val(d,'DW_AT_MIPS_linkage_name'),'external':val(d,'DW_AT_external')})
Path('work/locale_closure/data-metadata.json').write_text(json.dumps(out,indent=2))
print(json.dumps({'symbols':[(s['name'],s['address'],s['size'],s['section']) for s in out['symbols']],'metadata_count':len(out['metadata'])}))
