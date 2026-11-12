import json,hashlib,re,struct
from pathlib import Path
p=Path('work/target_walking');data=json.loads((p/'old-types-signatures-strings.json').read_text());program=json.loads((p/'program-info.json').read_text())
assert data['target_sha256']==program['sha256']=='61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c'
functions=[];graphs=[];instructions=[]
for f in sorted(p.glob('*-decompile.json')):
 d=json.loads(f.read_text());assert d['truncated'] is False
 (p/(f.stem+'.c')).write_text(d['code'])
 functions.append({'address':d['function']['address'],'name':d['function']['name'],'decompilation':f.name,'complete_export':True})
for f in sorted(p.glob('*-calls.json')):
 d=json.loads(f.read_text());pages=d if isinstance(d,list) else [d]
 for page in pages:
  for kind in ('callers','callees'):
   if page.get(kind):assert page[kind]['has_more'] is False
  graphs.append({'function':page['function'],'callers':page.get('callers'),'callees':page.get('callees'),'evidence':f.name})
for address,total in [('08132670',69),('081341a0',45)]:
 d=json.loads((p/(address+'-instructions.json')).read_text());assert d['total']==len(d['items'])==total and not d['has_more']
 instructions.append({'address':address,'instruction_count':total,'file':address+'-instructions.json','complete':True})
pack=(p/'packet-target-walk.inc').read_text()
for name,fields in [('packet_target',[('BYTE','header'),('DWORD','dwVID'),('BYTE','bHPPercent')]),('SPacketGCWalkMode',[('BYTE','header'),('DWORD','vid'),('BYTE','mode')])]:
 body=re.search(r'struct\s+'+name+r'\s*\{(.*?)\}',pack,re.S).group(1)
 assert re.findall(r'\b(BYTE|DWORD)\s+(\w+)\s*;',body)==fields
 old=next(r for r in data['records'] if r['unit']=='char.cpp' and r['name']==name)
 assert old['size']==6 and [m['name'] for m in old['members']]==[x[1] for x in fields]
ctype=next(e for e in data['enums'] if e['name']=='ECharType')
for v in ctype['values']:assert re.search(r'\b'+v['name']+r'\s*=\s*'+str(v['value'])+r'\s*,',(p/'common-length-char-type.inc').read_text())
text=(p/'target_walking_regression.cpp').read_text();fixture=(p/'target_walk_desc_output_syscalls.c').read_text()
assert not re.search(r'void\s+CHARACTER::',text)
assert 'for(unsigned type=0;type<256;++type)' in text
assert 'source.BroadcastTargetPacket();' in text and 'std::signal(SIGABRT,ChildAbort)' in text
assert 'abort();' in fixture and 'line!=4801' in fixture
for label in ('walking_log','running_log'):
 raw=next(s for s in data['elf_strings'] if s['name']==label)
 assert raw['escaped_bytes'] in (p/'char-target-walk-bodies.inc').read_text()
manifest={'schema_version':1,'target_sha256':program['sha256'],'program_info':'program-info.json','scope':'Two complete candidate methods and existing direct/transitive dependency exports; no build, runtime test, original execution or whole-server equivalence claim.','metadata':'old-types-signatures-strings.json','functions':functions,'graphs':graphs,'instructions':instructions,'packet_layouts':{'packet_target':{'size':6,'offsets':{'header':0,'dwVID':1,'bHPPercent':5}},'SPacketGCWalkMode':{'size':6,'offsets':{'header':0,'vid':1,'mode':5}}},'expected_vectors':{'target_vid89abcdef_hp33':struct.pack('<BIB',63,0x89abcdef,33).hex(),'walk_vid1234_mode1':struct.pack('<BIB',111,1234,1).hex()},'candidate_hashes':{f.name:hashlib.sha256(f.read_bytes()).hexdigest() for f in sorted(p.iterdir()) if f.suffix in ('.inc','.cpp','.c') and not re.match(r'^[0-9a-f]{8}-',f.name)},'validation':'Static export pagination, metadata/field order, original strings and candidate structure checked. ABI/regression sources remain uncompiled/unrun.'}
manifest['artifacts']=[{'path':f.name,'bytes':f.stat().st_size,'sha256':hashlib.sha256(f.read_bytes()).hexdigest()} for f in sorted(p.iterdir()) if f.is_file() and f.name!='manifest.json']
(p/'manifest.json').write_text(json.dumps(manifest,indent=2))
print(json.dumps({'functions':len(functions),'graph_roots':len(graphs),'instructions':instructions,'staged_validation':'passed static checks only'}))
