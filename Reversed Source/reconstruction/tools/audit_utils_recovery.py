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
root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
r=root/'reconstruction'
inv=json.loads((r/'dwarf-inventory.json').read_text(encoding='utf-8-sig'))
unit=next(u for u in inv['units'] if u['source']=='utils.c')
wanted={int(f['low_pc'],16):f['name'] for f in unit['functions']}
records={};origins={}
def walk(x,origin):
 if isinstance(x,dict):
  if isinstance(x.get('code'),str) and x.get('function',{}).get('address'):
   a=int(x['function']['address'],16)
   if a in wanted:
    records[f'{a:08x}']=x;origins.setdefault(a,set()).add(origin)
  for k,v in x.items():
   if k!='content':walk(v,origin)
 elif isinstance(x,list):
  for v in x:walk(v,origin)
for p in sorted(r.glob('*ghidra.json')):
 if p.name=='utils-ghidra.json':continue
 walk(json.loads(p.read_text(encoding='utf-8-sig')),p.name)
source=(root/'libthecore/src/utils.c').read_text()
rows=[]
for a,n in sorted(wanted.items()):
 rows.append({'name':n,'address':f'0x{a:08x}','evidence_files':sorted(origins.get(a,[])),'source_definition_present':bool(re.search(r'^'+r'[^\n;]*\b'+re.escape(n)+r'\([^;\n]*\)\s*\{',source,re.M)),'decompilation_retained':f'{a:08x}' in records})
summary={'target_sha256':inv['sha256'],'scope':'All addressed subprograms in utils.c DWARF compilation unit; source name coverage is not a semantic equivalence check.','functions':rows,'missing_evidence':[x['name'] for x in rows if not x['decompilation_retained']],'missing_definitions':[x['name'] for x in rows if not x['source_definition_present']]}
(r/'utils-ghidra.json').write_text(json.dumps(records,indent=2),encoding='utf-8')
(r/'utils-coverage.json').write_text(json.dumps(summary,indent=2),encoding='utf-8')
print(json.dumps({'addressed_functions':len(rows),'missing_evidence':summary['missing_evidence'],'missing_definitions':summary['missing_definitions']}))
