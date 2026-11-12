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
import json,re
root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));r=root/'reconstruction'
inv=json.loads((r/'dwarf-inventory.json').read_text(encoding='utf-8-sig'))
report={'target_sha256':inv['sha256'],'limitations':['Address/body evidence coverage is not proof of source or binary equivalence.','Manager standard-library specialization bodies are provided by the selected C++ library, not manually transcribed.'],'units':[]}
for name,evidence,source in [('buffer.c','buffer-ghidra.json','libthecore/src/buffer.c'),('buffer_manager.cpp','buffer-manager-ghidra.json','game/src/buffer_manager.cpp')]:
 u=next(u for u in inv['units'] if u['source']==name)
 saved=json.loads((r/evidence).read_text(encoding='utf-8-sig'))
 rows=[]
 for f in u['functions']:
  key=f'{int(f["low_pc"],16):08x}';record=saved.get(key)
  standard='/usr/include/c++/' in (f.get('decl_file') or '') and not f['name'].startswith('_GLOBAL__')
  row={'name':f['name'],'address':'0x'+key,'retained_complete_decompilation':bool(record and isinstance(record.get('code'),str) and not record.get('truncated')),'scope':'C++ standard library' if standard else 'application/singleton/startup/ABI variant'}
  if name=='buffer.c':row['source_definition_present']=bool(re.search(r'^'+r'[^\n;]*\b'+re.escape(f['name'])+r'\([^;\n]*\)\s*\{',(root/source).read_text(),re.M))
  rows.append(row)
 report['units'].append({'unit':name,'source':source,'evidence':evidence,'addressed_records':len(rows),'retained_complete':sum(x['retained_complete_decompilation'] for x in rows),'standard_library_records':sum(x['scope']=='C++ standard library' for x in rows),'functions':rows})
(r/'buffer-coverage.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps([{k:v for k,v in u.items() if k!='functions'} for u in report['units']]))
