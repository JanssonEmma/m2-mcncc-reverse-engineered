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
unit=next(u for u in inv['units'] if u['source']=='socket.c')
wanted={int(f['low_pc'],16):f['name'] for f in unit['functions']}
records={};origins={}
def walk(x,path):
 if isinstance(x,dict):
  if isinstance(x.get('code'),str) and x.get('function',{}).get('address'):
   a=int(x['function']['address'],16)
   if a in wanted: records[f'{a:08x}']=x;origins.setdefault(a,set()).add(path.name)
  for k,v in x.items():
   if k!='content':walk(v,path)
 elif isinstance(x,list):
  for v in x:walk(v,path)
for p in r.glob('socket-*-ghidra.json'):walk(json.loads(p.read_text(encoding='utf-8-sig')),p)
source=(root/'libthecore/src/socket.c').read_text()
rows=[]
for a,n in sorted(wanted.items()):
 rec=records.get(f'{a:08x}');rows.append({'name':n,'address':f'0x{a:08x}','evidence':sorted(origins.get(a,[])),'complete_decompilation':bool(rec and not rec.get('truncated')),'source_definition_present':bool(re.search(r'^'+r'[^\n;]*\b'+re.escape(n)+r'\([^;]*\)\s*\{',source,re.M))})
report={'target_sha256':inv['sha256'],'scope':'All addressed socket.c DWARF functions; source-name coverage is not semantic or binary equivalence.','functions':rows,'missing_evidence':[x['name'] for x in rows if not x['complete_decompilation']],'missing_definitions':[x['name'] for x in rows if not x['source_definition_present']]}
(r/'socket-ghidra.json').write_text(json.dumps(records,indent=2),encoding='utf-8')
(r/'socket-coverage.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps({'addressed_functions':len(rows),'missing_evidence':report['missing_evidence'],'missing_definitions':report['missing_definitions']}))
