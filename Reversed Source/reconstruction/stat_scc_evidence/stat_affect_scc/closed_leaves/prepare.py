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
def get(rel):
 raw=(root/rel).read_bytes();before[rel]=hashlib.sha256(raw).hexdigest();return raw.decode('utf-8-sig')
def add_header(rel,fragment,absent):
 s=get(rel);assert all(x not in s for x in absent);at=s.rfind('#endif');assert at>=0;changes[rel]=(s[:at]+'\n'+(p/fragment).read_text()+'\n'+s[at:]).encode()
add_header('common/tables.h','tables.h.additions',['SPacketGDRemoveAffect','TPacketGDRemoveAffect'])
add_header('game/src/packet.h','packet.h.additions',['TPacketGCCreateFly','TPacketGCAffectRemove','struct packet_fly '])
rel='game/src/char.h';s=get(rel);assert 'IsAffectFlag(' not in s and 'CreateFly(' not in s;pos=s.index('public:',s.index('class CHARACTER :'));pos+=len('public:');changes[rel]=(s[:pos]+'\n'+(p/'char.h.additions').read_text()+s[pos:]).encode()
for rel,fragment,absent in [('game/src/char_affect.cpp','char_affect.cpp.additions',['SendAffectRemovePacket(','CHARACTER::IsAffectFlag(']),('game/src/char_battle.cpp','char_battle.cpp.additions',['CHARACTER::CreateFly('])]:
 s=get(rel);assert all(x not in s for x in absent);changes[rel]=(s+'\n'+(p/fragment).read_text()).encode()
for name in ['affect_packet_leaves_regression.cpp','affect_packet_leaves_abi.cpp']:
 rel='reconstruction/tests/'+name;assert not (root/rel).exists();before[rel]=None;changes[rel]=(p/name).read_bytes()
rel='reconstruction/tests/Makefile';s=get(rel);assert 'affect_packet_leaves_regression' not in s;assert s.count('RUNTIME_TESTS = ')==1;s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = affect_packet_leaves_regression ',1);changes[rel]=(s+'\n'+(p/'Makefile.addition').read_text()).encode()
plan={'root':str(root),'scope':'Complete independently closed original3methods only: IsAffectFlag, SendAffectRemovePacket, CreateFly; full original packed6/9/10byte records; actual descriptor/CHAR/DB/view integration. No stat/affect mutation or lifecycle substitutes.','files':[]}
for rel,data in changes.items():
 q=out/'payload'/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(data);plan['files'].append({'path':rel,'before':before[rel],'after':hashlib.sha256(data).hexdigest()})
out.joinpath('plan.json').write_text(json.dumps(plan,indent=2)+'\n');print(out/'plan.json')
