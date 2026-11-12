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
import json,re
from pathlib import Path
r=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction'))
inv=json.loads((r/'dwarf-inventory.json').read_text(encoding='utf-8-sig'))
u=next(x for x in inv['units'] if x['source']=='fdwatch.c')
records=json.loads((r/'fdwatch-ghidra.json').read_text(encoding='utf-8-sig'))
source=(r.parent/'libthecore/src/fdwatch.c').read_text()
rows=[]
for f in u['functions']:
 key=f'{int(f["low_pc"],16):08x}';rec=records.get(key)
 rows.append({'name':f['name'],'address':'0x'+key,'complete_decompilation':bool(rec and rec.get('code') and not rec.get('truncated')),'source_definition_present':bool(re.search(r'^'+r'[^\n;]*\b'+re.escape(f['name'])+r'\([^;]*\)\s*\{',source,re.M))})
report={'target_sha256':inv['sha256'],'scope':'All 12 addressed fdwatch.c functions, including external register helper. Body/name coverage is not proof of equivalence.','functions':rows,'missing_evidence':[x['name'] for x in rows if not x['complete_decompilation']],'missing_definitions':[x['name'] for x in rows if not x['source_definition_present']]}
(r/'fdwatch-coverage.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps({'total':len(rows),'missing_evidence':report['missing_evidence'],'missing_definitions':report['missing_definitions']}))
