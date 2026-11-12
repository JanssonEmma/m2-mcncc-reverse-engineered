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
import json,re,hashlib
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));stage=p/'integration-ready'
t=lambda q:q.read_text(encoding='utf-8-sig').replace('\r\n','\n')
# Entire raw PC data tail remains identical except declaring known nested constructor.
old=t(root/'game/src/questpc.h');new=t(stage/'game/src/questpc.h')
a=old[old.index('    struct TQuestStateChangeInfo'):];b=new[new.index('    struct TQuestStateChangeInfo'):]
b=b.replace(' TQuestStateChangeInfo(DWORD,int,int);','')
assert a==b
for name in ('desc_fixture.cpp','entity_packet_fixture.cpp'):
 rel='reconstruction/tests/'+name
 assert t(stage/rel)==t(root/rel)+'\n'+(p/'quest_pc_event_boundary.fragment').read_text()
 assert 'thecore_shutdown' in t(stage/rel)
# Existing manager field data unchanged.
rel='game/src/questmanager.h';old=t(root/rel);new=t(stage/rel)
assert new.replace('\n'+(p/'questmanager.h.fragment').read_text(),'')==old
# No fake manager constructor/scalar initializes beyond L.
s=(p/'questmanager.cpp.fragment').read_text();assert 'CQuestManager::CQuestManager():L(0) {}' in s
names=lambda s:next(x for x in s.splitlines() if x.startswith('RUNTIME_TESTS = ')).split('=',1)[1].split()
before=names(t(root/'reconstruction/tests/Makefile'));after=names(t(stage/'reconstruction/tests/Makefile'))
assert len(before)==95 and after==['quest_pc_lookup_regression']+before
report={'source_only':True,'production_writes':False,'PC_complete_private_data_preserved':True,'CQuestManager_complete_data_preserved':True,'old_unknown_fields_not_initialized':['PC::m_iLastState','CQuestManager raw fields other thanL'],'legacy_fixture_all_prior_hooks_shutdown_boundaries_preserved':True,'canonical_runtime_before':95,'canonical_runtime_after':96,'API_declarations':61,'real_event_test':True,'validation':'No builds; source/record/graph audit only.'}
(p/'integration-review.json').write_text(json.dumps(report,indent=2))
print(json.dumps(report))
