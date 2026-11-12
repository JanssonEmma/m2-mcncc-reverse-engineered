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
import argparse,json,hashlib
p=Path(__file__).resolve().parent;a=argparse.ArgumentParser();a.add_argument('--root',default=_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));v=a.parse_args();root=Path(v.root);out=p/'integration';changes={};before={}
def get(rel):
 raw=(root/rel).read_bytes();before[rel]=hashlib.sha256(raw).hexdigest();return raw.decode('utf-8-sig')
for rel,name in [('game/src/safebox.h','safebox.h.candidate'),('game/src/safebox.cpp','safebox.cpp.candidate'),('reconstruction/tests/safebox_closed_regression.cpp','safebox_closed_regression.cpp'),('reconstruction/tests/safebox_closed_abi.cpp','safebox_closed_abi.cpp'),('reconstruction/tests/safebox_desc_output_syscalls.c','safebox_desc_output_syscalls.c')]:
 assert not (root/rel).exists(),'Refuse overwrite '+rel;before[rel]=None;changes[rel]=(p/name).read_bytes()
rel='common/length.h';s=get(rel);assert 'enum EWindows' not in s;at=s.rfind('#endif');assert at>=0;changes[rel]=(s[:at]+'\n'+(p/'length.h.additions').read_text(encoding='utf-8')+'\n'+s[at:]).encode()
rel='common/tables.h';s=get(rel);assert 'SSafeboxTable' not in s and 'TSafeboxTable' not in s;at=s.rfind('#endif');assert at>=0;changes[rel]=(s[:at]+'\n'+(p/'tables.h.additions').read_text(encoding='utf-8')+'\n'+s[at:]).encode()
rel='game/src/char.h';s=get(rel);assert 'GetSafebox(' not in s and 'GetMall(' not in s;at=s.index('public:',s.index('class CHARACTER :'))+len('public:');changes[rel]=(s[:at]+'\n'+(p/'char.h.additions').read_text(encoding='utf-8')+s[at:]).encode()
rel='game/src/char.cpp';s=get(rel);assert 'CHARACTER::GetSafebox(' not in s and 'CHARACTER::GetMall(' not in s;changes[rel]=(s+'\n'+(p/'char.cpp.additions').read_text(encoding='utf-8')).encode()
rel='reconstruction/tests/Makefile';s=get(rel);assert 'safebox_closed_regression' not in s and s.count('RUNTIME_TESTS = ')==1;s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = safebox_closed_regression ',1);anchor='\t./quest_pc_lookup_regression$(EXEEXT)';assert s.count(anchor)==1;s=s.replace(anchor,'\t./safebox_closed_regression$(EXEEXT)\n'+anchor,1);changes[rel]=(s+'\n'+(p/'Makefile.addition').read_text(encoding='utf-8')).encode()
plan={'root':str(root),'scope':'Complete old135-slot CSafebox data/API with independently closed actual constructor/Add/Save/lookup/window/resize + CHARACTER pointer getters only. NO fake destructor/Remove/MoveItem/items. TESTONLY raw-storage lifetime boundary explicitly omits unclosed populated destructor and manually reclaims empty storage/grids.','files':[]}
for rel,data in changes.items():
 q=out/'payload'/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(data);plan['files'].append({'path':rel,'before':before[rel],'after':hashlib.sha256(data).hexdigest()})
out.joinpath('plan.json').write_text(json.dumps(plan,indent=2)+'\n');print(out/'plan.json')
