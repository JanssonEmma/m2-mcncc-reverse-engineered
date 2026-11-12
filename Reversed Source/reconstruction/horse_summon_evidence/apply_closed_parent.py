"""Parent-only apply with its existing F permission; child does not invoke this."""
from pathlib import Path
import hashlib,json,shutil,datetime
p=Path(__file__).parent
m=json.loads((p/'apply-manifest.json').read_text())
root=Path(m['root']);ready=p/'integration-ready'
sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest() if f.exists() else None
# Check every source, prepared payload, and protected fixture BEFORE ANY production write.
for row in m['files']:
 target=root/row['path'];payload=ready/row['path']
 assert sha(payload)==row['after_sha256'],('staged payload changed',row['path'])
 actual=sha(target)
 assert actual in (row['before_sha256'],row['after_sha256']),('production SHA conflict',row['path'],actual)
for name,digest in m['protected_global_fixtures'].items():
 assert sha(root/'reconstruction/tests'/name)==digest,('protected global fixture changed',name)
backup=p/'parent-apply-backup'
applied=[];already=[]
for row in m['files']:
 target=root/row['path']
 if sha(target)==row['after_sha256']:
  already.append(row['path']);continue
 if target.exists():
  saved=backup/row['path'];saved.parent.mkdir(parents=True,exist_ok=True)
  if not saved.exists():shutil.copyfile(target,saved)
 target.parent.mkdir(parents=True,exist_ok=True)
 target.write_bytes((ready/row['path']).read_bytes())
 assert sha(target)==row['after_sha256']
 applied.append(row['path'])
 # Save exact per-file progress immediately, allowing a guarded resume after interruption.
 (p/'parent-apply-progress.json').write_text(json.dumps({'status':'applying','applied':applied,'already_applied':already,'validation_pending':True},indent=2))
for name,digest in m['protected_global_fixtures'].items():assert sha(root/'reconstruction/tests'/name)==digest
result={'status':'applied_focused_validation_pending','utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'applied':applied,'already_applied':already,'global_fixtures_unchanged':True,'full_suite_run':False}
(p/'parent-apply-progress.json').write_text(json.dumps(result,indent=2))
print(json.dumps(result,indent=2))
