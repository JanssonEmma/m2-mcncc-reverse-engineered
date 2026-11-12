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
# Workspace-only fullCHARconfirmation/CQMstyle package. No Fmutation/build.
from pathlib import Path
import json,hashlib,re,ast
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));out=p/'integration';changes={};before={};h=lambda b:hashlib.sha256(b).hexdigest()
def get(rel):
 b=(root/rel).read_bytes();before[rel]=h(b);return b.decode('utf-8-sig')
rel='game/src/char.cpp';s=get(rel);assert 'CHARACTER::ConfirmWithMsg(' not in s;changes[rel]=(s+'\n'+(p/'char.cpp.fragment').read_text(encoding='utf-8-sig')).encode('utf-8')
rel='game/src/questmanager.cpp';s=get(rel);assert 'CQuestManager::SetSkinStyle(' not in s;changes[rel]=(s+'\n'+(p/'questmanager.cpp.fragment').read_text(encoding='utf-8-sig')).encode('utf-8')
rel='game/src/char.h';s=get(rel);assert 'ConfirmWithMsg(' not in s;start=re.search(r'class CHARACTER\s*:',s).start();at=s.index('public:',start)+len('public:');s=s[:at]+'\n'+(p/'char.h.fragment').read_text(encoding='utf-8-sig')+s[at:];changes[rel]=s.encode('utf-8')
rel='game/src/questmanager.h';s=get(rel);assert 'SetSkinStyle(' not in s;at=s.index('public:',s.index('class CQuestManager'))+len('public:');s=s[:at]+'\n'+(p/'questmanager.h.fragment').read_text(encoding='utf-8-sig')+s[at:];changes[rel]=s.encode('utf-8')
rel='game/src/packet.h';s=get(rel);assert 'packet_quest_confirm' not in s;at=s.rfind('#endif');assert at>=0;changes[rel]=(s[:at]+'\n'+(p/'packet.h.fragment').read_text(encoding='utf-8-sig')+'\n'+s[at:]).encode('utf-8')
for name in ['quest_confirm_leaves_regression.cpp','quest_confirm_leaves_abi.cpp','quest_confirm_leaves_desc_output_syscalls.c']:
 rel='reconstruction/tests/'+name;assert not(root/rel).exists();before[rel]=None;changes[rel]=(p/name).read_bytes()
rel='reconstruction/tests/Makefile';s=get(rel);assert 'quest_confirm_leaves_regression' not in s;registry=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split();assert len(registry)==112,'Requiresapplied112baseline'
s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = quest_confirm_leaves_regression ',1);ls=s.splitlines(keepends=True);at=next(i for i,l in enumerate(ls)if l.startswith('check:'));ls.insert(at+1,'\t./quest_confirm_leaves_regression$(EXEEXT)\n');s=''.join(ls);registry=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split();ls=s.splitlines();at=next(i for i,l in enumerate(ls)if l.startswith('check:'));executions=[]
for l in ls[at+1:]:
 if not l.startswith('\t'):break
 a=re.search(r'\./([A-Za-z0-9_]+)\$\(EXEEXT\)',l)
 if a:executions.append(a.group(1))
assert executions==registry and len(registry)==113 and registry.count('quest_confirm_leaves_regression')==1;changes[rel]=(s+'\n'+(p/'Makefile.addition').read_text(encoding='utf-8-sig')).encode('utf-8')
sdk=json.loads(Path('work/quest_yield_bridges/MANIFEST.json').read_text(encoding='utf-8'))['sdk_expected_files']
for row in sdk:assert h((root/row['path']).read_bytes())==row['sha256']
deps=[{'path':r,'sha256':h((root/r).read_bytes())}for r in ['game/src/questlua.cpp','game/src/questpc.cpp','game/src/questpc.h','game/src/entity.h','game/src/entity.cpp','game/src/desc.h','game/src/desc.cpp','game/src/buffer_manager.h','game/src/buffer_manager.cpp','libthecore/include/typedef.h']]+sdk
plan={'root':str(root),'scope':'Whole CHARConfirmWithMsg08136310+wholeCQMSetSkinStyle081a5400/full74byteoldpacket. ActualDESC/script/CQM graph, no partialConfirm/RunState. Prepareduncompiled.','runtime_count_after':113,'dependencies':deps,'files':[]}
for rel,data in changes.items():
 q=out/'payload'/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(data);plan['files'].append({'path':rel,'before':before[rel],'after':h(data)})
(out/'plan.json').write_text(json.dumps(plan,indent=2)+'\n',encoding='utf-8');(p/'apply.py').write_text(Path('work/quest_yield_bridges/apply.py').read_text(encoding='utf-8-sig'),encoding='utf-8')
s=Path('work/quest_yield_bridges/validate.py').read_text(encoding='utf-8-sig').replace('quest_yield_bridges','quest_confirm_leaves').replace('quest-yield-bridges','quest-confirm-leaves');s=s.replace("'../../game/src/questmanager.cpp','../../game/src/questlua.cpp'])","'../../game/src/char.cpp','../../game/src/questmanager.cpp','../../game/src/questlua.cpp'])");s=s.replace("run('strict32',[v.cxx,","run('strict32',[v.cxx,'-Isql_shim','-I../dependencies/mysql-4.1.3/include','-include','sql_socket_platform.h',");(p/'validate.py').write_text(s,encoding='utf-8')
for q in p.glob('*.py'):ast.parse(q.read_text(encoding='utf-8-sig'),filename=str(q))
m={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','status':'preparedonly_uncompiled_unapplied','originals':[dict(name='CHARACTER::ConfirmWithMsg',address='08136310',instructions=35,calls=2),dict(name='quest::CQuestManager::SetSkinStyle',address='081a5400',instructions=12,calls=0)],'record':dict(name='packet_quest_confirm',alias='TPacketGCQuestConfirm',size=74,fields=[dict(name='header',offset=0,type='BYTE'),dict(name='msg',offset=1,type='char[65]'),dict(name='timeout',offset=66,type='long32signed'),dict(name='requestPID',offset=70,type='DWORD=unsignedlong32')]),'runtime_count_after':113,'files':plan['files'],'dependencies':deps,'sdk_expected_files':sdk,'plan_sha256':h((out/'plan.json').read_bytes()),'frontier_only':['GotoConfirmState0805c720→confirm_timeout_event0805c650→CQMConfirm081a1ff0→RunState0805cde0→PC::EndRunning0805f4d0 SCC','CQMGetCurrentNPCCharacterPtr081a5700/GetCurrentNPCRace081a5660→CHARGetQuestNPC08134f50 not integratedhere'],'static_checks':['PythonAST','113orderedcanonicalregistry/executionexactonce'],'project_mutation_or_build':False,'package_files':{q.name:h(q.read_bytes())for q in p.iterdir()if q.is_file()and q.name!='MANIFEST.json'}};(p/'MANIFEST.json').write_text(json.dumps(m,indent=2)+'\n',encoding='utf-8');print('Prepared113 /9paths/'+str(len(deps))+'guards; no Fwrites/builds')
