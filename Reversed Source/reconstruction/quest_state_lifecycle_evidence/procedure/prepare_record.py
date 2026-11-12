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
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';pkg=w/'quest_state_lifecycle';out=p/'prepared'
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
old=passed(w/'checkpoint-97-validation.receipt.json');full=passed(w/'checkpoint-98-validation.receipt.json');focused=passed(w/'quest-state-lifecycle-focused.receipt.json')
assert old['scope']=='full' and len(old['suites'])==109 and old['suites']==old['executed_runtime_commands']
for d in [full,focused]:
 assert d['make_exit_code']==0 and d['abi_requested'] and 'check-abi' in d['command']
 assert d['suites']==d['executed_runtime_commands'] and len(d['suites'])==len(set(d['suites']))
assert full['scope']=='full' and len(full['suites'])==110 and full['suites'].count('quest_state_lifecycle_regression')==1
assert [s for s in full['suites'] if s!='quest_state_lifecycle_regression']==old['suites']
assert focused['scope']=='focused' and set(focused['suites'])=={'quest_state_lifecycle_regression','quest_lua_queries_regression','pc_state_consumers_regression','pc_state_helpers_regression'}
v=rec/'tests/quest_state_lifecycle_validation';names=['strict32','item_sql_runtime-o2','item_sql_semaphore-o2','item_sql_api-o2','item_socket_config-o2','quest_state_lifecycle_desc_output_syscalls-o2','o2-build','o2-runtime'];ds={n:passed(v/('quest-state-lifecycle-'+n+'.receipt.json')) for n in names}
assert all(x in ds['strict32']['command'] for x in ['-std=c++03','-pedantic-errors','-m32','-fsyntax-only','quest_state_lifecycle_abi.cpp','quest_state_lifecycle_regression.cpp','../../game/src/questmanager.cpp','../../game/src/questlua.cpp'])
command=ds['o2-build']['command'];assert '-O2' in command and '-DMCNCC_TEST_REAL_EVENTS' in command and '-flto' not in command
for name in ['questlua.cpp','event.cpp','event_queue.cpp','questpc.cpp','questmanager.cpp','char.cpp','desc.cpp','buffer_manager.cpp']:assert '../../game/src/'+name in command,name
assert 'buffer_manager_core.o' in command
objects=[]
for n in names[1:6]:
 c=ds[n]['command'];assert '-O2' in c and c[-2]=='-o';objects.append(c[-1]);assert c[-1] in command
assert len(set(objects))==5 and ds['o2-runtime']['command']==[command[-1]]
# Explicit SDK reuse: do not treat old query proof's application source hash as current PC guard.
reusepath=v/'quest-state-lifecycle-sdk-reuse.receipt.json';reuse=load(reusepath);assert reuse['status']=='passed' and reuse['sdk_source_count']==28 and reuse['sdk_source_header_guards']==59;proofs.append(reusepath)
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
consumerindexpath=v/'quest-state-lifecycle-PROOF_INDEX.json';ci=load(consumerindexpath);assert ci['status']=='passed' and len(ci['receipts'])==8 and h(ci['sdk_reuse_receipt'])==ci['sdk_reuse_receipt_sha256'];proofs.append(consumerindexpath)
assert {Path(r['path']).name for r in ci['receipts']}=={'quest-state-lifecycle-'+n+'.receipt.json' for n in names}
for row in ci['receipts']:assert h(row['path'])==row['sha256']
m0=load(pkg/'MANIFEST.json');assert len(m0['sdk_expected_files'])==59
for row in m0['sdk_expected_files']:assert h(root/row['path'])==row['sha256'],row
prov=load(w/'quest_lua_queries/sdk-provenance.json');assert len(prov['files'])==58
for row in prov['files']:assert h(root/'liblua'/row['path'])==h(row['reference'])==row['sha256'],row
plan=load(pkg/'integration/plan.json');review=load(p/'source-guards.json');assert review.get('review') and plan['runtime_count_after']==110
assert len(plan['files'])==len({r['path'].replace('\\','/').lower() for r in plan['files']})==8
assert review['guards']=={r['path']:r['after'] for r in plan['files']}
assert review['support_read_guards']=={r['path']:r['sha256'] for r in plan['dependencies']} and len(review['support_read_guards'])==66
for rel,want in {**review['guards'],**review['support_read_guards']}.items():assert h(root/rel)==want,rel
for row in plan['files']:assert h(pkg/'integration/payload'/row['path'])==row['after'],row
changes=review.get('reviewed_preparation_overrides',{});evidence={}
for rel,want in m0['package_files'].items():
 q=pkg/rel;key=str(q.relative_to(w.parent)).replace('\\','/');actual=h(q);immutable=bool(re.match(r'^0[0-9a-f]{7}-',q.name)) or q.name in ['ELF_OWNER_AUDIT.json','original-api-packet-dwarf.json','questlua.cpp.candidate','questmanager.cpp.fragment','questmanager.h.private.fragment','questmanager.h.public.fragment','packet.h.fragment']
 if actual!=want:assert not immutable and changes.get(key)==actual,(key,'unreviewed preparation change')
 evidence[key]=actual
for rel,want in changes.items():assert h(w.parent/rel)==want,rel
assert (pkg/'questmanager.cpp.fragment').read_text(encoding='utf-8-sig').strip() in (root/'game/src/questmanager.cpp').read_text(encoding='utf-8')
assert h(pkg/'questlua.cpp.candidate')==h(root/'game/src/questlua.cpp'),'Whole original-owner new source differs frozen candidate'
audit=load(pkg/'ELF_OWNER_AUDIT.json');owners={x['address']:x for x in audit['symbols']};dies={x['low_pc']:x for x in audit['dies'] if x.get('low_pc')}
for a in ['0805cd80','0805d000','0805cd10','081a4ee0','081a4f30','081a4f50']:
 assert a in owners and owners[a]['name'].startswith('_ZN5quest13CQuestManager'),a
 assert a in dies and dies[a]['decl_file']==('questlua.cpp' if a in ['0805cd80','0805d000','0805cd10'] else 'questmanager.cpp'),a
 assert any(x.get('name')=='this' and x.get('artificial') for x in dies[a]['parameters']),a
merged=load(rec/'function-coverage-overrides.json');sha=merged['target_sha256'];assert m0['target_sha256']==sha
functions=[]
for address,n in [('0805cd80',28),('0805d000',22),('0805cd10',34),('081a4ee0',21),('081a4f30',7),('081a4f50',122)]:
 d=load(pkg/(address+'-decompile.json'));assert d['function']['address']==address and d.get('code') and not d.get('truncated');functions.append(d)
 asm=load(pkg/(address+'-assembly.json'));assert asm['has_more'] is False and len(asm['items'])==asm['total']==n

def ref(path,anchor):
 q=root/path;text=q.read_text(encoding='utf-8');assert anchor in text,(path,anchor)
 return dict(path=path,anchor=anchor,line=text[:text.index(anchor)].count('\n')+1,file_sha256=h(q))
limits='OpenState runtime STATIC-ONLY: original sret directly constructs returnstorage; named-local C++ source may copy indeterminate default QuestState counters if NRVO omitted. Full original method retained, no zero repair/copyctor/API change; manual actual coroutine setup tests CloseState only and is not OpenState replacement/proof. Unsuppressed empty &script[0] C++03 unsafe domain excluded; no c_str repair. Native old COW/unshare/allocation/invalidcontext/stringborrowed/registry-corruption/format callback domains remain original. Actual SDK5.0.3 external publicAPI compatible only, not exact original5.0/internalABI (96/16 vs92/12);28SDKobjects/2archives reused by strictSDKsubset/all59fileSHA, not rebuilt by98. No fake VM/startup/RunState/Reward/EndRunning/server/VisualStudio equivalence.'
scopes={'0805cd80':'Whole original CQM OpenState in questlua.cpp: real defaultstate, args0/stinput, currentL newthread then freshcurrentL registryref, returnbyvalue. STATICONLY original direct-sret/NRVO limitation; no runtimepass claim.','0805d000':'Whole original CQM CloseState in questlua.cpp: co guard, actual currentownerL registryunref thencoNULL only; actualSDK registration/unref/idreuse/currentL/no-ref/allfields runtime.','0805cd10':'Whole PRIVATE CQM GotoEndState in questlua.cpp: ignorestate, temporary[DONE] append/destroy then actualSendScript, no coroutineclose/state/VM execution.','081a4ee0':'Whole manager ClearScript: clearcomplete string before skin1, unrelatedPCdone/drop/error/context preserved.','081a4f30':'Whole manager AddScript complete append inclbinary/selfalias under actualSDK native string semantics.','081a4f50':'Whole manager SendScript: all240drop/DONE/NEXT/PCdone/skin/error controls, originalcaptured6-byteheader/WORDwrap/freshfullpayload/callbacklatecontext/testserver/debug/finalclear/poolorder; unsuppressedempty raw&str[0] excluded.'}
for d in functions:
 a=d['function']['address'];name=d['function']['name'].split('::')[-1];owner='game/src/questlua.cpp' if a in ['0805cd80','0805d000','0805cd10'] else 'game/src/questmanager.cpp'
 r={'address':a,'source_refs':[ref(owner,'CQuestManager::'+name+'(')],'test_refs':[ref('reconstruction/tests/quest_state_lifecycle_regression.cpp','int main(')],'review_scope':scopes[a],'reviewer':'Parent checkpoint98 rawELFowner/artificialthis/exactCU + wholeinstruction/data review;focused4/strict32/newO2app+SQL/verifiedSDKreuse/full110+ABI; OpenStaticOnly','limitations':limits,'runtime_scope':'static_only_original_sret_NRVO_indeterminate_copy' if a=='0805cd80' else 'Bounded actualSDK/PC/network cases, original unsafe domains excluded'}
 prior=next((x for x in merged['reviews'] if x['address']==a),None)
 if prior is None:merged['reviews'].append(r)
 else:prior.clear();prior.update(r)
status=load(rec/'validation-status.json');assert status['checkpoint97_pc_state_consumers']['status']=='passed'
note='Checkpoint98 reconstructs6whole original CQM methods in exactoriginal owners: questlua.cpp OpenState/CloseState/privateGotoEndState; manager.cpp ClearScript/AddScript/SendScript, full packed6-byte packet_script. RawELF mangledsymbols/lowPC/CU/artificialthis audit disproves freefunction/address-neighborhood guesses. Focused4+ABI, strict32,5newO2SQL/observer+actualO2app and9helperproofs inclSDKreuse/index; full110 exactordered runtimes+ABI pass. Five methods dynamicallytested; OpenSTATICONLY. '+limits
manifest={'checkpoint':98,'status':'passed','full_runtime_commands':110,'focused_runtime_commands':4,'abi':True,'standalone_function_mappings':6,'inline_semantic_bodies':0,'typed_packet_size':6,'runtime_scope':{'OpenState':'static_only_original_sret_NRVO_indeterminate_copy','other5':'bounded actualSDK/script control matrix/network','unsuppressed_empty_script':'unsafe_original_cpp03_excluded'},'source_guards':review['guards'],'support_read_guards':review['support_read_guards'],'source_guard_review':review,'sdk_reuse':{'compiled_by_this_group':False,'sdk_objects':28,'archives':2,'all_expected_files':59,'original_revision_recovered':False,'old_application_hash_ignored_only_for_SDK_reuse':True},'proof_sha256':{str(q.resolve()):h(q) for q in proofs},'original_evidence_sha256':evidence,'input_sha256':{str(q.resolve()):h(q) for q in [pkg/'integration/plan.json',pkg/'MANIFEST.json',pkg/'ELF_OWNER_AUDIT.json',p/'source-guards.json',Path(__file__)]},'baseline_metadata_sha256':{name:h(rec/name) for name in ['function-coverage-overrides.json','validation-status.json','CURRENT_STATUS.md','RESUME_CHECKPOINT.md']},'baseline_overrides_sha256':h(rec/'function-coverage-overrides.json'),'baseline_validation_status_sha256':h(rec/'validation-status.json'),'note':note}
status.update(date=full['finished_utc'][:10],runtime_suites=full['suites'],runtime_suite_count=110,runtime_commands_verified=full['executed_runtime_commands'],runtime_command_count_verified=110,combined_make_check_exit_code=0,make_check_abi_exit_code=0,latest_full_check_exit_code=0,latest_check_abi_exit_code=0,full_validation_log=full['log'],latest_validation_receipt=str((w/'checkpoint-98-validation.receipt.json').resolve()));status['checkpoint98_quest_state_lifecycle']=manifest
out.mkdir(exist_ok=True)
def write(name,data):(out/name).write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')
for name,data in [('RECORD_MANIFEST.json',manifest),('validation-status.json',status),('function-coverage-overrides.json',merged),('quest-state-lifecycle-ghidra.json',{'target_sha256':sha,'functions':functions}),('quest-state-lifecycle-owner-review.json',{'target_sha256':sha,'owner_audit':audit,'packet_ref':ref('game/src/packet.h','struct packet_script'),'packet_dwarf':load(pkg/'original-api-packet-dwarf.json'),'sdk_reuse':reuse,'sdk_subset_source_sha256':sdksources,'policy':limits})]:write(name,data)
(out/'quest-state-lifecycle-recovery.md').write_text('# CQM state lifecycle/scripts checkpoint98\n\n'+note+'\n',encoding='utf-8');print('Prepared workspace-only98:6wholeCQMmaps/full110+ABI, exactquestluaowner, OpenSTATICONLY. No project writes.')
