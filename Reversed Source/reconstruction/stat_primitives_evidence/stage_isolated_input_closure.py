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
# Read current Makefile; stage a reviewable patch only. Never writes production.
import difflib,json,hashlib
from pathlib import Path
source=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\tests\\Makefile'))
out=Path('work/stat_primitives')
text=source.read_text()
lines=text.splitlines(keepends=True)
target=None;selected=set()
for line in lines:
 if line and not line.startswith(('\t','#',' ')) and ':' in line and ':=' not in line:
  target=line.split(':',1)[0]
 if line.startswith('\t') and '../../game/src/char.cpp' in line and 'DESC_PACKET_SOURCES' not in line:
  assert target
  selected.add(target)
assert len(selected)==6,sorted(selected)
new=[];target=None;inserted=False
for line in lines:
 if not inserted and line.startswith('.PHONY: check check-abi clean'):
  new.append('# TEST ONLY link closure; RaceToJob remains in its original production module.\n')
  new.append('ISOLATED_LOGIN_CLOSURE = ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp\n\n')
  inserted=True
 if line and not line.startswith(('\t','#',' ')) and ':' in line and ':=' not in line:
  target=line.split(':',1)[0]
  if target in selected:line=line.rstrip('\r\n')+' $(ISOLATED_LOGIN_CLOSURE)\n'
 if line.startswith('\t') and target in selected and '../../game/src/char.cpp' in line and 'DESC_PACKET_SOURCES' not in line:
  assert '$(LDFLAGS)' in line
  line=line.replace('$(LDFLAGS)','$(ISOLATED_LOGIN_CLOSURE) $(LDFLAGS)',1)
 new.append(line)
assert inserted
new=''.join(new).splitlines(keepends=True)
patch=''.join(difflib.unified_diff(lines,new,fromfile='a/reconstruction/tests/Makefile',tofile='b/reconstruction/tests/Makefile'))
(out/'isolated-input-closure.patch').write_text(patch)
(out/'isolated-input-closure.json').write_text(json.dumps({'source':str(source),'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'targets':sorted(selected),'policy':'Review patch and apply after current builds finish. Fixture must never be linked with real input.cpp or desc_fixture.cpp. No production files modified by this script.'},indent=2))
print(json.dumps({'targets':sorted(selected),'patch_lines':len(patch.splitlines())}))
