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
import json,hashlib,re
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';out=p/'prepared';pkgs=[w/'quest_yield_bridges',w/'quest_predicate_diagnostics']
load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'))
h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest()
norm=lambda q:str(q).replace('\\','/')
proofs=[]
def passed(q):
 d=load(q);assert d.get('status')=='passed' and d.get('exit_code')==0 and d.get('finished_utc'),q;proofs.append(q)
 if d.get('log'):
  qlog=Path(d['log']);assert qlog.is_file()
  if d.get('log_sha256'):assert h(qlog)==d['log_sha256']
  proofs.append(qlog)
 return d
old=passed(w/'checkpoint-98-validation.receipt.json');full=passed(w/'checkpoint-99-validation.receipt.json');focused=passed(w/'quest-control-focused.receipt.json');suites={'quest_yield_bridges_regression','quest_predicate_diagnostics_regression'}
assert old['scope']=='full' and len(old['suites'])==110 and old['suites']==old['executed_runtime_commands']
for d in [full,focused]:
 assert d['make_exit_code']==0 and d['abi_requested'] and 'check-abi' in d['command'] and d['suites']==d['executed_runtime_commands'] and len(d['suites'])==len(set(d['suites']))
assert full['scope']=='full' and len(full['suites'])==112 and all(full['suites'].count(s)==1 for s in suites)
assert [s for s in full['suites'] if s not in suites]==old['suites']
assert focused['scope']=='focused' and set(focused['suites'])==suites|{'quest_state_lifecycle_regression','pc_state_consumers_regression'}
# Verified SDK reuse, not recompilation; old application-source hashes are intentionally outside SDK-only scope.
reuse_reviews={}
for pkg in pkgs:
 stem=pkg.name;prefix=stem.replace('_','-');v=rec/'tests'/(stem+'_validation');names=['strict32','item_sql_runtime-o2','item_sql_semaphore-o2','item_sql_api-o2','item_socket_config-o2',stem+'_desc_output_syscalls-o2','o2-build','o2-runtime'];ds={n:passed(v/(prefix+'-'+n+'.receipt.json')) for n in names}
 strict=ds['strict32']['command'];assert all(x in strict for x in ['-std=c++03','-pedantic-errors','-m32','-fsyntax-only',stem+'_abi.cpp',stem+'_regression.cpp','../../game/src/questmanager.cpp'])
 if stem=='quest_yield_bridges':assert '../../game/src/questlua.cpp' in strict
 command=ds['o2-build']['command'];assert '-O2' in command and '-DMCNCC_TEST_REAL_EVENTS' in command and '-flto' not in command
 for name in ['event.cpp','event_queue.cpp','questpc.cpp','questmanager.cpp','char.cpp','desc.cpp','buffer_manager.cpp']:assert '../../game/src/'+name in command,name
 if stem=='quest_yield_bridges':assert '../../game/src/questlua.cpp' in command
 assert 'buffer_manager_core.o' in command
 objects=[]
 for name in names[1:6]:
  c=ds[name]['command'];assert '-O2' in c and c[-2]=='-o';objects.append(c[-1]);assert c[-1] in command
 assert len(set(objects))==5 and ds['o2-runtime']['command']==[command[-1]]
 reusepath=v/(prefix+'-sdk-reuse.receipt.json');reuse=load(reusepath);assert reuse['status']=='passed' and reuse['sdk_source_count']==28 and reuse['sdk_source_header_guards']==59;proofs.append(reusepath)
 index=Path(reuse['proof_index']);idx=load(index);assert idx['status']=='passed' and idx['sdk_C_objects']==28 and len(idx['receipts'])==39 and h(index)==reuse['proof_index_sha256'];proofs.append(index)
 for row in idx['receipts']:
  assert h(row['path'])==row['sha256'];passed(Path(row['path']))
 for key,count in [('sdk_objects',28),('sdk_archives',2)]:
  assert len(idx[key])==count and len({x['path'] for x in idx[key]})==count and reuse[key]==idx[key]
  for row in idx[key]:assert h(row['path'])==row['sha256'],row
 assert all(row['path'] in command for row in idx['sdk_archives']) and not any(x in command for x in ['../../liblua/liblualib_mcncc.a','../../liblua/liblua_mcncc.a'])
 sdkroot=(root/'liblua').resolve();sources={q:want for q,want in idx['source_sha256'].items() if sdkroot in Path(q).resolve().parents};assert len(sources)==28
 for q,want in sources.items():assert h(q)==want,q
 ciq=v/(prefix+'-PROOF_INDEX.json');ci=load(ciq);assert ci['status']=='passed' and len(ci['receipts'])==8 and h(ci['sdk_reuse_receipt'])==ci['sdk_reuse_receipt_sha256'];proofs.append(ciq)
 assert {Path(r['path']).name for r in ci['receipts']}=={prefix+'-'+n+'.receipt.json' for n in names}
 for row in ci['receipts']:assert h(row['path'])==row['sha256']
 m=load(pkg/'MANIFEST.json');assert len(m['sdk_expected_files'])==59
 for row in m['sdk_expected_files']:assert h(root/row['path'])==row['sha256']
 reuse_reviews[stem]={'reuse':reuse,'SDK_only_source_sha256':sources}
prov=load(w/'quest_lua_queries/sdk-provenance.json');assert len(prov['files'])==58
for row in prov['files']:assert h(root/'liblua'/row['path'])==h(row['reference'])==row['sha256']
plan=load(p/'integration/plan.json');review=load(p/'source-guards.json');assert review.get('review') and plan['runtime_count_after']==112
assert len(plan['files'])==len({norm(r['path']).lower() for r in plan['files']})==10
guards={norm(k):v for k,v in review['guards'].items()};support={norm(k):v for k,v in review['support_read_guards'].items()}
assert guards=={norm(r['path']):r['after'] for r in plan['files']} and support=={norm(r['path']):r['sha256'] for r in plan['dependencies']} and len(support)==70
for rel,want in {**guards,**support}.items():assert h(root/rel)==want,rel
for row in plan['files']:assert h(p/'integration/payload'/norm(row['path']))==row['after']
for source,want in plan['parent_plan_sha256'].items():assert h(source)==want,source
changes={norm(k):v for k,v in review.get('reviewed_preparation_overrides',{}).items()};evidence={}
for pkg in pkgs:
 m=load(pkg/'MANIFEST.json');index=m['package_files']
 if isinstance(index,list):index={r['path']:r['sha256'] for r in index}
 for rel,want in index.items():
  q=pkg/rel;key=norm(q.relative_to(w.parent));actual=h(q);immutable=bool(re.match(r'^0[0-9a-f]{7}-',q.name)) or q.name.startswith(('ELF_OWNER','original-')) or q.name.endswith('.fragment')
  if actual!=want:assert not immutable and changes.get(key)==actual,(key,'unreviewed preparation change')
  evidence[key]=actual
 for q in [pkg/'MANIFEST.json',pkg/'integration/plan.json']:evidence[norm(q.relative_to(w.parent))]=h(q)
 for name in ['questmanager.cpp.fragment','questlua.cpp.fragment']:
  q=pkg/name
  if q.exists():assert q.read_text(encoding='utf-8-sig').strip() in (root/'game/src'/name.replace('.fragment','')).read_text(encoding='utf-8'),'Whole source fragment absent'
for rel,want in changes.items():assert h(w.parent/rel)==want,rel
merged=load(rec/'function-coverage-overrides.json');sha=merged['target_sha256'];assert all(load(pkg/'MANIFEST.json')['target_sha256']==sha for pkg in pkgs)
functions=[];spec=[(pkgs[0],'0805cc90',36,'questlua.cpp'),(pkgs[0],'0805cc10',36,'questlua.cpp'),(pkgs[0],'0805cb90',36,'questlua.cpp'),(pkgs[0],'0805c1d0',270,'questlua.cpp'),(pkgs[0],'081a7bb0',53,'questmanager.cpp'),(pkgs[1],'081a4c20',71,'questmanager.cpp'),(pkgs[1],'081a7a20',120,'questmanager.cpp')]
for pkg,a,n,owner in spec:
 d=load(pkg/(a+'-decompile.json'));assert d['function']['address']==a and d.get('code') and not d.get('truncated');functions.append(d)
 asm=load(pkg/(a+'-assembly.json'));assert asm['has_more'] is False and len(asm['items'])==asm['total']==n
owner_audit=load(pkgs[0]/'ELF_OWNER_API_AUDIT.json')

def ref(path,anchor):
 q=root/path;t=q.read_text(encoding='utf-8');assert anchor in t,(path,anchor)
 return dict(path=path,anchor=anchor,line=t[:t.index(anchor)].count('\n')+1,file_sha256=h(q))
limits='Whole appmethods preserve original unsafe/null/lifetime/invalidated iterator/VM/format/out-of-range/concurrent domains; CanStartQuest present-empty vector &v[0] is original C++03 unsafe and excluded, no empty repair/status handling. Actual supplied SDK5.0.3 aux_do consumes errors through _ALERT; original app ignores lua_dobuffer status, then uses current top truthiness and restores savedtop. ActualpublicopaqueSDK compatibility only, not exact original5.0/internalABI (96/16 vs92/12). SDK28objects/twoarchives reusedstrictSHA/all59guards, not rebuilt by99; oldappsource hashes excluded onlySDKreuse. Prior OpenState and Build matching remain static-only; full Confirm/RunState/VM/Reward/EndRunning/server/VisualStudio remain pending.'
scopes={'0805cc90':'Whole private CQM Pause: suspend1 before actual[NEXT] append/SendScript; actualstate/network.', '0805cc10':'Whole private CQM Input: suspend3 before[INPUT] append/SendScript.', '0805cb90':'Whole private CQM SelectItem: suspend5 before[SELECT_ITEM] append/SendScript.', '0805c1d0':'Whole private CQM Select: actualSDK table length/args short/traversal/string conversions, original lua_typename(co,-1) role/diagnostics, actualscript append/suspend2/debug/SendScript; defined SDK/table domains only.', '081a7bb0':'Whole public vararg CQM QuestError: bounded vsnprintf4096 then error sink, fresh server/CHAR lookup and cached CHAR for both messages. Does not set/reset error or script/state flags; no RunState or Confirm replacement.', '081a4c20':'Whole unsigned-index CanStartQuest: missing-map true without L; actualgettop/dobuffer ignoredstatus/topboolean/restore. ActualSDKaux_do error consumption/current prefixtruthiness retained; emptyvector unsafe excluded.', '081a7a20':'Whole WriteRunningStateToSyserr: reverse actualLuaquery oldstate/context then event-map lookup and fresh quest/CHAR/test_server rereads, realdiagnostic+ChatPacket; borrowedstate pointer raw.'}
for (pkg,a,n,owner),d in zip(spec,functions):
 name=d['function']['name'].split('::')[-1];r={'address':a,'source_refs':[ref('game/src/'+owner,'CQuestManager::'+name+'(')],'test_refs':[ref('reconstruction/tests/'+pkg.name+'_regression.cpp','int main(')],'review_scope':scopes[a],'reviewer':'Parent checkpoint99 exactowner/fullinstructions/API/callee review +focused4/two strictO2 actualgraphs/twoSDKreuse indexes/full112+ABI','limitations':limits}
 prior=next((x for x in merged['reviews'] if x['address']==a),None)
 if prior is None:merged['reviews'].append(r)
 else:prior.clear();prior.update(r)
status=load(rec/'validation-status.json');assert status['checkpoint98_quest_state_lifecycle']['status']=='passed'
note='Checkpoint99 restores7whole originalCQM methods:4privatequestlua yield bridges, publicQuestError and manager CanStartQuest/WriteRunningStateToSyserr. Focused4+ABI, both strict32/newO2SQL+actualapp graphs with actualSDKreuse9proofseach, full112 ordered runtime+ABI pass; no full111repeat. CanStart missingtrue/ignoredstatus/topbool/restore and actualSDKaux_do errorconsumption retained. '+limits
manifest={'checkpoint':99,'status':'passed','full_runtime_commands':112,'focused_runtime_commands':4,'abi':True,'standalone_function_mappings':7,'inline_semantic_bodies':0,'source_guards':guards,'support_read_guards':support,'source_guard_review':review,'sdk_reuse':{'compiled_by_this_group':False,'objects_perSDK':28,'archives':2,'all_expected_files':59,'old_application_hash_ignored_only_for_SDK_reuse':True},'proof_sha256':{str(q.resolve()):h(q) for q in proofs},'original_evidence_sha256':evidence,'input_sha256':{str(q.resolve()):h(q) for q in [p/'integration/plan.json',p/'source-guards.json',Path(__file__)]},'baseline_metadata_sha256':{name:h(rec/name) for name in ['function-coverage-overrides.json','validation-status.json','CURRENT_STATUS.md','RESUME_CHECKPOINT.md']},'baseline_overrides_sha256':h(rec/'function-coverage-overrides.json'),'baseline_validation_status_sha256':h(rec/'validation-status.json'),'note':note}
status.update(date=full['finished_utc'][:10],runtime_suites=full['suites'],runtime_suite_count=112,runtime_commands_verified=full['executed_runtime_commands'],runtime_command_count_verified=112,combined_make_check_exit_code=0,make_check_abi_exit_code=0,latest_full_check_exit_code=0,latest_check_abi_exit_code=0,full_validation_log=full['log'],latest_validation_receipt=str((w/'checkpoint-99-validation.receipt.json').resolve()));status['checkpoint99_quest_control_group']=manifest
out.mkdir(exist_ok=True)
def write(name,data):(out/name).write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')
for name,data in [('RECORD_MANIFEST.json',manifest),('validation-status.json',status),('function-coverage-overrides.json',merged),('quest-control-group-ghidra.json',{'target_sha256':sha,'functions':functions}),('quest-control-owner-sdk-review.json',{'target_sha256':sha,'yield_owner_API_audit':owner_audit,'sdk_reuse':reuse_reviews,'policy':limits})]:write(name,data)
(out/'quest-control-group-recovery.md').write_text('# Quest control grouped checkpoint99\n\n'+note+'\n',encoding='utf-8');print('Prepared workspace-only99:7wholemaps/full112+ABI, actualSDKreuse not recompilation. No project writes.')
