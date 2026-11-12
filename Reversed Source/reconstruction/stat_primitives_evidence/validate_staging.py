import hashlib,json,re
from pathlib import Path
p=Path('work/stat_primitives');d=json.loads((p/'old-types-signatures.json').read_text());summary=json.loads((p/'evidence-summary.json').read_text())
assert d['target_sha256']==json.loads((p/'program-info.json').read_text())['sha256']
checks={}
for name,file,expected in [('SPlayerSkill','common-tables-player-skill.inc',[('BYTE','bMasterType'),('BYTE','bLevel'),('int','tNextRead')]),('packet_point_change','game-packet-point-change.inc',[('int','header'),('DWORD','dwVID'),('BYTE','type'),('long','amount'),('long','value')])]:
 text=re.sub(r'//[^\n]*','',(p/file).read_text())
 body=re.search(r'struct\s+'+name+r'\s*\{(.*?)\}',text,re.S).group(1)
 fields=re.findall(r'\b(BYTE|DWORD|int|long)\s+(\w+)\s*;',body)
 assert fields==expected,(name,fields)
 original=next(r for r in d['records'] if r['unit']=='char.cpp' and r['name']==name)
 assert [v['name'] for v in original['members']]==[v[1] for v in fields]
 checks[name]={'all_original_members_present_in_order':True,'old_size':original['size'],'candidate_time_storage':('old_signed_int32' if name=='SPlayerSkill' else 'not_applicable')}
for address,count in [('080f2d50',21),('0812a580',31),('081485e0',9),('08148080',8)]:
 ins=json.loads((p/(address+'-instructions.json')).read_text());assert ins['total']==count and len(ins['items'])==count and not ins['has_more']
 for x in json.loads((p/(address+'-calls.json')).read_text()):assert not x['callers']['has_more'] and not x['callees']['has_more']
for f in p.glob('*-decompile.json'):assert json.loads(f.read_text())['truncated'] is False
out={'target_sha256':d['target_sha256'],'validation_kind':'Static metadata/candidate/export completeness only. No compiler, build, runtime test or original target execution.','records':checks,'complete_helper_instruction_counts':{'RaceToJob':21,'GetJob':31,'IsDead':9,'IsStun':8},'full_export_count':len(list(p.glob('*-decompile.json'))),'candidate_sha256':{f.name:hashlib.sha256(f.read_bytes()).hexdigest() for f in p.glob('*.inc')}}
(p/'staging-validation.json').write_text(json.dumps(out,indent=2));print(json.dumps(out))
