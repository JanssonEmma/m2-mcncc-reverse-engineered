# Parent only. Default validates/prints plan, --apply performs guarded writes.
import argparse,json,hashlib,os
from pathlib import Path
p=Path(__file__).resolve().parent
a=argparse.ArgumentParser();a.add_argument('--apply',action='store_true');args=a.parse_args()
plan=json.loads((p/'parent_apply/plan.json').read_text());root=Path(plan['root']).resolve();base=p/'parent_apply'
def sha(b):return hashlib.sha256(b).hexdigest()
for r in plan['changes']:
 target=(root/r['target']).resolve()
 assert target.is_relative_to(root),'Target escapes approved root'
 old=target.read_bytes() if target.exists() else None
 assert (sha(old) if old is not None else None)==r['expected_sha256'],'Baseline changed: '+r['target']
 data=(base/r['payload']).read_bytes()
 assert sha(data)==r['desired_sha256'],'Payload changed: '+r['target']
print('Validated '+str(len(plan['changes']))+' file baselines and payloads.')
if not args.apply:
 print('Dry run only. Review payload/plan, then parent may rerun with --apply.')
else:
 backup=base/'backups'
 for r in plan['changes']:
  target=root/r['target']
  if target.exists():
   dest=backup/r['target'];dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(target.read_bytes())
 for r in plan['changes']:
  target=root/r['target'];target.parent.mkdir(parents=True,exist_ok=True)
  temporary=target.with_name(target.name+'.banword-stage-tmp')
  assert not temporary.exists(),'Unexpected temporary file'
  temporary.write_bytes((base/r['payload']).read_bytes());os.replace(temporary,target)
 print('Applied all guarded files. Existing-file backups: '+str(backup))
