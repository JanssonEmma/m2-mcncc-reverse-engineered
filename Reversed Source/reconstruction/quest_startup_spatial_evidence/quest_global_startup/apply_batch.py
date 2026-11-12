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
import hashlib,json,sys
r=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
w=Path('work/quest_global_startup')
assert json.loads((r/'reconstruction/validation-status.json').read_text())['checkpoint107_map_allow']['status']=='passed_focused'
rows=json.loads((w/'apply-plan.json').read_text())
sha=lambda b:hashlib.sha256(b).hexdigest()
files={row['path']:(w/'payload'/row['path']).read_bytes() for row in rows}
for row in rows:
    rel=row['path']
    assert sha((r/rel).read_bytes())==row['before'] and sha(files[rel])==row['after'],rel
sectree=Path('work/sectree_manager')
for name in ('sectree_manager.h','sectree_manager.cpp'):
    files['game/src/'+name]=(sectree/name).read_bytes()
files['reconstruction/tests/sectree_manager_regression.cpp']=(sectree/'sectree_manager_regression.cpp').read_bytes()
for rel in files.keys()-{row['path'] for row in rows}:
    assert not (r/rel).exists(),rel
rel='reconstruction/tests/Makefile'
make=(r/rel).read_text()
assert make.count('RUNTIME_TESTS = ')==1 and make.count('\t./quest_npc_set_regression$(EXEEXT)')==1
make=make.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = sectree_manager_regression ',1)
make=make.replace('\t./quest_npc_set_regression$(EXEEXT)','\t./sectree_manager_regression$(EXEEXT)\n\t./quest_npc_set_regression$(EXEEXT)',1)
make+='''
SECTREE_MANAGER_REGRESSION = sectree_manager_regression.cpp ../../game/src/sectree_manager.cpp

sectree_manager_regression$(EXEEXT): $(SECTREE_MANAGER_REGRESSION) ../../game/src/sectree_manager.h ../../game/src/sectree.h
	$(CXX) -I../../game/src $(CPPFLAGS) $(CXXFLAGS) $(SECTREE_MANAGER_REGRESSION) $(LDFLAGS) -o $@

.PHONY: check-sectree-manager-abi clean-sectree-manager
check-abi: check-sectree-manager-abi
check-sectree-manager-abi:
	$(CXX) -m32 -std=c++03 -pedantic-errors -fsyntax-only -I../../game/src $(SECTREE_MANAGER_REGRESSION)

clean: clean-sectree-manager
clean-sectree-manager:
	$(RM) sectree_manager_regression$(EXEEXT)
'''
files[rel]=make.encode()
applied=[]
for rel,payload in files.items():
    dest=r/rel
    before=dest.read_bytes() if dest.exists() else None
    if before is not None:
        backup=w/'before'/rel
        backup.parent.mkdir(parents=True,exist_ok=True)
        backup.write_bytes(before)
    applied.append(dict(path=rel,before=sha(before) if before is not None else None,after=sha(payload)))
    dest.write_bytes(payload)
(w/'applied.json').write_text(json.dumps(applied,indent=2)+'\n')
print('Applied direct callbacks and sector manager to',len(applied),'paths.')
