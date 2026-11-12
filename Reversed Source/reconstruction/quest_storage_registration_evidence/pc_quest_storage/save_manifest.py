from pathlib import Path
import json,hashlib
p=Path('work/pc_quest_storage');p.joinpath('MANIFEST.json').write_text(json.dumps({'scope':'Full7PCstorage originals+4inline semantic bodies, guardedcurrent100->101 staged only; no build/apply/pass claim.','files':[{'path':str(f).replace('\\','/'),'sha256':hashlib.sha256(f.read_bytes()).hexdigest()} for f in sorted(p.rglob('*')) if f.is_file() and f.name!='MANIFEST.json']},indent=2)+'\n',encoding='utf-8')
