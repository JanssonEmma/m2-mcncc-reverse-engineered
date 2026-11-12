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
# Render workspace-only integration from reviewed complete Save/record candidate, no project writes/builds.
from pathlib import Path
import json,hashlib,re
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));manifest=json.loads((p/'MANIFEST.json').read_text(encoding='utf-8'));out=p/'integration';changes={};before={};h=lambda b:hashlib.sha256(b).hexdigest()
for row in manifest['files']:
 raw=(root/row['path']).read_bytes();assert h(raw)==row['before'],'Stale reviewed source '+row['path'];data=(p/row['candidate']).read_bytes();assert h(data)==row['after'];changes[row['path']]=data;before[row['path']]=row['before']
for name in ['pc_quest_save_regression.cpp','pc_quest_save_abi.cpp','pc_quest_save_desc_output_syscalls.c']:
 rel='reconstruction/tests/'+name;assert not (root/rel).exists();before[rel]=None;changes[rel]=(p/name).read_bytes()
rel='reconstruction/tests/Makefile';raw=(root/rel).read_bytes();before[rel]=h(raw);s=raw.decode('utf-8-sig');assert 'pc_quest_save_regression' not in s;s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = pc_quest_save_regression ',1);lines=s.splitlines(keepends=True);at=next(i for i,l in enumerate(lines) if l.startswith('check:'));lines.insert(at+1,'\t./pc_quest_save_regression$(EXEEXT)\n');s=''.join(lines);registry=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split();ls=s.splitlines();at=next(i for i,l in enumerate(ls) if l.startswith('check:'));executions=[]
for l in ls[at+1:]:
 if not l.startswith('\t'):break
 m=re.search(r'\./([A-Za-z0-9_]+)\$\(EXEEXT\)',l)
 if m:executions.append(m.group(1))
assert executions==registry and len(registry)==104 and registry.count('pc_quest_save_regression')==1
changes[rel]=(s+'\n'+(p/'Makefile.addition').read_text(encoding='utf-8-sig')).encode('utf-8')
plan={'root':str(root),'scope':'Complete PC::Save08060770, full106byte original record, actual DB descriptor/PC/buffer/event graph regression; prepared only, no build/pass claim','runtime_count_after':len(registry),'dependencies':manifest['dependencies'],'files':[]}
for rel,data in changes.items():
 q=out/'payload'/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(data);plan['files'].append({'path':rel,'before':before[rel],'after':h(data)})
(out/'plan.json').write_text(json.dumps(plan,indent=2)+'\n',encoding='utf-8')
(p/'apply.py').write_text(Path('work/pc_quest_info/next_window/apply.py').read_text(encoding='utf-8-sig'),encoding='utf-8')
s=Path('work/pc_quest_info/next_window/validate.py').read_text(encoding='utf-8-sig').replace('pc_quest_info','pc_quest_save').replace('pc-quest-info','pc-quest-save')
s=s.replace("run('strict32',","run('normal-build-runtime-abi',base+['-B','check-pc-quest-save','check-pc-quest-save-abi'])\nrun('strict32',")
s=s.replace("'../../game/src/buffer_manager.cpp'","'../../game/src/buffer_manager.cpp','../../game/src/desc_client.cpp','buffer_manager_core.o'")
(p/'validate.py').write_text(s,encoding='utf-8');print('Prepared104 ordered canonical runtimes /6 guarded paths; no project writes')
