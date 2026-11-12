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
import json,re,hashlib
from pathlib import Path
p=Path('work/stat_primitives')
test=(p/'stat_primitives_regression.cpp').read_text()
fixture=(p/'stat_primitives_desc_output_syscalls.c').read_text()
recipe=(p/'Makefile.stat-primitives.inc').read_text()
assert not re.search(r'(?:BYTE|bool|int)\s+CHARACTER::(?:GetJob|IsDead|IsStun)\s*\(',test)
assert '#include "desc_output_syscalls.c"' in fixture and 'abort();' in fixture
assert 'DESC_PACKET_SOURCES' in recipe and 'party_lifecycle_fixture.cpp' not in recipe
assert '$(filter-out $(DESC_PACKET_SOURCES),../../game/src/char_battle.cpp)' in recipe
golden=re.search(r'golden\[17\]=\{([^}]+)\}',test,re.S).group(1)
values=bytes(int(x.strip(),0) for x in golden.split(','))
old=json.loads((p/'evidence-summary.json').read_text())['expected_static_layout_vectors']
assert values.hex()==old['packet_header17_vid89abcdef_type13_amount_minus01020304_value12345678']
lines=(p/'isolated-input-closure.patch').read_text().splitlines()
for line in lines:
 if line.startswith(('---','+++','@@')):continue
 assert line and line[0] in ' +-'
changes=json.loads((p/'isolated-input-closure.json').read_text())
assert len(changes['targets'])==6
files=['stat_primitives_regression.cpp','stat_primitives_desc_output_syscalls.c','stat_primitives_unused_input_fixture.cpp','Makefile.stat-primitives.inc','isolated-input-closure.patch','stat_primitives_abi.cpp','REGRESSION_HANDOFF.md']
out={'validation':'Static candidate review only; not compiled or run. No production edits or builds.', 'golden_packet_bytes':values.hex(),'isolated_target_count':6,'current_base_syscall_fixture_sha256':hashlib.sha256(Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\tests\\desc_output_syscalls.c')).read_bytes()).hexdigest(),'files':{name:hashlib.sha256((p/name).read_bytes()).hexdigest() for name in files}}
(p/'regression-staging-validation.json').write_text(json.dumps(out,indent=2))
print(json.dumps(out))
