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
import hashlib,json
from pathlib import Path
p=Path(__file__).resolve().parent;r=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));base=p/'parent_apply';rel='reconstruction/tests/locale_transfer_regression.cpp';old=(r/rel).read_bytes();s=old.decode();nl='\r\n' if b'\r\n' in old else '\n'
a='#include <cstring>'+nl;assert s.count(a)==1;s=s.replace(a,a+'#include <cerrno>'+nl,1)
a='        const int status=static_cast<int>(_spawnl(_P_WAIT,executable,argv0.c_str(),"--missing",number,filename,static_cast<char*>(0)));'+nl+'        assert(status==1);'
b='''        errno=0;
        const int status=static_cast<int>(_spawnl(_P_WAIT,executable,argv0.c_str(),"--missing",number,filename,static_cast<char*>(0)));
        const int spawn_errno=errno;
        if(status!=1) {
            std::fprintf(stderr,"locale transfer spawn failure: status=%d errno=%d slot=%d executable=[%s] argv0=[%s] file=[%s]\\n",status,spawn_errno,slot,executable,argv0.c_str(),filename);
            std::fflush(stderr);
        }
        assert(status==1);'''.replace('\n',nl)
assert s.count(a)==1;s=s.replace(a,b,1);data=s.encode();dest=base/'payload'/rel;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(data);sha=lambda b:hashlib.sha256(b).hexdigest()
(base/'plan.json').write_text(json.dumps(dict(root=str(r),changes=[dict(target=rel,expected_sha256=sha(old),desired_sha256=sha(data),payload=str(dest.relative_to(base)))],note='TEST ONLY diagnostic before unchanged actual _spawnl status==1 assertion. No altered spawn/exit/application behavior or retry.'),indent=2))
print('Prepared one guarded diagnostic-only test edit.')
