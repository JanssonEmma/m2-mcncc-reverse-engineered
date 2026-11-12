from pathlib import Path
import json,hashlib
p=Path(__file__).parent;m=json.loads((p/'packet-fix-manifest.json').read_text());f=Path(m['target']);payload=Path(m['payload'])
sha=lambda x:hashlib.sha256(x.read_bytes()).hexdigest()
assert sha(payload)==m['after_sha256']
assert sha(f) in (m['before_sha256'],m['after_sha256']),"Current packet.h changed; inspect before applying"
if sha(f)==m['before_sha256']:
 (p/'packet-before-single-definition-fix.h').write_bytes(f.read_bytes())
 f.write_bytes(payload.read_bytes())
assert sha(f)==m['after_sha256']
(p/'packet-fix-applied.json').write_text(json.dumps({'applied':True,'target':str(f),'sha256':sha(f),'validation_pending':True},indent=2))
print("Packet single-definition fix applied; focused validation pending.")
