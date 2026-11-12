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
import json,hashlib
from pathlib import Path
p=Path('work/item_canary_audit');root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
target=root/'reconstruction/tests/item_regression.cpp';old=target.read_bytes();s=old.decode('utf-8')
seed='    std::memset(storage.bytes,0xa5,sizeof(storage.bytes));'
read='    for(size_t i=first;i<=last;++i) assert(storage.bytes[i]==0xa5);'
assert s.count(seed)==1 and s.count(read)==1
s=s.replace(seed,'    // Observable raw-byte seed survives placement-new lifetime DSE under O2/LTO.\n    volatile unsigned char* const raw=storage.bytes;\n    for(size_t i=0;i<sizeof(storage.bytes);++i) raw[i]=0xa5;')
s=s.replace(read,'    const volatile unsigned char* const observed=reinterpret_cast<const volatile unsigned char*>(item);\n    for(size_t i=first;i<=last;++i) assert(observed[i]==0xa5);')
data=s.encode('utf-8');base=p/'parent_apply';dest=base/'payload/reconstruction/tests/item_regression.cpp'
dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(data)
plan={'root':str(root),'changes':[{'target':'reconstruction/tests/item_regression.cpp','expected_sha256':hashlib.sha256(old).hexdigest(),'desired_sha256':hashlib.sha256(data).hexdigest(),'payload':'payload/reconstruction/tests/item_regression.cpp'}],'note':'TEST ONLY volatile unsigned-byte seed/read. Original first..last inclusive and exact0xa5 comparison preserved. No production/layout/recipe changes.'}
(base/'plan.json').write_text(json.dumps(plan,indent=2))
print({'files':1,'baseline':plan['changes'][0]['expected_sha256'],'range':'original first..last inclusive unchanged'})
