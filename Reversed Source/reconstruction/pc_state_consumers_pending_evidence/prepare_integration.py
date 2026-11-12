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
# PARENT ONLY workspace renderer after verified108. Never applies or builds.
from pathlib import Path
import json,hashlib,re
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));m=json.loads((p/'MANIFEST.json').read_text(encoding='utf-8'));h=lambda b:hashlib.sha256(b).hexdigest();out=p/'integration';files=[];changes={};before={}
for row in m['current_read_guards']:assert h((root/row['path']).read_bytes())==row['sha256'],'STALE readguard '+row['path']
for row in m['files']:
 assert h((root/row['path']).read_bytes())==row['before'],'STALE source '+row['path'];data=(p/row['candidate']).read_bytes();assert h(data)==row['after'];changes[row['path']]=data;before[row['path']]=row['before']
for row in m['sdk_expected_files']:assert h((root/row['path']).read_bytes())==row['sha256'],'SDK missing/drift '+row['path']
header=(root/'game/src/questmanager.h').read_text(encoding='utf-8');assert 'GetQuestStateName(' in header and 'GetQuestStateIndex(' in header
for name in ['pc_state_consumers_regression.cpp','pc_state_consumers_abi.cpp','pc_state_consumers_desc_output_syscalls.c']:
 rel='reconstruction/tests/'+name;assert not(root/rel).exists();changes[rel]=(p/name).read_bytes();before[rel]=None
rel='reconstruction/tests/Makefile';raw=(root/rel).read_bytes();before[rel]=h(raw);s=raw.decode('utf-8-sig');assert 'pc_state_consumers_regression' not in s;assert 'LUA_SDK_LIBRARIES' in s
old=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split();assert len(old)==108,'REQUIRES verified108baseline, not group106/107'
s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = pc_state_consumers_regression ',1);ls=s.splitlines(keepends=True);at=next(i for i,l in enumerate(ls)if l.startswith('check:'));ls.insert(at+1,'\t./pc_state_consumers_regression$(EXEEXT)\n');s=''.join(ls);registry=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split();ls=s.splitlines();at=next(i for i,l in enumerate(ls)if l.startswith('check:'));executions=[]
for l in ls[at+1:]:
 if not l.startswith('\t'):break
 a=re.search(r'\./([A-Za-z0-9_]+)\$\(EXEEXT\)',l)
 if a:executions.append(a.group(1))
assert executions==registry and len(registry)==109 and registry.count('pc_state_consumers_regression')==1;changes[rel]=(s+'\n'+(p/'Makefile.addition').read_text(encoding='utf-8-sig')).encode('utf-8')
plan={'root':str(root),'scope':'Whole3PC state consumers using actualLua-backed queries/network. Awaitindependentreview/compile, no fakeVM/defaultstatecopy.','runtime_count_after':109,'dependencies':m['current_read_guards']+[{'path':rel,'sha256':h((root/rel).read_bytes())}for rel in ['game/src/questmanager.cpp','game/src/questmanager.h']]+m['sdk_expected_files'],'files':[]}
for rel,data in changes.items():
 q=out/'payload'/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(data);plan['files'].append({'path':rel,'before':before[rel],'after':h(data)})
(out/'plan.json').write_text(json.dumps(plan,indent=2)+'\n',encoding='utf-8');print('Prepared workspace109 fivepaths; no Fwrites/builds')
