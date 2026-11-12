# Resolve portable paths from the repository root, regardless of current working directory.
import sys as _portable_sys
from pathlib import Path as _portable_Path
for _portable_parent in (_portable_Path(__file__).resolve().parent, *_portable_Path(__file__).resolve().parents):
    if (_portable_parent / 'project_paths.py').is_file():
        _portable_sys.path.insert(0, str(_portable_parent))
        break
else:
    raise RuntimeError('project_paths.py not found in repository ancestors')
from project_paths import resolve_path as _resolve_portable_path
from pathlib import Path
from datetime import datetime
from zoneinfo import ZoneInfo
import json,hashlib,re,copy
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';combined=w/'checkpoint102_quest_constructor_context';npc=w/'quest_npc_context';arg=w/'arg_script_constructor';argi=w/'arg_script_constructor_integration';out=p/'prepared'
load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'))
h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest()
proofs=[]
def passed(q):
 d=load(q);assert d.get('status')=='passed' and d.get('exit_code')==0 and d.get('finished_utc'),q;proofs.append(q)
 if d.get('log'):
  qlog=Path(d['log']);assert qlog.is_file()
  if d.get('log_sha256'):assert h(qlog)==d['log_sha256']
  proofs.append(qlog)
 return d
lastfullpath=w/'checkpoint-101-validation.receipt.json';lastfull=passed(lastfullpath);focuspath=w/'quest-constructor-context-focused-retry.receipt.json';focused=passed(focuspath)
assert lastfull['scope']=='full' and len(lastfull['suites'])==len(set(lastfull['suites']))==114 and lastfull['suites']==lastfull['executed_runtime_commands'] and lastfull['abi_requested'] and lastfull['make_exit_code']==0
assert focused['scope']=='focused' and focused['suites']==focused['executed_runtime_commands'] and set(focused['suites'])=={'arg_script_constructor_regression','quest_npc_context_regression','quest_ownership_regression','pc_quest_storage_regression'} and len(focused['suites'])==4 and focused['abi_requested'] and focused['make_exit_code']==0 and 'check-abi'in focused['command']
failedpath=w/'quest-constructor-context-focused.receipt.json';failed=load(failedpath);assert failed['status']=='failed' and failed['exit_code']!=0 and failed.get('finished_utc');proofs += [failedpath,Path(failed['log'])]
plan=load(combined/'integration/plan.json');review=load(p/'source-guards.json');assert plan['runtime_count_after']==116 and len(plan['files'])==len({r['path'].lower().replace('\\','/')for r in plan['files']})==11 and len(plan['dependencies'])==74
assert review['guards']=={r['path']:r['after']for r in plan['files']} and review['support_read_guards']=={r['path']:r['sha256']for r in plan['dependencies']}
for rel,want in {**review['guards'],**review['support_read_guards']}.items():assert h(root/rel)==want,rel
for r in plan['files']:assert h(combined/'integration/payload'/r['path'])==r['after']
make=(rec/'tests/Makefile').read_text(encoding='utf-8-sig');registry=re.search(r'^RUNTIME_TESTS = (.*)$',make,re.M).group(1).split();assert len(registry)==len(set(registry))==116 and registry[:2]==['arg_script_constructor_regression','quest_npc_context_regression'] and registry[2:]==lastfull['suites']
lines=make.splitlines();i=next(i for i,s in enumerate(lines)if s.startswith('check:'));commands=[]
for line in lines[i+1:]:
 if not line.startswith('\t'):break
 match=re.search(r'\./([A-Za-z0-9_]+)\$\(EXEEXT\)',line)
 if match:commands.append(match.group(1))
assert commands==registry,'Registered recipe ordering is a static check, NOT execution of full116'
fixdir=p/'reviewed_fixes';fix=load(fixdir/'reviewed-fixes.json');overrides=fix['reviewed_combined_package_overrides'];assert overrides==review['reviewed_combined_package_overrides'] and set(overrides)=={'work/checkpoint102_quest_constructor_context/integration/plan.json','work/checkpoint102_quest_constructor_context/integration/payload/game/src/questmanager.h'}
for q,want in overrides.items():assert h(w.parent/q)==want
oldplan=load(fixdir/'combined-plan.before.json');merge=load(combined/'MERGE_MANIFEST.json');assert h(fixdir/'combined-plan.before.json')==fix['original_combined_plan_sha256']==merge['plan_sha256'];expected=copy.deepcopy(oldplan)
row=next(x for x in expected['files']if x['path']=='game/src/questmanager.h');assert row['after']==h(fixdir/'questmanager.h.before')==fix['production_files'][0]['before_applied'];row['after']=h(fixdir/'questmanager.h.after');assert expected==plan
before=(fixdir/'questmanager.h.before').read_bytes();after=(fixdir/'questmanager.h.after').read_bytes();assert before.replace(b'LPCHARACTER GetCurrentNPCCharacterPtr()',b'CHARACTER* GetCurrentNPCCharacterPtr()')==after and before!=after
assert h(root/'game/src/questmanager.h')==h(fixdir/'questmanager.h.after')==fix['production_files'][0]['after']
for q in fixdir.iterdir():
 if q.is_file():proofs.append(q)
assert len(review['independent_reviews'])>=4
for r in review['independent_reviews']:
 q=w.parent/r['path'];assert h(q)==r['sha256'];d=load(q);assert d['status']in ['passed_no_actionable_mismatch','passed_exact_minimal_correction'] and not d.get('actionable_findings',[])
 if d['status']=='passed_exact_minimal_correction':assert q.name=='quest_npc_context_header_correction_independent_review.json' and d['after_header_sha256']==h(root/'game/src/questmanager.h') and d['updated_plan_sha256']==h(combined/'integration/plan.json')
 proofs.append(q)
evidence={}
def freeze(q,want,allow=False):
 key=str(q.relative_to(w.parent)).replace('\\','/');actual=h(q)
 if actual!=want:assert allow and overrides.get(key)==actual,(key,'unreviewed package drift')
 evidence[key]=actual
for r in load(combined/'PACKAGE_FILES.json')['files']:freeze(combined/r['path'],r['sha256'],True)
for r in load(arg/'MANIFEST.json')['files']:freeze(arg/r['path'],r['sha256'])
for r in load(argi/'MANIFEST.json')['upstream_files']:freeze(arg/r['path'],r['sha256'])
for rel,want in load(npc/'MANIFEST.json')['package_files'].items():freeze(npc/rel,want)
for r in merge['inputs']:assert h(r['path'])==r['sha256']
for q in [combined/'PACKAGE_FILES.json',combined/'MERGE_MANIFEST.json',arg/'MANIFEST.json',argi/'MANIFEST.json',npc/'MANIFEST.json']:evidence[str(q.relative_to(w.parent)).replace('\\','/')]=h(q)
assert merge['runtime_order']==registry
v=rec/'tests/quest_constructor_context_validation';ciq=v/'combined116-PROOF_INDEX.json';ci=load(ciq);assert ci['status']=='passed' and ci['plan_sha256']==h(combined/'integration/plan.json') and len(ci['receipts'])==len(ci['proof_indexes'])==2;proofs.append(ciq)
for r in ci['receipts']:assert h(r['path'])==r['sha256'];passed(Path(r['path']))
for r in ci['proof_indexes']:assert h(r['path'])==r['sha256'];proofs.append(Path(r['path']))
av=v/'constructor';ai=load(av/'arg-script-constructor-PROOF_INDEX.json');assert ai['status']=='passed' and ai['runtime_count_after']==116 and len(ai['receipts'])==3
for r in ai['receipts']:assert h(r['path'])==r['sha256'];passed(Path(r['path']))
for q,want in ai['source_sha256'].items():assert h(q)==want
ac=load(av/'arg-script-constructor-o2-build.receipt.json')['command'];assert '-O2'in ac and '-flto'not in ac and ac.count('../../game/src/questnpc.cpp')==1
asc=load(av/'arg-script-constructor-strict32.receipt.json')['command'];assert all(x in asc for x in ['-std=c++03','-pedantic-errors','-m32','-fsyntax-only','arg_script_constructor_regression.cpp','arg_script_constructor_abi.cpp','../../game/src/questnpc.cpp'])
nv=v/'npc_context';names=['strict32','item_sql_runtime-o2','item_sql_semaphore-o2','item_sql_api-o2','item_socket_config-o2','quest_npc_context_desc_output_syscalls-o2','o2-build','o2-runtime'];ds={n:passed(nv/('quest-npc-context-'+n+'.receipt.json'))for n in names}
assert all(x in ds['strict32']['command']for x in ['-std=c++03','-m32','-fsyntax-only','quest_npc_context_abi.cpp','quest_npc_context_regression.cpp','../../game/src/char.cpp','../../game/src/questmanager.cpp','../../game/src/questnpc.cpp'])
cmd=ds['o2-build']['command'];assert '-O2'in cmd and '-flto'not in cmd and '-DMCNCC_TEST_REAL_EVENTS'in cmd and cmd.count('../../game/src/questnpc.cpp')==1
objs=[]
for n in names[1:6]:
 c=ds[n]['command'];assert '-O2'in c and c[-2]=='-o' and c[-1]in cmd;objs.append(c[-1])
assert len(set(objs))==5 and ds['o2-runtime']['command']==[cmd[-1]]
ni=load(nv/'quest-npc-context-PROOF_INDEX.json');assert ni['status']=='passed' and len(ni['receipts'])==8 and h(ni['sdk_reuse_receipt'])==ni['sdk_reuse_receipt_sha256']
for r in ni['receipts']:assert h(r['path'])==r['sha256']
reusepath=nv/'quest-npc-context-sdk-reuse.receipt.json';reuse=load(reusepath);assert reuse['status']=='passed' and reuse['sdk_source_count']==28 and reuse['sdk_source_header_guards']==59;proofs.append(reusepath)
idxpath=Path(reuse['proof_index']);idx=load(idxpath);assert h(idxpath)==reuse['proof_index_sha256'] and idx['status']=='passed' and len(idx['receipts'])==39;proofs.append(idxpath)
for r in idx['receipts']:assert h(r['path'])==r['sha256'];passed(Path(r['path']))
for key,count in [('sdk_objects',28),('sdk_archives',2)]:
 assert len(idx[key])==count and len({r['path']for r in idx[key]})==count and reuse[key]==idx[key]
 for r in idx[key]:assert h(r['path'])==r['sha256']
assert all(r['path']in cmd for r in idx['sdk_archives'])
sdkroot=(root/'liblua').resolve();sdk_sources={q:want for q,want in idx['source_sha256'].items()if sdkroot in Path(q).resolve().parents};assert len(sdk_sources)==28
for q,want in sdk_sources.items():assert h(q)==want
for r in load(npc/'MANIFEST.json')['sdk_expected_files']:assert h(root/r['path'])==r['sha256']
original_ref=load(npc/'MANIFEST.json')['original_evidence_manifest'];original_manifest=w.parent/original_ref['path'];assert h(original_manifest)==original_ref['sha256'];original_root=original_manifest.parent
for r in load(original_manifest)['files']:freeze(original_root/r['path'],r['sha256'])
evidence[str(original_manifest.relative_to(w.parent)).replace('\\','/')]=h(original_manifest)
merged=load(rec/'function-coverage-overrides.json');sha=merged['target_sha256'];originals=[]
for a,n in [('08134f30',7),('08134f50',24),('081a5700',13),('081a5660',21)]:
 d=load(original_root/(a+'-decompile.json'));assert d['function']['address']==a and d.get('code')and not d.get('truncated');originals.append(d)
 asm=load(original_root/(a+'-assembly.json'));assert asm['has_more']is False and len(asm['items'])==asm['total']==n
assert load(arg/'MANIFEST.json')['target_sha256']==load(npc/'MANIFEST.json')['target_sha256']==sha

def ref(path,anchor):
 q=root/path;t=q.read_text(encoding='utf-8');assert anchor in t,(path,anchor)
 return dict(path=path,anchor=anchor,line=t[:t.index(anchor)].count('\n')+1,file_sha256=h(q))
limits='Current102 validation is FOCUSED ONLY;116registered, not116executed. Last full run is101/114 on prior source state, preserved historically. NPC singleton assert and two-current-lookup sequence static-only; tests change registries/context between ordinary calls, no invented Find hook/concurrent race. Inline AArgScript constructor initializes full STL members but leaves quest_index/state_index indeterminate; tests initialize both BEFOREcopy, original loader default-resize UB remains excluded. No new standalone ctor/dtor mapping. Actual supplied SDK5.0.3 public opaque compatibility only; original5.0/internalABI equality unproven (96/16 vs92/12). SDK28objects/twoarchives reusedstrictSHA/all59guards, not rebuilt. Prior OpenState/Build matching static-only; fullVM/dispatch/Reward/EndRunning/server/VisualStudio unfinished.'
for d in originals:
 a=d['function']['address'];name=d['function']['name'].split('::')[-1];owner='char.cpp'if a in ['08134f30','08134f50']else'questmanager.cpp';cls='CHARACTER'if owner=='char.cpp'else'CQuestManager'
 r={'address':a,'source_refs':[ref('game/src/'+owner,cls+'::'+name+'(')],'test_refs':[ref('reconstruction/tests/quest_npc_context_regression.cpp','int main(')],'review_scope':'Whole original NPC VID/context/race method using actual manager registry and existing race method. Bounded ordinary-state runtime; exact singleton assertion/double-lookup roles retained statically.','reviewer':'Parent102 wholeoriginal review +focused4/constructor3/NPC8+SDKreuse, NOTfull116','limitations':limits}
 old=next((x for x in merged['reviews']if x['address']==a),None)
 if old is None:merged['reviews'].append(r)
 else:old.clear();old.update(r)
status=load(rec/'validation-status.json');assert status['checkpoint101_quest_ownership']['status']=='passed'
for key,expected in [('runtime_suites',lastfull['suites']),('runtime_suite_count',114),('runtime_commands_verified',lastfull['executed_runtime_commands']),('runtime_command_count_verified',114)]:assert status[key]==expected,(key,'legacy lastfull fields must remain114')
lastfullmeta={'checkpoint':101,'runtime_commands':114,'suites':lastfull['suites'],'receipt':str(lastfullpath.resolve()),'receipt_sha256':h(lastfullpath),'log':lastfull['log'],'finished_utc':lastfull['finished_utc'],'scope':'Historical prior101 source state; does not validate current102 all116'}
note='Checkpoint102 focused validation passed4normal suites+ABI, constructor strict/O2 three receipts, NPC strict/O2 eight+SDKreuse and sealed combinedindex.116tests REGISTERED only; latestFULL remains101/114.4wholeNPCmethods+1genuineinlineAArgScriptctor integrated, exactheaderalias spelling fix documented; no full116 claim. '+limits
manifest={'checkpoint':102,'status':'passed_focused','validation_scope':'focused_only','registered_runtime_commands':116,'registered_suites':registry,'focused_runtime_commands':4,'executed_focused_suites':focused['executed_runtime_commands'],'last_full_validation':lastfullmeta,'abi':True,'standalone_function_mappings':4,'inline_semantic_bodies':1,'source_guards':review['guards'],'support_read_guards':review['support_read_guards'],'source_guard_review':review,'proof_sha256':{str(q.resolve()):h(q)for q in proofs},'original_evidence_sha256':evidence,'input_sha256':{str(q.resolve()):h(q)for q in [combined/'PACKAGE_FILES.json',combined/'MERGE_MANIFEST.json',p/'source-guards.json',fixdir/'reviewed-fixes.json',p/'VALIDATION_POLICY.md',Path(__file__)]},'baseline_metadata_sha256':{name:h(rec/name)for name in ['function-coverage-overrides.json','validation-status.json','CURRENT_STATUS.md','RESUME_CHECKPOINT.md']},'baseline_overrides_sha256':h(rec/'function-coverage-overrides.json'),'baseline_validation_status_sha256':h(rec/'validation-status.json'),'note':note}
status.update(date=datetime.fromisoformat(focused['finished_utc']).astimezone(ZoneInfo('Europe/Berlin')).date().isoformat(),latest_validation_scope='focused_only',registered_runtime_suites=registry,registered_runtime_suite_count=116,latest_focused_suites=focused['suites'],latest_focused_check_exit_code=0,latest_focused_receipt=str(focuspath.resolve()),latest_validation_receipt=str(focuspath.resolve()),last_full_validation=lastfullmeta,legacy_full_fields_last_checkpoint=101)
status['checkpoint102_constructor_context']=manifest
inline={'target_sha256':sha,'inline_source_ref':ref('game/src/quest.h','AArgScript() {}'),'inline_evidence':load(arg/'MANIFEST.json'),'source_guard':load(arg/'SOURCE_GUARD.json'),'test_ref':ref('reconstruction/tests/arg_script_constructor_regression.cpp','int main('),'policy':'Original empty user body/full implicit STL members, indeterminate scalars preserved. Inline081af25d..081af263 is NOTstandalone function; compiler destructor/copy helpers context-only; no zero defaults or explicit destructor added.','limits':limits}
out.mkdir(exist_ok=True)
def write(name,data):(out/name).write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')
for name,data in [('RECORD_MANIFEST.json',manifest),('validation-status.json',status),('function-coverage-overrides.json',merged),('quest-constructor-context-ghidra.json',{'target_sha256':sha,'functions':originals}),('arg-script-inline-review.json',inline)]:write(name,data)
(out/'VALIDATION_POLICY.md').write_text((p/'VALIDATION_POLICY.md').read_text(encoding='utf-8-sig'),encoding='utf-8')
(out/'quest-constructor-context-recovery.md').write_text('# Constructor/NPC context checkpoint102 — focused validation\n\n'+note+'\n',encoding='utf-8');print('Prepared workspace-only102: FOCUSED4 passed, REGISTERED116, LASTFULL101/114 preserved. No project writes/full116 claim.')
