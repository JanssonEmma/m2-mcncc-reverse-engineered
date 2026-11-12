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
# Parent-only renderer. FullNPCsource propagates beside EVERY audited actualmanager.cpp occurrence.
from pathlib import Path
import json,hashlib,re,ast
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'));m=load(p/'MANIFEST.json');h=lambda b:hashlib.sha256(b).hexdigest();out=p/'integration';changes={};before={}
for row in m['current_read_guards']+m['sdk_expected_files']:assert h((root/row['path']).read_bytes())==row['sha256'],'STALE '+row['path']
for row in m['files']:
 q=root/row['path'];assert (h(q.read_bytes()) if q.exists() else None)==row['before'];data=(p/row['candidate']).read_bytes();assert h(data)==row['after'];changes[row['path']]=data;before[row['path']]=row['before']
for name in ['quest_ownership_regression.cpp','quest_ownership_abi.cpp','quest_ownership_desc_output_syscalls.c']:
 rel='reconstruction/tests/'+name;assert not(root/rel).exists();changes[rel]=(p/name).read_bytes();before[rel]=None
rel='reconstruction/tests/Makefile';raw=(root/rel).read_bytes();before[rel]=h(raw);s=raw.decode('utf-8-sig');assert 'quest_ownership_regression' not in s and '../../game/src/questnpc.cpp' not in s;registry_before=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split();assert len(registry_before)==113,'Expectedfutureverified113baseline'
manager='../../game/src/questmanager.cpp';npc='../../game/src/questnpc.cpp';rows=[];lines=s.splitlines(keepends=True)
for i,line in enumerate(lines):
 if manager not in line:continue
 kind='recipe' if line.startswith('\t') else ('source_assignment' if '=' in line and ':' not in line.split('=',1)[0] else 'target_dependencies');assert not any(t in line for t in ['filter-','findstring','foreach','wildcard']);assert line.count(manager)==1
 rows.append(dict(line=i+1,kind=kind,before=line.rstrip('\n')));lines[i]=line.replace(manager,manager+' '+npc)
assert len(rows)==13 and {k:sum(r['kind']==k for r in rows) for k in ['source_assignment','target_dependencies','recipe']}==dict(source_assignment=1,target_dependencies=6,recipe=6)
s=''.join(lines);assert s.count(manager)==s.count(npc)==13;s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = quest_ownership_regression ',1);lines=s.splitlines(keepends=True);at=next(i for i,l in enumerate(lines) if l.startswith('check:'));lines.insert(at+1,'\t./quest_ownership_regression$(EXEEXT)\n');s=''.join(lines);registry=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split();ls=s.splitlines();at=next(i for i,l in enumerate(ls) if l.startswith('check:'));executions=[]
for l in ls[at+1:]:
 if not l.startswith('\t'):break
 a=re.search(r'\./([A-Za-z0-9_]+)\$\(EXEEXT\)',l)
 if a:executions.append(a.group(1))
assert registry==executions and len(registry)==114 and registry.count('quest_ownership_regression')==1;changes[rel]=(s+'\n'+(p/'Makefile.addition').read_text(encoding='utf-8-sig')).encode('utf-8')
plan=dict(root=str(root),scope='WholeactualCQM/NPCowninglifecycle, globalfixture retirement/actualNPCsource propagation; PREPAREDONLY NEXTWINDOW; no compile/runtimepass.',runtime_count_after=114,dependencies=m['current_read_guards']+m['sdk_expected_files'],files=[])
for rel,data in changes.items():
 q=out/'payload'/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(data);plan['files'].append(dict(path=rel,before=before[rel],after=h(data)))
(out/'plan.json').write_text(json.dumps(plan,indent=2)+'\n',encoding='utf-8');(p/'MAKEFILE_PROPAGATION_AUDIT.json').write_text(json.dumps(dict(before_sha256=before['reconstruction/tests/Makefile'],actualmanager_occurrences=13,paired_actualNPC_occurrences=13,rows=rows,policy='Everyfullmanager source-list/recipe/prerequisite occurrence receivesactualNPCsource, preservingexisting sharedmacros. Noapparchive/testconditional/dummy dtor.'),indent=2)+'\n',encoding='utf-8');(p/'apply.py').write_text(Path('work/pc_state_consumers/apply.py').read_text(encoding='utf-8-sig'),encoding='utf-8')
s=Path('work/pc_state_consumers/validate.py').read_text(encoding='utf-8-sig').replace('pc_state_consumers','quest_ownership').replace('pc-state-consumers','quest-ownership');s=s.replace("'../../game/src/questpc.cpp'])","'../../game/src/questmanager.cpp','../../game/src/questnpc.cpp'])");s=s.replace("for required in ['../../game/src/event.cpp'","assert cmd.count('../../game/src/questnpc.cpp')==1\nfor required in ['../../game/src/questnpc.cpp','../../game/src/event.cpp'");ast.parse(s);(p/'validate.py').write_text(s,encoding='utf-8');print('Prepared114/8paths67guards; NPCpaired13literalcontexts source/O2+deps; oldfixturecandidate onlyactualmethodsremoved. NoFwrites/builds.')
