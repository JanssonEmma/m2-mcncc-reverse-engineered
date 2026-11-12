import json,hashlib
from pathlib import Path
p=Path('work/stat_dependency')
def unwrap(d):return d.get('structuredContent',d)
exports=[]
for f in p.glob('*-decompile.json'):
 d=unwrap(json.loads(f.read_text()));assert not d.get('truncated'),f
 (p/(f.stem+'.c')).write_text(d['code'])
 exports.append({'address':d['function']['address'],'name':d['function']['name'],'file':f.name,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
graphs=[]
for f in p.glob('*-calls.json'):
 d=json.loads(f.read_text());pages=d if isinstance(d,list) else [unwrap(d)]
 for q in pages:
  c=q['callees'];assert not c['has_more'],f
  graphs.append({'function':q['function'],'calls':c['items'],'file':f.name})
m=json.loads((p/'old-data-metadata.json').read_text())
enums={e['name']:e for e in m['enums']}
lines=['// Candidate data/API declarations ONLY. Do not copy modern enum extensions.','#ifndef OLD_STAT_DEPENDENCY_CANDIDATE_H','#define OLD_STAT_DEPENDENCY_CANDIDATE_H','// Assumes existing target BYTE/DWORD/long32 and original CHARACTER records.']
for name in ('EApplyTypes','EPointTypes'):
 e=enums[name];lines+=['enum '+name+' {']+['    '+v['name']+' = '+str(v['value'])+',' for v in e['values']]+['};']
lines+=['// Candidate CHARACTER member declarations, DWARF signatures:','// void ApplyPoint(BYTE bApplyType, long lVal);','// void PointChange(BYTE type, int amount, bool bAmount, bool bBroadcast);','// Do not infer defaults from the newer reference; old internal ApplyPoint calls use false,false.','// Required helpers include ComputeBattlePoints, all old PointChange branch dependencies.','#endif','']
(p/'candidate-stat-data-api.h').write_text('\n'.join(lines))
apply_values=next(g['byte_values'] for g in m['globals'] if g['name']=='aApplyInfo')
apply_names={v['value']:v['name'] for v in enums['EApplyTypes']['values']}
point_names={v['value']:v['name'] for v in enums['EPointTypes']['values']}
mapping=[{'apply':i,'apply_name':apply_names.get(i),'point':v,'point_name':point_names.get(v),'special_dispatch':i in (0,3,4,51,77)} for i,v in enumerate(apply_values)]
(p/'apply-point-map.json').write_text(json.dumps(mapping,indent=2))
info=unwrap(json.loads((p/'program-info.json').read_text()))
manifest={'target_sha256':info['sha256'],'program':info,'scope':'Complete core and selected dependency exports, exhaustive direct call-site pages for saved graphs; deeper transitive closure is intentionally unfinished. No original execution.','exports':sorted(exports,key=lambda x:x['address']),'graphs':graphs,'constants_metadata':'old-data-metadata.json'}
(p/'manifest.json').write_text(json.dumps(manifest,indent=2))
for a,total in [('0812e340',1342),('0812f7d0',255),('08167420',335)]:
 pages=json.loads((p/(a+'-instructions.json')).read_text());items=[i for q in pages for i in q.get('items',q.get('instructions',[]))];assert len(items)==total and not pages[-1]['has_more']
print(json.dumps({'decompiled_functions':len(exports),'call_graph_roots':len(graphs),'apply_entries':len(mapping),'all_exports_untruncated':True,'instruction_counts':[1342,255,335]}))
