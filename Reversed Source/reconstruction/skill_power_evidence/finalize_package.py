import json,hashlib
from pathlib import Path
p=Path('work/skill_power')
funcs=[]
for f in sorted(p.glob('*-decompile.json')):
 r=json.loads(f.read_text());assert not r['truncated']
 a=r['function']['address'];ins=json.loads((p/(a+'-instructions.json')).read_text())
 assert ins['has_more']==False and len(ins['items'])==ins['total']
 funcs.append({'address':a,'name':r['function']['name'],'code':r['code'],'status':'evidence_available_candidate_staged_not_tested','raw_export':f.name,'raw_sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'instruction_file':a+'-instructions.json'})
graphs=[]
for f in sorted(p.glob('*-calls.json')):
 r=json.loads(f.read_text())
 pages={k:{'total':r[k]['total'],'has_more':r[k]['has_more']} for k in ('callers','callees') if r.get(k)}
 assert all(not x['has_more'] for x in pages.values())
 graphs.append({'file':f.name,'pages':pages})
out={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','program_functions':7238,'exported_functions':len(funcs),'functions':funcs,'callgraphs':graphs,'note':'Evidence and candidates do not establish full recovery or validation. Main constructor context is a partial main page.'}
(p/'addressed-evidence.json').write_text(json.dumps(out,indent=2))
files={}
for f in sorted(p.iterdir()):
 if f.is_file() and f.name!='manifest.json':files[f.name]={'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()}
(p/'manifest.json').write_text(json.dumps({'target_sha256':out['target_sha256'],'complete_class_target_bytes':40,'power_slots':8,'ints_per_power_slot':41,'damage_ints':99,'validation':'No builds or original ELF execution. Static evidence only; regression/API/ABI candidates ready.','files':files},indent=2))
print(json.dumps({'full_exports':len(funcs),'complete_callgraphs':len(graphs),'files_hashed':len(files)}))
