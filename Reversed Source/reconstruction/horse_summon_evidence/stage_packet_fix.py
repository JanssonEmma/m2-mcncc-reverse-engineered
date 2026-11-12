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
import hashlib,json
p=Path(__file__).parent;f=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\game\\src\\packet.h'))
s=f.read_text();block=(p/'packet-record-additions.inc').read_text();count=s.count(block)
assert count>1,count
first=s.index(block)+len(block);fixed=s[:first]+s[first:].replace(block,'')
assert fixed.count('typedef struct packet_dead')==1 and fixed.count('typedef struct packet_move')==1
dest=p/'packet-single-definition-fixed.h';dest.write_text(fixed)
manifest={'target':str(f),'before_sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'payload':str(dest),'after_sha256':hashlib.sha256(dest.read_bytes()).hexdigest(),'removed_duplicate_blocks':count-1}
(p/'packet-fix-manifest.json').write_text(json.dumps(manifest,indent=2))
(p/'PACKET_FIX_RESUME.md').write_text('Focused build failed at packet duplicate declarations: original integration replace() expanded at every pack(pop). Stage fix retains FIRST exact block only, preserving every pragma/island and all other records. Apply apply_packet_fix_parent.py with root F grant; child no Fwrite. Then rerun focused suite+ABI only. No runtime was executed on failed build.\n')
print(json.dumps(manifest))
