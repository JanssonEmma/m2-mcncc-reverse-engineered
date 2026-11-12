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
# Workspace renderer only; no F writes/builds. Parent re-renders after review at next allowance window.
from pathlib import Path
import argparse,json,hashlib,re
p=Path(__file__).resolve().parent;base=p.parent;a=argparse.ArgumentParser();a.add_argument('--root',default=_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));v=a.parse_args();root=Path(v.root);out=p/'integration';changes={};before={}
def get(rel):
 raw=(root/rel).read_bytes();before[rel]=hashlib.sha256(raw).hexdigest();return raw.decode('utf-8-sig')
rel='game/src/questpc.cpp';s=get(rel)
for n in ['SetCurrentQuestStartFlag','SetCurrentQuestDoneFlag','ClearCurrentQuestBeginFlag','SetCurrentQuestBeginFlag','GetCurrentQuestBeginFlag','SetCurrentQuestTitle','SetQuestTitle','SetCurrentQuestClockName','SetCurrentQuestClockValue','SetCurrentQuestCounterName','SetCurrentQuestCounterValue','SetCurrentQuestIconFile','SendQuestInfoPakcet']:assert 'PC::'+n+'(' not in s,'Refuse existing '+n
changes[rel]=(s+'\n'+(base/'questpc.cpp.candidate').read_text(encoding='utf-8-sig')).encode('utf-8')
rel='game/src/questmanager.h';s=get(rel);assert 'GetCurrentCharacterPtr(' not in s;assert 'GetQuestIndexByName(' in s;at=s.index('public:',s.index('class CQuestManager'))+len('public:');changes[rel]=(s[:at]+'\n'+(base/'questmanager.h.additions.candidate').read_text(encoding='utf-8-sig')+s[at:]).encode('utf-8')
rel='game/src/packet.h';s=get(rel);assert 'packet_quest_info' not in s;at=s.rfind('#endif');assert at>=0;changes[rel]=(s[:at]+'\n'+(base/'packet.h.additions.candidate').read_text(encoding='utf-8-sig')+'\n'+s[at:]).encode('utf-8')
for name in ['pc_quest_info_regression.cpp','pc_quest_info_abi.cpp','pc_quest_info_desc_output_syscalls.c']:
 rel='reconstruction/tests/'+name;assert not (root/rel).exists();before[rel]=None;changes[rel]=(p/name).read_bytes()
rel='reconstruction/tests/Makefile';s=get(rel);assert 'pc_quest_info_regression' not in s;s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = pc_quest_info_regression ',1);lines=s.splitlines(keepends=True);at=next(i for i,l in enumerate(lines) if l.startswith('check:'));lines.insert(at+1,'\t./pc_quest_info_regression$(EXEEXT)\n');s=''.join(lines);ls=s.splitlines();at=next(i for i,l in enumerate(ls) if l.startswith('check:'));executions=[]
for l in ls[at+1:]:
 if not l.startswith('\t'):break
 m=re.search(r'\./([A-Za-z0-9_]+)\$\(EXEEXT\)',l)
 if m:executions.append(m.group(1))
registry=re.search(r'^RUNTIME_TESTS = (.*)$',s,re.M).group(1).split();assert executions==registry and executions.count('pc_quest_info_regression')==1
changes[rel]=(s+'\n'+(p/'Makefile.addition').read_text(encoding='utf-8-sig')).encode('utf-8');plan={'root':str(root),'scope':'13complete original PCinfo bodies, full6bytepacket, original code-backed contextgetter only. NO inventedenum/ClearSendFlag, no fake methods. Prepared/uncompiled safe-domain actualpool/network tests; no passclaim.','runtime_count_after':len(registry),'dependencies':[{'path':rel,'sha256':hashlib.sha256((root/rel).read_bytes()).hexdigest()} for rel in ['game/src/questmanager.cpp','game/src/questpc.h','game/src/buffer_manager.cpp','game/src/buffer_manager.h']],'files':[]}
for rel,data in changes.items():
 q=out/'payload'/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(data);plan['files'].append({'path':rel,'before':before[rel],'after':hashlib.sha256(data).hexdigest()})
out.joinpath('plan.json').write_text(json.dumps(plan,indent=2)+'\n',encoding='utf-8');print(out/'plan.json',len(registry),'canonical runtimes; rendering only')
