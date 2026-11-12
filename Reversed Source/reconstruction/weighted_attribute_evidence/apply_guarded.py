"""Parent only: preflight every source SHA before writing any production file."""
from pathlib import Path
import hashlib,json,shutil
base=Path(__file__).resolve().parent
manifest=json.loads((base/'manifest.json').read_text())
root=Path(manifest['root']);stage=base/'integration-ready'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest() if p.exists() else None
for rel,m in manifest['files'].items():
 assert sha(root/rel)==m['before_sha256'],'Production changed: '+rel
 assert sha(stage/rel)==m['after_sha256'],'Stage changed: '+rel
for rel,m in manifest['files'].items():
 p=root/rel;backup=base/'parent-apply-backup'/rel
 if p.exists():backup.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(p,backup)
 p.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(stage/rel,p)
 assert sha(p)==m['after_sha256']
print('Applied',len(manifest['files']),'SHA-guarded files.')
