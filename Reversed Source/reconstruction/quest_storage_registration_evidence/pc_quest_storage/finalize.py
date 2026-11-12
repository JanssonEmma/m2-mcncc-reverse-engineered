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
import json,hashlib,re,ast
p=Path('work/pc_quest_storage');root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));target='61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c'
all=json.loads((p/'evidence/storage-all-ghidra.json').read_text(encoding='utf-8-sig'));selected=[r for r in all['functions'] if r['function']['address'] not in ('0805e380','0805e2a0')];assert len(selected)==7
p.joinpath('normalized-originals.json').write_text(json.dumps({'target_sha256':target,'origin':'7complete old PC quest-storage bodies;4inline semantic bodies separately reviewed with parent instruction provenance, no invented addresses.','functions':selected},indent=2)+'\n',encoding='utf-8');reviews=[]
for r in selected:
 name=r['function']['name'];anchor=name+'('
 if name=='PC::SetQuestState':anchor='void PC::SetQuestState(const std::string& name,int next)'
 reviews.append({'address':r['function']['address'],'source_refs':[{'path':'game/src/questpc.cpp','anchor':anchor}],'test_refs':[{'path':'reconstruction/tests/pc_quest_storage_regression.cpp','anchor':'int main('}],'review_scope':'Complete original PC storage method; full actual manager name query/hash, owned PC/QuestState/STL/flag/pending/timer/event lifecycle, external logger callbacks only. ClearQuest original matching-nonzero flag key-alias UAF domain excluded runtime, full body preserved. Default GetQuest counters remain uninitialized until explicit test initialization.','notes':'PENDING parent normal/O2/strict/full validation. No VM/Lua state-name resolution or AArgScript ctor recovery claim.'})
p.joinpath('mappings.pending.json').write_text(json.dumps({'target_sha256':target,'reviews':reviews},indent=2)+'\n',encoding='utf-8')
meta=json.loads((p/'metadata.json').read_text(encoding='utf-8-sig'));p.joinpath('inline-scope.json').write_text(json.dumps({'scope':'Original3inline iterator helpers + inline TQuestStateChangeInfo3field constructor. NO standalone export/status invented.','methods':{n:[r for r in meta['methods']+meta['inline_instances'] if r['name']==n] for n in ('quest_begin','quest_end','quest_find','TQuestStateChangeInfo')},'AArgScript':'Unproven scalar defaults: no standalone ctor and only default inline instance is optimizer-dead. No body added; tests use legal null pointer vector elements, never fake pointee objects.'},indent=2)+'\n',encoding='utf-8')
sources={str(f.relative_to(root)).replace('\\','/'):f for f in (root/'game/src').glob('*.cpp')};matrix=[]
for f in (p/'evidence').glob('*-calls.json'):
 x=json.loads(f.read_text(encoding='utf-8-sig'));rows=[]
 for name in sorted(set(y['name'] for y in x.get('callees',{}).get('items',[]))):
  found=[]
  for rel,file in sources.items():
   s=file.read_text(encoding='utf-8-sig',errors='replace')
   for m in re.finditer(re.escape(name)+r'\s*\(',s):
    aft=s[m.end():];end=aft.find(')');suffix=aft[end+1:].lstrip() if end>=0 else ''
    if suffix.startswith('{'):found.append({'path':rel,'line':s.count('\n',0,m.start())+1,'sha256':hashlib.sha256(file.read_bytes()).hexdigest()})
  state='actual-current-source' if found else 'original-standard-library/header or pending'
  if name.startswith('PC::') and name.split('::')[1] in ['HasQuest','GetQuest','GetCurrentQuestIndex','SetQuest','SetQuestState','AddQuestStateChange','ClearQuest']:state='staged-closed-PC-storage' if x['function']['address'] not in ('0805e380','0805e2a0') else 'check-full-Lua-wrapper-frontier'
  if name=='CQuestManager::GetQuestIndexByName':state='actual-applied-manager-query100'
  if name=='CQuestManager::GetQuestStateIndex':state='PENDING-full-Lua-state-resolution'
  if name.startswith('QuestState::'):state='actual-full-record constructor/implicit memberwise lifecycle, counter caveat'
  rows.append({'name':name,'state':state,'definitions':found,'call_sites':[y['call_site'] for y in x.get('callees',{}).get('items',[]) if y['name']==name]})
 matrix.append({'root':x['function'],'has_more':x.get('callees',{}).get('has_more'),'callees':rows})
p.joinpath('dependency-matrix.json').write_text(json.dumps(matrix,indent=2)+'\n',encoding='utf-8')
for f in p.glob('*.py'):ast.parse(f.read_text(encoding='utf-8-sig'))
plan=json.loads((p/'integration/plan.json').read_text(encoding='utf-8-sig'));assert len(plan['files'])==6
for row in plan['files']:assert hashlib.sha256((p/'integration/payload'/row['path']).read_bytes()).hexdigest()==row['after']
make=(p/'integration/payload/reconstruction/tests/Makefile').read_text(encoding='utf-8-sig');registry=re.search(r'^RUNTIME_TESTS = (.*)$',make,re.M).group(1).split();lines=make.splitlines();start=next(i for i,l in enumerate(lines) if l.startswith('check:'));executed=[]
for l in lines[start+1:]:
 if not l.startswith('\t'):break
 m=re.search(r'\./([A-Za-z0-9_]+)\$\(EXEEXT\)',l)
 if m:executed.append(m.group(1))
assert executed==registry and len(registry)==101 and registry[0]=='pc_quest_storage_regression' and registry[1]=='quest_manager_indices_regression'
for f in [p/'pc_quest_storage_regression.cpp',p/'pc_quest_storage_abi.cpp',p/'questpc.cpp.fragment']:
 s=f.read_text(encoding='utf-8-sig');s=re.sub(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'','',s,flags=re.S)
 for a,b in [('(',')'),('{','}'),('[',']')]:
  count=0
  for c in s:
   if c==a:count+=1
   if c==b:count-=1
   assert count>=0,(str(f),a)
  assert count==0,(str(f),a,count)
print('Static only: 7originals/4inline scope,6payloadSHA,101canonical executions==registry, Python AST/C++delimiters; no builds')
