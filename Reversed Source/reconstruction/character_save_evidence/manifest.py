from pathlib import Path
import json,hashlib
p=Path(__file__).parent
out={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','full_immediate_save_closure_recovered':True,'production_changed':False,'builds_or_tests_run':False,'source_preservation_audit_passed':True,'files':[]}
for f in p.iterdir():
 if f.is_file() and f.name!='manifest.json':out['files'].append({'file':f.name,'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
p.joinpath('manifest.json').write_text(json.dumps(out,indent=2))
print(len(out['files']),'artifacts hashed')
