# Parent only: all-file SHA preflight, then reversible backups/apply.
from pathlib import Path
import json,hashlib,shutil
p=Path(__file__).resolve().parent;m=json.loads((p/'integration-manifest.json').read_text());root=Path(m['root']);stage=p/'integration-ready'
sha=lambda q:hashlib.sha256(q.read_bytes()).hexdigest() if q.exists() else None
for rel,g in m['files'].items():
 assert sha(root/rel)==g['before_sha256'],'Current source changed: '+rel
 assert sha(stage/rel)==g['after_sha256'],'Staged source changed: '+rel
for rel,g in m['files'].items():
 dst=root/rel;src=stage/rel
 if dst.exists():
  backup=p/'parent-apply-backup'/rel;backup.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(dst,backup)
 dst.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(src,dst);assert sha(dst)==g['after_sha256']
print('Applied14 guarded PC files; canonical96 suites. No builds run.')
