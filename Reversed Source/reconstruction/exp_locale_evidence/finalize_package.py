import json,re,struct,hashlib
from pathlib import Path
p=Path('work/exp_locale')
d=json.loads((p/'data.json').read_text())
src=(p/'constants-exp-definitions.inc').read_text()
checks=[]
for row in d['symbols']:
 if row['name']=='exp_table': continue
 m=re.search(r'const DWORD '+row['name']+r'\[100\] = \{(.*?)\};',src,re.S)
 assert m
 vals=[int(s) for s in re.findall(r'(\d+)UL',m.group(1))]
 raw=struct.pack('<100I',*vals)
 assert raw.hex()==row['raw_hex']
 assert hashlib.sha256(raw).hexdigest()==row['sha256']
 checks.append({'name':row['name'],'count':len(vals),'sha256':row['sha256']})
exports=[]
for f in sorted(p.glob('*-decompile.json')):
 r=json.loads(f.read_text()); assert not r['truncated']
 exports.append({'address':r['function']['address'],'name':r['function']['name'],'code':r['code'],'evidence':{'raw_file':f.name,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()},'status':'evidence_available_unreviewed'})
out={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','program_function_count':7238,'exported_function_count':len(exports),'functions':exports,'note':'Full raw export provenance preserved; availability does not claim implementation or complete recovery.'}
(p/'locale-addressed-evidence.json').write_text(json.dumps(out,indent=2))
files={}
for f in sorted(p.iterdir()):
 if f.is_file() and f.name!='manifest.json':
  files[f.name]={'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()}
(p/'manifest.json').write_text(json.dumps({'target_sha256':out['target_sha256'],'verified_tables':checks,'table_bytes':2000,'initial_pointer_bytes':'00000000','exported_functions':len(exports),'validation':'Static byte comparison only; no builds or ELF execution by staging agent.','files':files},indent=2))
print(json.dumps({'tables_verified':len(checks),'bytes_verified':2000,'normalized_evidence_functions':len(exports),'files_hashed':len(files)}))
