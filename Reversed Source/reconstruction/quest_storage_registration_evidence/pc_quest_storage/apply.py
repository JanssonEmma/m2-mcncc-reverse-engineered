# Parent-only guarded application. Default validates all source/dependency/payload guards without F writes.
from pathlib import Path
import argparse,json,hashlib
p=argparse.ArgumentParser();p.add_argument('--plan',default=str(Path(__file__).resolve().parent/'integration/plan.json'));p.add_argument('--apply',action='store_true');a=p.parse_args();planpath=Path(a.plan).resolve();d=json.loads(planpath.read_text(encoding='utf-8-sig'));root=Path(d['root']).resolve();items=[]
for row in d.get('dependencies',[]):
 path=(root/row['path']).resolve();assert root in path.parents
 if not path.exists() or hashlib.sha256(path.read_bytes()).hexdigest()!=row['sha256']:raise SystemExit('STALE actual manager dependency '+row['path']+'; review/rebase prepare.py')
for x in d['files']:
 dst=(root/x['path']).resolve();assert root in dst.parents;raw=(planpath.parent/'payload'/x['path']).read_bytes()
 if hashlib.sha256(raw).hexdigest()!=x['after']:raise SystemExit('Stale payload '+x['path'])
 if x['before'] is None:
  if dst.exists():raise SystemExit('Refuse existing newfile '+x['path'])
 elif not dst.exists() or hashlib.sha256(dst.read_bytes()).hexdigest()!=x['before']:raise SystemExit('STALE source '+x['path']+'; review/rebase prepare.py')
 items.append((dst,raw,x))
if a.apply:
 for dst,raw,x in items:
  if dst.exists():
   backup=planpath.parent/'backups'/x['path'];backup.parent.mkdir(parents=True,exist_ok=True);backup.write_bytes(dst.read_bytes())
  dst.parent.mkdir(parents=True,exist_ok=True);dst.write_bytes(raw)
print(json.dumps({'mode':'applied' if a.apply else 'validated_only','files':len(items),'actual_dependencies':len(d.get('dependencies',[])),'scope':d['scope']}))
