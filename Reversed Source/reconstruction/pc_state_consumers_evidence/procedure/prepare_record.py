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
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';pkg=w/'pc_state_consumers';out=p/'prepared'
load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'))
h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest()
proofs=[]
def passed(q):
 d=load(q);assert d.get('status')=='passed' and d.get('exit_code')==0 and d.get('finished_utc'),q;proofs.append(q)
 if d.get('log'):
  log=Path(d['log']);assert log.is_file(),log
  if d.get('log_sha256'):assert h(log)==d['log_sha256'],log
  proofs.append(log)
 return d
old=passed(w/'checkpoint-96-validation.receipt.json');full=passed(w/'checkpoint-97-validation.receipt.json');focused=passed(w/'pc-state-consumers-focused.receipt.json')
assert old['scope']=='full' and len(old['suites'])==108 and old['suites']==old['executed_runtime_commands']
for d in [full,focused]:
 assert d['make_exit_code']==0 and d['abi_requested'] and 'check-abi' in d['command']
 assert d['suites']==d['executed_runtime_commands'] and len(d['suites'])==len(set(d['suites']))
assert full['scope']=='full' and len(full['suites'])==109 and full['suites'].count('pc_state_consumers_regression')==1
assert [s for s in full['suites'] if s!='pc_state_consumers_regression']==old['suites']
assert focused['scope']=='focused' and set(focused['suites'])=={'pc_state_consumers_regression','quest_lua_queries_regression','quest_event_flags_regression','pc_quest_storage_regression'}
v=rec/'tests/pc_state_consumers_validation';names=['strict32','item_sql_runtime-o2','item_sql_semaphore-o2','item_sql_api-o2','item_socket_config-o2','pc_state_consumers_desc_output_syscalls-o2','o2-build','o2-runtime'];ds={n:passed(v/('pc-state-consumers-'+n+'.receipt.json')) for n in names}
assert all(x in ds['strict32']['command'] for x in ['-std=c++03','-pedantic-errors','-m32','-fsyntax-only','pc_state_consumers_abi.cpp','pc_state_consumers_regression.cpp','../../game/src/questpc.cpp'])
command=ds['o2-build']['command'];assert '-O2' in command and '-DMCNCC_TEST_REAL_EVENTS' in command and '-flto' not in command
for name in ['event.cpp','event_queue.cpp','questpc.cpp','questmanager.cpp','char.cpp','desc.cpp','buffer_manager.cpp']:assert '../../game/src/'+name in command,name
assert 'buffer_manager_core.o' in command
objects=[]
for n in names[1:6]:
 c=ds[n]['command'];assert '-O2' in c and c[-2]=='-o';objects.append(c[-1]);assert c[-1] in command
assert len(set(objects))==5 and ds['o2-runtime']['command']==[command[-1]]
# Explicit SDK reuse: do not treat old query proof's application source hash as current PC guard.
reusepath=v/'pc-state-consumers-sdk-reuse.receipt.json';reuse=load(reusepath);assert reuse['status']=='passed' and reuse['sdk_source_count']==28 and reuse['sdk_source_header_guards']==59;proofs.append(reusepath)
sdkindex=Path(reuse['proof_index']);idx=load(sdkindex);assert h(sdkindex)==reuse['proof_index_sha256'] and idx['status']=='passed' and idx['sdk_C_objects']==28 and len(idx['receipts'])==39;proofs.append(sdkindex)
for row in idx['receipts']:
 q=Path(row['path']);assert h(q)==row['sha256'];passed(q)
for key,count in [('sdk_objects',28),('sdk_archives',2)]:
 assert len(idx[key])==count and len({r['path'] for r in idx[key]})==count and reuse[key]==idx[key]
 for row in idx[key]:assert h(row['path'])==row['sha256'],row
assert all(row['path'] in command for row in idx['sdk_archives'])
assert not any(x in command for x in ['../../liblua/liblualib_mcncc.a','../../liblua/liblua_mcncc.a'])
sdkroot=(root/'liblua').resolve();sdksources={q:want for q,want in idx['source_sha256'].items() if sdkroot in Path(q).resolve().parents};assert len(sdksources)==28
for q,want in sdksources.items():assert h(q)==want,q
consumerindexpath=v/'pc-state-consumers-PROOF_INDEX.json';ci=load(consumerindexpath);assert ci['status']=='passed' and len(ci['receipts'])==8 and h(ci['sdk_reuse_receipt'])==ci['sdk_reuse_receipt_sha256'];proofs.append(consumerindexpath)
assert {Path(r['path']).name for r in ci['receipts']}=={'pc-state-consumers-'+n+'.receipt.json' for n in names}
for row in ci['receipts']:assert h(row['path'])==row['sha256']
m0=load(pkg/'MANIFEST.json');assert len(m0['sdk_expected_files'])==59
for row in m0['sdk_expected_files']:assert h(root/row['path'])==row['sha256'],row
prov=load(w/'quest_lua_queries/sdk-provenance.json');assert len(prov['files'])==58
for row in prov['files']:assert h(root/'liblua'/row['path'])==h(row['reference'])==row['sha256'],row
plan=load(pkg/'integration/plan.json');review=load(p/'source-guards.json');assert review.get('review') and plan['runtime_count_after']==109
assert len(plan['files'])==len({r['path'].replace('\\','/').lower() for r in plan['files']})==5
assert review['guards']=={r['path']:r['after'] for r in plan['files']}
assert review['support_read_guards']=={r['path']:r['sha256'] for r in plan['dependencies']}
for rel,want in {**review['guards'],**review['support_read_guards']}.items():assert h(root/rel)==want,rel
for row in plan['files']:assert h(pkg/'integration/payload'/row['path'])==row['after'],row
changes=review.get('reviewed_preparation_overrides',{});evidence={}
for rel,want in m0['package_files'].items():
 q=pkg/rel;key=str(q.relative_to(w.parent)).replace('\\','/');actual=h(q);immutable=bool(re.match(r'^0[0-9a-f]{7}-',q.name)) or q.name in ['original-full-PC-api.json','questpc.cpp.fragment']
 if actual!=want:assert not immutable and changes.get(key)==actual,(key,'unreviewed preparation change')
 evidence[key]=actual
for rel,want in changes.items():assert h(w.parent/rel)==want,rel
assert (pkg/'questpc.cpp.fragment').read_text(encoding='utf-8-sig').strip() in (root/'game/src/questpc.cpp').read_text(encoding='utf-8')
merged=load(rec/'function-coverage-overrides.json');sha=merged['target_sha256'];assert m0['target_sha256']==sha
functions=[]
for address,n in [('0805e2a0',61),('0805e380',35),('08061980',121)]:
 d=load(pkg/(address+'-decompile.json'));assert d['function']['address']==address and d.get('code') and not d.get('truncated');functions.append(d)
 asm=load(pkg/(address+'-assembly.json'));assert asm['has_more'] is False and len(asm['items'])==asm['total']==n

def ref(path,anchor):
 q=root/path;text=q.read_text(encoding='utf-8');assert anchor in text,(path,anchor)
 return dict(path=path,anchor=anchor,line=text[:text.index(anchor)].count('\n')+1,file_sha256=h(q))
limits='Actual supplied SDK5.0.3 public opaque API compatibility only, not exact original5.0 revision/internal ABI: SDK MinGW32 lua_State96/TObject16 differ original92/12; Number8/CallInfo24 match. SDK28objects/twoarchives reused by strictSHA, not rebuilt by this module; only SDKsubset of old query source index applies, old PCapplication hash deliberately excluded. Raw borrowed Lua string/GC lifetime, null/invalidLua/format/iterator invalidation, nonfinite or out-of-range conversion domains preserved and not forced. No defaultQuestState indeterminate copies in tests. Full VM/Reward/EndRunning/server/VisualStudio unresolved; prior Build matching remains static-only.'
anchors={'0805e2a0':'PC::SetCurrentQuestStateName(','0805e380':'PC::SetQuestState(const std::string& quest_name,const std::string& state_name)','08061980':'PC::SendFlagList('}
scopes={'0805e2a0':'Whole query-first setter, current questname reread after actual Lua callback, actual SetFlag/DeleteFlag/SaveFlag; running state unchanged.','0805e380':'Whole string/string overload: actual Lua query first then real integer overload/transition queue with live aliased questname and cached index; no constchar overload claim.','08061980':'Whole live sorted flag display including actual reverse Lua query for status suffix, copied prefix, raw borrowed name and fresh numeric value after callback; real ChatPacket/DESC network.'}
for d in functions:
 a=d['function']['address'];r={'address':a,'source_refs':[ref('game/src/questpc.cpp',anchors[a])],'test_refs':[ref('reconstruction/tests/pc_state_consumers_regression.cpp','int main(')],'review_scope':scopes[a],'reviewer':'Parent checkpoint97 whole instruction/callee review + focused4/strict32/newO2app+SQL/verifiedSDKreuse/full109+ABI','limitations':limits}
 prior=next((x for x in merged['reviews'] if x['address']==a),None)
 if prior is None:merged['reviews'].append(r)
 else:prior.clear();prior.update(r)
status=load(rec/'validation-status.json');assert status['checkpoint96_quest_manager_group']['status']=='passed'
note='Checkpoint97 reconstructs whole PC SetCurrentQuestStateName0805e2a0/string-state SetQuestState0805e380/SendFlagList08061980 using actual Lua-backed manager queries and actual flag/statequeue/network graph. Focused4+ABI, strict32,5new distinctO2SQL/observer objects+actualO2app and9helperreceipts including verifiedSDKreuse/sealedindex, full109 exactordered runtimes+ABI pass. Original callback/alias/borrowedstring/freshflag ordering retained; no newtypes/API or fakeVM. '+limits
manifest={'checkpoint':97,'status':'passed','full_runtime_commands':109,'focused_runtime_commands':4,'abi':True,'standalone_function_mappings':3,'inline_semantic_bodies':0,'source_guards':review['guards'],'support_read_guards':review['support_read_guards'],'source_guard_review':review,'sdk_reuse':{'compiled_by_this_group':False,'sdk_objects':28,'archives':2,'all_expected_files':59,'original_revision_recovered':False,'old_PC_hash_ignored_only_for_SDK_reuse':True},'proof_sha256':{str(q.resolve()):h(q) for q in proofs},'original_evidence_sha256':evidence,'input_sha256':{str(q.resolve()):h(q) for q in [pkg/'integration/plan.json',pkg/'MANIFEST.json',p/'source-guards.json',Path(__file__)]},'baseline_metadata_sha256':{name:h(rec/name) for name in ['function-coverage-overrides.json','validation-status.json','CURRENT_STATUS.md','RESUME_CHECKPOINT.md']},'baseline_overrides_sha256':h(rec/'function-coverage-overrides.json'),'baseline_validation_status_sha256':h(rec/'validation-status.json'),'note':note}
status.update(date=full['finished_utc'][:10],runtime_suites=full['suites'],runtime_suite_count=109,runtime_commands_verified=full['executed_runtime_commands'],runtime_command_count_verified=109,combined_make_check_exit_code=0,make_check_abi_exit_code=0,latest_full_check_exit_code=0,latest_check_abi_exit_code=0,full_validation_log=full['log'],latest_validation_receipt=str((w/'checkpoint-97-validation.receipt.json').resolve()));status['checkpoint97_pc_state_consumers']=manifest
out.mkdir(exist_ok=True)
def write(name,data):(out/name).write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')
for name,data in [('RECORD_MANIFEST.json',manifest),('validation-status.json',status),('function-coverage-overrides.json',merged),('pc-state-consumers-ghidra.json',{'target_sha256':sha,'functions':functions}),('pc-state-consumers-sdk-reuse-review.json',{'target_sha256':sha,'reuse':reuse,'sdk_subset_source_sha256':sdksources,'policy':limits})]:write(name,data)
(out/'pc-state-consumers-recovery.md').write_text('# PC state consumers checkpoint97\n\n'+note+'\n',encoding='utf-8');print('Prepared workspace-only97:3wholemaps/full109+ABI, verifiedactualSDKreuse not recompilation. No project writes.')
