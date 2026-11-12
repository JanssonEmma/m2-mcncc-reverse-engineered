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
import argparse,json,hashlib,re
p=Path(__file__).resolve().parent;a=argparse.ArgumentParser();a.add_argument('--root',default=_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));v=a.parse_args();root=Path(v.root);out=p/'integration';changes={};before={}
qh=(root/'game/src/questmanager.h').read_text(encoding='utf-8-sig');qc=(root/'game/src/questmanager.cpp').read_text(encoding='utf-8-sig');assert re.search(r'unsigned\s+int\s+GetQuestIndexByName\s*\(\s*const\s+std::string\s*&',qh),'Apply real manager query package first';assert 'CQuestManager::GetQuestIndexByName(' in qc,'Real query BODY missing'
def get(rel):
 raw=(root/rel).read_bytes();before[rel]=hashlib.sha256(raw).hexdigest();return raw.decode('utf-8-sig')
rel='game/src/questpc.h';s=get(rel);bodies=json.loads((p/'questpc.h.inline.json').read_text(encoding='utf-8-sig'))
for old,key in [('QuestInfoIterator quest_begin();','quest_begin'),('QuestInfoIterator quest_end();','quest_end'),('QuestInfoIterator quest_find(DWORD);','quest_find'),('TQuestStateChangeInfo(DWORD,int,int);','change_ctor')]:
 assert s.count(old)==1,old;s=s.replace(old,bodies[key],1)
changes[rel]=s.encode('utf-8')
rel='game/src/questpc.cpp';s=get(rel)
for name in ['HasQuest','GetQuest','GetCurrentQuestIndex','SetQuest','SetQuestState','AddQuestStateChange','ClearQuest']:assert 'PC::'+name+'(' not in s,'Refuse existing '+name
changes[rel]=(s+'\n'+(p/'questpc.cpp.fragment').read_text(encoding='utf-8-sig')).encode('utf-8')
for name in ['pc_quest_storage_regression.cpp','pc_quest_storage_abi.cpp','pc_quest_storage_desc_output_syscalls.c']:
 rel='reconstruction/tests/'+name;assert not (root/rel).exists(),'Refuse existing newfile '+rel;before[rel]=None;changes[rel]=(p/name).read_bytes()
rel='reconstruction/tests/Makefile';s=get(rel);assert 'pc_quest_storage_regression' not in s and s.count('RUNTIME_TESTS = ')==1;s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = pc_quest_storage_regression ',1)
anchor='\t./quest_manager_indices_regression$(EXEEXT)';assert s.count(anchor)==1
s=s.replace(anchor,'\t./pc_quest_storage_regression$(EXEEXT)\n'+anchor,1)
lines=s.splitlines();start=next(i for i,l in enumerate(lines) if l.startswith('check:'));commands=[]
for line in lines[start+1:]:
 if not line.startswith('\t'):break
 match=re.search(r'\./([A-Za-z0-9_]+)\$\(EXEEXT\)',line)
 if match:commands.append(match.group(1))
registry=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split()
assert commands==registry,('Canonical execution/registry mismatch',commands,registry)
assert commands.count('pc_quest_storage_regression')==1
changes[rel]=(s+'\n'+(p/'Makefile.addition').read_text(encoding='utf-8-sig')).encode('utf-8')
plan={'root':str(root),'scope':'Full7PC quest-storage originals +4original inline iterator/record ctor bodies. Requires actual manager query; real PC/QuestState/flag/event lifecycle, no Lua query or AArgScript constructor substitute. ClearQuest dangling-live-key nonzero path preserved, excluded runtime domain documented.','dependencies':[{'path':rel,'sha256':hashlib.sha256((root/rel).read_bytes()).hexdigest()} for rel in ['game/src/questmanager.h','game/src/questmanager.cpp']],'files':[]}
for rel,data in changes.items():
 q=out/'payload'/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(data);plan['files'].append({'path':rel,'before':before[rel],'after':hashlib.sha256(data).hexdigest()})
out.joinpath('plan.json').write_text(json.dumps(plan,indent=2)+'\n',encoding='utf-8');print(out/'plan.json')
