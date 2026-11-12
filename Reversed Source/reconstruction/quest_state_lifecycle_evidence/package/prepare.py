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
# Workspace-only complete lifecycle/script package. No application/build.
from pathlib import Path
import json,hashlib,re,ast
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));out=p/'integration';changes={};before={};h=lambda b:hashlib.sha256(b).hexdigest()
def get(rel):
 raw=(root/rel).read_bytes();before[rel]=h(raw);return raw.decode('utf-8-sig')
rel='game/src/questmanager.cpp';s=get(rel)
for name in ['OpenState','CloseState','ClearScript','AddScript','SendScript','GotoEndState']:assert 'CQuestManager::'+name+'(' not in s
changes[rel]=('#include "char.h"\n#include "desc.h"\n#include "buffer_manager.h"\n#include "packet.h"\n'+s+'\n'+(p/'questmanager.cpp.fragment').read_text(encoding='utf-8-sig')).encode('utf-8')
rel='game/src/questlua.cpp';assert not(root/rel).exists();before[rel]=None;changes[rel]=(p/'questlua.cpp.candidate').read_bytes()
rel='game/src/questmanager.h';s=get(rel);assert 'OpenState(' not in s;at=s.index('public:',s.index('class CQuestManager'))+len('public:');s=s[:at]+'\n'+(p/'questmanager.h.public.fragment').read_text(encoding='utf-8-sig')+s[at:];at=s.index('private:',s.index('class CQuestManager'))+len('private:');s=s[:at]+'\n'+(p/'questmanager.h.private.fragment').read_text(encoding='utf-8-sig')+s[at:];changes[rel]=s.encode('utf-8')
rel='game/src/packet.h';s=get(rel);assert 'struct packet_script ' not in s;at=s.rfind('#endif');assert at>=0;changes[rel]=(s[:at]+'\n'+(p/'packet.h.fragment').read_text(encoding='utf-8-sig')+'\n'+s[at:]).encode('utf-8')
for name in ['quest_state_lifecycle_regression.cpp','quest_state_lifecycle_abi.cpp','quest_state_lifecycle_desc_output_syscalls.c']:
 rel='reconstruction/tests/'+name;assert not(root/rel).exists();before[rel]=None;changes[rel]=(p/name).read_bytes()
rel='reconstruction/tests/Makefile';s=get(rel);assert 'quest_state_lifecycle_regression' not in s;registry=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split();assert len(registry)==109,'Expected applied97 source109baseline'
s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = quest_state_lifecycle_regression ',1);ls=s.splitlines(keepends=True);at=next(i for i,l in enumerate(ls)if l.startswith('check:'));ls.insert(at+1,'\t./quest_state_lifecycle_regression$(EXEEXT)\n');s=''.join(ls);registry=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split();ls=s.splitlines();at=next(i for i,l in enumerate(ls)if l.startswith('check:'));executions=[]
for l in ls[at+1:]:
 if not l.startswith('\t'):break
 a=re.search(r'\./([A-Za-z0-9_]+)\$\(EXEEXT\)',l)
 if a:executions.append(a.group(1))
assert executions==registry and len(registry)==110 and registry.count('quest_state_lifecycle_regression')==1;changes[rel]=(s+'\n'+(p/'Makefile.addition').read_text(encoding='utf-8-sig')).encode('utf-8')
sdk=json.loads(Path('work/pc_state_consumers/MANIFEST.json').read_text(encoding='utf-8'))['sdk_expected_files']
for row in sdk:assert h((root/row['path']).read_bytes())==row['sha256']
deps=[{'path':rel,'sha256':h((root/rel).read_bytes())}for rel in ['game/src/quest.h','game/src/questpc.h','game/src/questpc.cpp','game/src/buffer_manager.h','game/src/buffer_manager.cpp','game/src/char.h','game/src/desc.h']]+sdk
plan={'root':str(root),'scope':'Whole6originalCQMmethods; correctquestlua.cppOpen/Close/GotoEnd andquestmanager.cppClear/Add/Send; complete6bytepacket. Open staticonlyNRVO/counter-copylimitation. RealSDK/PC/network, nofakeVM/startup. Prepared/uncompiled.','runtime_count_after':110,'dependencies':deps,'files':[]}
for rel,data in changes.items():
 q=out/'payload'/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(data);plan['files'].append({'path':rel,'before':before[rel],'after':h(data)})
(out/'plan.json').write_text(json.dumps(plan,indent=2)+'\n',encoding='utf-8')
(p/'apply.py').write_text(Path('work/pc_state_consumers/apply.py').read_text(encoding='utf-8-sig'),encoding='utf-8')
s=Path('work/pc_state_consumers/validate.py').read_text(encoding='utf-8-sig').replace('pc_state_consumers','quest_state_lifecycle').replace('pc-state-consumers','quest-state-lifecycle');s=s.replace("'../../game/src/questpc.cpp'])","'../../game/src/questmanager.cpp','../../game/src/questlua.cpp'])");s=s.replace("'../../game/src/event.cpp','../../game/src/event_queue.cpp'","'../../game/src/questlua.cpp','../../game/src/event.cpp','../../game/src/event_queue.cpp'");(p/'validate.py').write_text(s,encoding='utf-8')
for q in p.glob('*.py'):ast.parse(q.read_text(encoding='utf-8-sig'),filename=str(q))
addresses=[('OpenState','0805cd80',28,3),('CloseState','0805d000',22,1),('GotoEndState','0805cd10',34,4),('ClearScript','081a4ee0',21,1),('AddScript','081a4f30',7,1),('SendScript','081a4f50',122,15)]
m={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','status':'prepared_only_uncompiled_unapplied','originals':[dict(name='quest::CQuestManager::'+n,address=a,instructions=i,calls=c)for n,a,i,c in addresses],'owner_authority':'ELF_OWNER_AUDIT.json rawmangledsymbols/artificialthis/exactoriginalCUs override addressneighborhood/freefunctionheuristics','runtime_count_after':110,'files':plan['files'],'dependencies':deps,'sdk_expected_files':sdk,'plan_sha256':h((out/'plan.json').read_bytes()),'runtime_limitations':['OpenState STATICONLY namedreturn maycopy indeterminatecounters; originalNRVO directreturnstorage proven','Unconditionalsendemptystring nonconstoperator[](size) unsafeoriginaldomain excluded','Lua5.0.3 suppliedexternalSDK not exactold5.0 internallayout/VM reconstruction'],'static_checks':['PythonAST','110orderedcanonicalexecution/registryexactonce'],'project_mutation_or_build':False,'package_files':{q.name:h(q.read_bytes())for q in p.iterdir()if q.is_file()and q.name!='MANIFEST.json'}}
(p/'MANIFEST.json').write_text(json.dumps(m,indent=2)+'\n',encoding='utf-8');print('Prepared110 /8paths /'+str(len(deps))+'dependencies; no Fwrites/builds')
