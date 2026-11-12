from pathlib import Path
import argparse,json,hashlib
p=Path(__file__).resolve().parent;ap=argparse.ArgumentParser();ap.add_argument('--root',required=True);ap.add_argument('--full',required=True);ap.add_argument('--focused',required=True);ap.add_argument('--o2',required=True);ap.add_argument('--strict32-log',required=True);ap.add_argument('--strict32-exit-code',type=int,required=True);ap.add_argument('--source-guards');ap.add_argument('--out',default=str(p/'prepared'));a=ap.parse_args();root=Path(a.root);rec=root/'reconstruction';out=Path(a.out).resolve();assert p.parent.parent.resolve() in out.parents
load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'));h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest()
def passed(q):
 d=load(q);assert d.get('status')=='passed' and d.get('exit_code')==0 and d.get('finished_utc'),q;return d
m=load(p/'manifest.json');full=passed(a.full);focused=passed(a.focused);o2=passed(a.o2)
needed={'attribute_tail_regression','weighted_attribute_regression','item_attribute_regression'}
for d,scope in [(full,'full'),(focused,'focused')]:
 assert d.get('scope')==scope and d.get('make_exit_code')==0 and d.get('abi_requested') is True and 'check-abi' in d['command']
 s=d['suites'];r=d['executed_runtime_commands'];assert s==r and len(s)==len(set(s)) and needed<=set(s)
assert len(full['suites'])==m['expected_runtime_count']
assert o2.get('real_events') is True
obj={'item_sql_runtime_attribute_tail_o2.o','item_sql_semaphore_attribute_tail_o2.o','item_sql_api_attribute_tail_o2.o','item_socket_config_attribute_tail_o2.o'}
assert set(o2['distinct_optimized_SQL_objects'])==obj
assert all(any('-O2' in c and c[-1]==x and c[-2]=='-o' for c in o2['commands']) for x in obj)
assert any('-O2' in c and '../../game/src/item_attribute.cpp' in c and '../../game/src/item.cpp' in c and '-DMCNCC_TEST_REAL_EVENTS' in c for c in o2['commands'])
for rel,want in o2['source_sha256'].items():assert h(rec/'tests'/rel)==want,rel
strict=Path(a.strict32_log);assert a.strict32_exit_code==0 and strict.is_file() and 'error:' not in strict.read_text(encoding='utf-8',errors='replace').lower()
guards={rel:d['after_sha256'] for rel,d in m['files'].items()};review='Exact frozen6-file additive payload'
if a.source_guards:
 g=load(a.source_guards);assert g.get('review') and set(g['guards'])==set(guards);guards=g['guards'];review=g['review']
for rel,want in guards.items():assert h(root/rel)==want,rel
status=load(rec/'validation-status.json');prior=status.get('checkpoint85_weighted_attributes',{});assert prior.get('status')=='passed' and prior.get('full_runtime_commands')==93
normalized=load(p/'attribute-tail-ghidra.json');mapping=load(p/'attribute-tail-coverage.pending.json');assert len(normalized['functions'])==10 and all(not f.get('truncated') for f in normalized['functions']);assert len(mapping['reviews'])==10
assert normalized['target_sha256']==mapping['target_sha256']==m['target_sha256']
merged=load(rec/'function-coverage-overrides.json');baselineOver=h(rec/'function-coverage-overrides.json');baselineStatus=h(rec/'validation-status.json')
for r in mapping['reviews']:
 for ref in r['source_refs']+r['test_refs']:
  q=root/ref['path'];s=q.read_text(encoding='utf-8');assert ref['anchor'] in s;ref['line']=s[:s.index(ref['anchor'])].count('\n')+1;ref['file_sha256']=h(q);ref.pop('staged_file_sha256',None);ref.pop('status',None)
 r['reviewer']='Parent checkpoint86 complete originals/API/instructions/source + focused/O2/strict32/full94+ABI review'
 r['limitations']=r['limitations'].replace('Staged, not compiled or run by child. ','')
 old=next((x for x in merged['reviews'] if x['address']==r['address']),None)
 if old is None:merged['reviews'].append(r)
 else:old.clear();old.update(r)
proofs={str(Path(q).resolve()):h(Path(q)) for q in [a.full,a.focused,a.o2,a.strict32_log]}
note='Checkpoint86:10 complete original item_attribute.cpp tail methods integrated additively with original const/nonconst API and full existing item/map/packed records. Focused actual tail+weighted+itemattribute/ABI, optimized actual SQL/events/application graph, parent-confirmed strict32 exit0 and all'+str(len(full['suites']))+' ordered runtime commands+ABI pass. Original recursive RemoveAttributeType unchanged; rare hole/prefix counting, key-vs-index filters, unweighted rare draw, live post-RNG records, copy-hook/save-only behavior, fixed normal change count and actual addon Gaussian/cache semantics retained. Invalid raw pointers/negative removal indices/invalid rare set/empty availability/post-RNG zero-cap remain unsafe original domains and were not forced. Typed map rows are synthetic test inputs, not Boot/data restoration. Complete server/stat/quest/horse/startup reconstruction remains unfinished.'
status['checkpoint86_attribute_tail']={'status':'passed','full_runtime_commands':len(full['suites']),'focused_runtime_commands':len(focused['suites']),'abi':True,'original_function_mappings':10,'proof_sha256':proofs,'source_guards':guards,'source_guard_review':review,'strict32_exit_confirmation':'parent-supplied0','limitations':note}
out.mkdir(parents=True,exist_ok=True)
def write(n,x):(out/n).write_text(json.dumps(x,indent=2)+'\n',encoding='utf-8')
write('attribute-tail-ghidra.json',normalized);write('attribute-tail-symbol-manifest.json',load(p/'attribute-tail-symbol-manifest.json'));write('function-coverage-overrides.json',merged);write('validation-status.json',status)
for n in ['CURRENT_STATUS.md','RESUME_CHECKPOINT.md']:(out/n).write_text((rec/n).read_text(encoding='utf-8')+'\n\n'+note+'\n',encoding='utf-8')
(out/'attribute-tail-recovery.md').write_text('# Complete item attribute tail — checkpoint86\n\n'+note+'\n',encoding='utf-8')
write('RECORD_MANIFEST.json',{'checkpoint':86,'production_writes':False,'full_runtime_commands':len(full['suites']),'original_function_mapping_addresses':sorted(x['address'] for x in mapping['reviews']),'source_guards':guards,'source_guard_review':review,'proof_sha256':proofs,'baseline_overrides_sha256':baselineOver,'baseline_validation_status_sha256':baselineStatus,'note':note})
print(json.dumps({'prepared_workspace_only':str(out),'original_mappings':10,'full_runtime_commands':len(full['suites'])}))
