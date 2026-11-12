from pathlib import Path
import hashlib,json
p=Path(__file__).parent;m=json.loads((p/'horse-access-fix-manifest.json').read_text());f=Path(m['target']);payload=Path(m['payload'])
sha=lambda x:hashlib.sha256(x.read_bytes()).hexdigest()
assert sha(payload)==m['after_sha256']
assert sha(f) in (m['before_sha256'],m['after_sha256']),"Current char_horse.cpp changed"
if sha(f)==m['before_sha256']:
 (p/'char_horse-before-private-access-fix.cpp').write_bytes(f.read_bytes())
 f.write_bytes(payload.read_bytes())
assert sha(f)==m['after_sha256']
(p/'horse-access-fix-applied.json').write_text(json.dumps({'applied':True,'target':str(f),'sha256':sha(f),'validation_pending':True},indent=2))
print('Horse private-field fix applied through actual inline accessors; validation pending.')
