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
import json, subprocess
from pathlib import Path
p = Path('work/pc_quest_storage')
x = json.loads((p / 'metadata.json').read_text(encoding='utf-8-sig'))
names = ['quest_begin', 'quest_end', 'quest_find', 'TQuestStateChangeInfo']
for n in names:
    rows = [r for r in x['inline_instances'] if r['name'] == n and r['attrs']['DW_AT_low_pc'] and (r['attrs']['DW_AT_low_pc'] >= 134512640)]
    print(n, [(hex(r['attrs']['DW_AT_low_pc']), hex(r['attrs']['DW_AT_high_pc']), r['placement']) for r in rows[:2]])
    for i, r in enumerate(rows[:2]):
        a = r['attrs']['DW_AT_low_pc']
        b = r['attrs']['DW_AT_high_pc']
        s = subprocess.run(['C:\\Strawberry\\c\\bin\\objdump.exe', '-d', '--start-address=' + hex(a - 15), '--stop-address=' + hex(b + 15), _resolve_portable_path('PROJECT_ROOT\\Original Binaries\\game')], capture_output=True, text=True, check=True)
        p.joinpath('evidence/' + n + '-inline-parent' + str(i) + '.txt').write_text(s.stdout, encoding='utf-8')
