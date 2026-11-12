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
import json,re,hashlib,ast
p=Path('work/pc_quest_info');root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));sources={str(f.relative_to(root)).replace('\\','/'):f for f in (root/'game/src').glob('*.cpp')};matrix=[]
for f in (p/'evidence').glob('*-calls.json'):
 x=json.loads(f.read_text(encoding='utf-8-sig'));rows=[]
 for name in sorted(set(y['name'] for y in x.get('callees',{}).get('items',[]))):
  found=[]
  for rel,file in sources.items():
   s=file.read_text(encoding='utf-8-sig',errors='replace')
   for m in re.finditer(re.escape(name)+r'\s*\(',s):
    a=s[m.end():];end=a.find(')');suffix=a[end+1:].lstrip() if end>=0 else ''
    if suffix.startswith('{'):found.append({'path':rel,'line':s.count('\n',0,m.start())+1,'sha256':hashlib.sha256(file.read_bytes()).hexdigest()})
  state='actual-current-source' if found else 'standard-runtime/header or staged-group'
  if name.startswith('PC::') and name!='PC::SetSendFlag':state='complete-staged-quest-info-group'
  if name in ('sys_log','_sys_err'):state='actual-core-logging'
  rows.append({'name':name,'state':state,'definitions':found,'call_sites':[y['call_site'] for y in x.get('callees',{}).get('items',[]) if y['name']==name]})
 matrix.append({'root':x['function'],'has_more':x.get('callees',{}).get('has_more'),'callees':rows})
p.joinpath('dependency-matrix.json').write_text(json.dumps({'graphs':matrix,'inlined_dependency':{'name':'CQuestManager::GetCurrentCharacterPtr','status':'genuine original missing headerinline accessor staged; complete existing field, no othercallee','parent_instruction':'0805f135 load [manager+0x7c]','not_standalone_export':True},'data_gap':'packet_quest_info6 bytes missingproduction, complete source staged','enum_gap':'7usedmask values proven instructions; reference-only enumerator names not found in scannedoldDWARF, not promoted','unproven_unused_API':'PC::ClearSendFlag declaration only; sender directly clearsfield, no fake body supplied'},indent=2)+'\n',encoding='utf-8')
for f in p.glob('*.py'):ast.parse(f.read_text(encoding='utf-8-sig'))
s=(p/'questpc.cpp.candidate').read_text(encoding='utf-8-sig');assert len(re.findall(r'\b(?:void|int) PC::',s))==13
stripped=re.sub(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'','',s,flags=re.S)
assert stripped.count('{')==stripped.count('}')
print('Preparation only:13fullbody candidates, exactcallmatrix, PythonAST/braces. No builds/apply/tests.')
