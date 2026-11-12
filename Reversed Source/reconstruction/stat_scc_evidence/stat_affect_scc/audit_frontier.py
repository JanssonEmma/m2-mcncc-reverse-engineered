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
import json,re,hashlib,shutil
p=Path('work/stat_affect_scc');root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));src={str(f.relative_to(root)).replace('\\','/'):f for f in (root/'game/src').glob('*.cpp')};graphs=[]
for f in (p/'evidence').glob('*-calls.json'):
 x=json.loads(f.read_text(encoding='utf-8-sig'));rows=[]
 for name in sorted(set(y['name'] for y in x.get('callees',{}).get('items',[]))):
  if name.startswith('<EXTERNAL>') or name.startswith(('_Rb_tree','std::','list<','map<')):state='external/STL-real-runtime';defs=[]
  else:
   defs=[]
   for rel,s in src.items():
    text=s.read_text(errors='replace')
    for m in re.finditer(re.escape(name)+r'\s*\(',text):
     aft=text[m.end():];end=aft.find(')');suffix=aft[end+1:].lstrip() if end>=0 else ''
     if suffix.startswith('{'):defs.append({'file':rel,'line':text.count('\n',0,m.start())+1,'sha256':hashlib.sha256(s.read_bytes()).hexdigest()})
   state='actual-current-source' if defs else 'not-found-current-game-source; verify header/library or pending'
   if name in ('CPoly::SetVar','CPoly::Eval'):state='actual-full-libpoly'
   if name in ('_sys_err','sys_log','number_ex','DISTANCE_APPROX','__AllocEventInfo','event_create_ex'):state='actual-existing-core/event'
   if name in ('locale_find','LC_IsYMIR'):state='actual-existing-locale'
  rows.append({'name':name,'state':state,'definitions':defs,'call_sites':[y['call_site'] for y in x.get('callees',{}).get('items',[]) if y['name']==name]})
 graphs.append({'root':x['function'],'callee_total':x.get('callees',{}).get('total'),'has_more':x.get('callees',{}).get('has_more'),'callees':rows})
p.joinpath('transitive-frontier.json').write_text(json.dumps(graphs,indent=2)+'\n');x=json.loads((p/'evidence/0806f710-decompile.json').read_text(encoding='utf-8-sig'));p.joinpath('evidence/0806f710-decompile.c').write_text(x['code'].replace('\r\n','\n'));shutil.copyfile('work/stat_compute_scc/evidence/skill_muyoung_event-ghidra.json',p/'evidence/skill_muyoung_event-ghidra.json');shutil.copyfile('work/mount_polymorph/08073890-decompile.c',p/'evidence/08073890-decompile.c')
missing=next(x for x in graphs if x['root']['address']=='0806f710');print('Full ComputeSkill182callsite frontier saved, missing-current unique:',[r['name'] for r in missing['callees'] if r['state'].startswith('not-found')])
