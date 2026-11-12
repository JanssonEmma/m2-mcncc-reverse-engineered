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
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';pkg=w/'quest_confirm_leaves';out=p/'prepared'
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
old=passed(w/'checkpoint-99-validation.receipt.json');full=passed(w/'checkpoint-100-validation.receipt.json');focused=passed(w/'quest-confirm-focused.receipt.json')
assert old['scope']=='full' and len(old['suites'])==112 and old['suites']==old['executed_runtime_commands']
for d in [full,focused]:
 assert d['make_exit_code']==0 and d['abi_requested'] and 'check-abi' in d['command']
 assert d['suites']==d['executed_runtime_commands'] and len(d['suites'])==len(set(d['suites']))
assert full['scope']=='full' and len(full['suites'])==113 and full['suites'].count('quest_confirm_leaves_regression')==1
assert [s for s in full['suites'] if s!='quest_confirm_leaves_regression']==old['suites']
assert focused['scope']=='focused' and set(focused['suites'])=={'quest_confirm_leaves_regression','quest_state_lifecycle_regression','quest_yield_bridges_regression','quest_predicate_diagnostics_regression'}
v=rec/'tests/quest_confirm_leaves_validation';names=['strict32','item_sql_runtime-o2','item_sql_semaphore-o2','item_sql_api-o2','item_socket_config-o2','quest_confirm_leaves_desc_output_syscalls-o2','o2-build','o2-runtime'];ds={n:passed(v/('quest-confirm-leaves-'+n+'.receipt.json')) for n in names}
assert all(x in ds['strict32']['command'] for x in ['-std=c++03','-pedantic-errors','-m32','-fsyntax-only','quest_confirm_leaves_abi.cpp','quest_confirm_leaves_regression.cpp','../../game/src/questmanager.cpp','../../game/src/char.cpp'])
command=ds['o2-build']['command'];assert '-O2' in command and '-DMCNCC_TEST_REAL_EVENTS' in command and '-flto' not in command
for name in ['event.cpp','event_queue.cpp','questpc.cpp','questmanager.cpp','char.cpp','desc.cpp','buffer_manager.cpp']:assert '../../game/src/'+name in command,name
assert 'buffer_manager_core.o' in command
objects=[]
for n in names[1:6]:
 c=ds[n]['command'];assert '-O2' in c and c[-2]=='-o';objects.append(c[-1]);assert c[-1] in command
assert len(set(objects))==5 and ds['o2-runtime']['command']==[command[-1]]
# Explicit SDK reuse: do not treat old query proof's application source hash as current PC guard.
reusepath=v/'quest-confirm-leaves-sdk-reuse.receipt.json';reuse=load(reusepath);assert reuse['status']=='passed' and reuse['sdk_source_count']==28 and reuse['sdk_source_header_guards']==59;proofs.append(reusepath)
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
consumerindexpath=v/'quest-confirm-leaves-PROOF_INDEX.json';ci=load(consumerindexpath);assert ci['status']=='passed' and len(ci['receipts'])==8 and h(ci['sdk_reuse_receipt'])==ci['sdk_reuse_receipt_sha256'];proofs.append(consumerindexpath)
assert {Path(r['path']).name for r in ci['receipts']}=={'quest-confirm-leaves-'+n+'.receipt.json' for n in names}
for row in ci['receipts']:assert h(row['path'])==row['sha256']
m0=load(pkg/'MANIFEST.json');assert len(m0['sdk_expected_files'])==59
for row in m0['sdk_expected_files']:assert h(root/row['path'])==row['sha256'],row
prov=load(w/'quest_lua_queries/sdk-provenance.json');assert len(prov['files'])==58
for row in prov['files']:assert h(root/'liblua'/row['path'])==h(row['reference'])==row['sha256'],row
plan=load(pkg/'integration/plan.json');review=load(p/'source-guards.json');assert review.get('review') and plan['runtime_count_after']==113
assert len(plan['files'])==len({r['path'].replace('\\','/').lower() for r in plan['files']})==9
assert review['guards']=={r['path']:r['after'] for r in plan['files']}
assert review['support_read_guards']=={r['path']:r['sha256'] for r in plan['dependencies']} and len(review['support_read_guards'])==69
for rel,want in {**review['guards'],**review['support_read_guards']}.items():assert h(root/rel)==want,rel
for row in plan['files']:assert h(pkg/'integration/payload'/row['path'])==row['after'],row
changes=review.get('reviewed_preparation_overrides',{});evidence={}
for rel,want in m0['package_files'].items():
 q=pkg/rel;key=str(q.relative_to(w.parent)).replace('\\','/');actual=h(q);immutable=bool(re.match(r'^0[0-9a-f]{7}-',q.name)) or q.name in ['ELF_API_PACKET_AUDIT.json','char.cpp.fragment','char.h.fragment','questmanager.cpp.fragment','questmanager.h.fragment','packet.h.fragment']
 if actual!=want:assert not immutable and changes.get(key)==actual,(key,'unreviewed preparation change')
 evidence[key]=actual
for rel,want in changes.items():assert h(w.parent/rel)==want,rel
assert (pkg/'questmanager.cpp.fragment').read_text(encoding='utf-8-sig').strip() in (root/'game/src/questmanager.cpp').read_text(encoding='utf-8')
assert (pkg/'char.cpp.fragment').read_text(encoding='utf-8-sig').strip() in (root/'game/src/char.cpp').read_text(encoding='utf-8')
# Serialized tool output escapes backslashes: verify the actual original NUL literal is one slash.
literal=next(s for s in (pkg/'char.cpp.fragment').read_text(encoding='utf-8').splitlines() if 'msg[64]' in s);assert list(literal[literal.index('=')+1:literal.index(';')].strip().encode())==[39,92,48,39]
audit=load(pkg/'ELF_API_PACKET_AUDIT.json');owners={x['address']:x['name'] for x in audit['symbols']};assert owners['08136310']=='_ZN9CHARACTER14ConfirmWithMsgEPKcim' and owners['081a5400']=='_ZN5quest13CQuestManager12SetSkinStyleEi'
merged=load(rec/'function-coverage-overrides.json');sha=merged['target_sha256'];assert m0['target_sha256']==sha
functions=[]
for address,n in [('08136310',35),('081a5400',12)]:
 d=load(pkg/(address+'-decompile.json'));assert d['function']['address']==address and d.get('code') and not d.get('truncated');functions.append(d)
 asm=load(pkg/(address+'-assembly.json'));assert asm['has_more'] is False and len(asm['items'])==asm['total']==n

def ref(path,anchor):
 q=root/path;t=q.read_text(encoding='utf-8');assert anchor in t,(path,anchor)
 return dict(path=path,anchor=anchor,line=t[:t.index(anchor)].count('\n')+1,file_sha256=h(q))
limits='Two complete original leaves only, no Confirm dispatcher/timeout callback/PCConfirmWait/RunState implementation. NonnullDESC with invalid/null message or insufficient read range, invalidated contexts/borrowed aliases/concurrent descriptor changes remain original unsafe domains excluded without repair. Two GetDesc source/instruction loads preserved; runtime rebind claims require actual test evidence, not inferred from duplicate source calls. Actual SDK5.0.3 public compatibility only, not original5.0/internalABI; SDK28O2objects/twoarchives reusedstrictSHA/all59SDKguards, no recompilationclaim. Prior OpenState and Build matching remain static-only; fullVM/Reward/EndRunning/server/VisualStudio linking unresolved.'
refs={'08136310':('game/src/char.cpp','CHARACTER::ConfirmWithMsg('),'081a5400':('game/src/questmanager.cpp','CQuestManager::SetSkinStyle(')}
scopes={'08136310':'Whole CHARACTER ConfirmWithMsg35instructions: noDESC skips message access/output; actualpacked74record header46, requestPID/timeout, strncpy64+literalNUL64, freshDESC Packet; original long/DWORD32, fullPODfields defined.','081a5400':'Whole CQM SetSkinStyle12instructions: unsignedstyle<6 stores originalint, otherwise1, includingnegative/INTMIN/MAX fallback; no newenum/APIowner narrowing.'}
for d in functions:
 a=d['function']['address'];path,anchor=refs[a];r={'address':a,'source_refs':[ref(path,anchor)],'test_refs':[ref('reconstruction/tests/quest_confirm_leaves_regression.cpp','int main(')],'review_scope':scopes[a],'reviewer':'Parent checkpoint100 wholeELFsymbol/API/packet/instruction review +focused4/strict32/newO2app+SQL/verifiedSDKreuse/full113+ABI','limitations':limits}
 prior=next((x for x in merged['reviews'] if x['address']==a),None)
 if prior is None:merged['reviews'].append(r)
 else:prior.clear();prior.update(r)
status=load(rec/'validation-status.json');assert status['checkpoint99_quest_control_group']['status']=='passed'
note='Checkpoint100 restores whole CHARACTER::ConfirmWithMsg08136310 and CQM::SetSkinStyle081a5400, completepacked74-byte originalpacket. Originalmessage64copy+oneNUL, signedlongtimeout/DWORDPID and unsignedstyle<6/fallback1 retained. Focused4+ABI, strict32,5newO2SQL/observer+actualO2app and9helperproofs inclSDKreuse/index; full113 exactordered runtime+ABI pass. '+limits
manifest={'checkpoint':100,'status':'passed','full_runtime_commands':113,'focused_runtime_commands':4,'abi':True,'standalone_function_mappings':2,'inline_semantic_bodies':0,'typed_packet_size':74,'source_guards':review['guards'],'support_read_guards':review['support_read_guards'],'source_guard_review':review,'sdk_reuse':{'compiled_by_this_group':False,'sdk_objects':28,'archives':2,'all_expected_files':59,'original_revision_recovered':False,'old_application_hash_ignored_only_for_SDK_reuse':True},'proof_sha256':{str(q.resolve()):h(q) for q in proofs},'original_evidence_sha256':evidence,'input_sha256':{str(q.resolve()):h(q) for q in [pkg/'integration/plan.json',pkg/'MANIFEST.json',pkg/'ELF_API_PACKET_AUDIT.json',p/'source-guards.json',Path(__file__)]},'baseline_metadata_sha256':{name:h(rec/name) for name in ['function-coverage-overrides.json','validation-status.json','CURRENT_STATUS.md','RESUME_CHECKPOINT.md']},'baseline_overrides_sha256':h(rec/'function-coverage-overrides.json'),'baseline_validation_status_sha256':h(rec/'validation-status.json'),'note':note}
status.update(date=full['finished_utc'][:10],runtime_suites=full['suites'],runtime_suite_count=113,runtime_commands_verified=full['executed_runtime_commands'],runtime_command_count_verified=113,combined_make_check_exit_code=0,make_check_abi_exit_code=0,latest_full_check_exit_code=0,latest_check_abi_exit_code=0,full_validation_log=full['log'],latest_validation_receipt=str((w/'checkpoint-100-validation.receipt.json').resolve()));status['checkpoint100_quest_confirm_leaves']=manifest
out.mkdir(exist_ok=True)
def write(name,data):(out/name).write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')
for name,data in [('RECORD_MANIFEST.json',manifest),('validation-status.json',status),('function-coverage-overrides.json',merged),('quest-confirm-leaves-ghidra.json',{'target_sha256':sha,'functions':functions}),('quest-confirm-owner-packet-review.json',{'target_sha256':sha,'owner_API_packet_audit':audit,'packet_ref':ref('game/src/packet.h','struct packet_quest_confirm'),'sdk_reuse':reuse,'SDK_only_source_sha256':sdksources,'policy':limits})]:write(name,data)
(out/'quest-confirm-leaves-recovery.md').write_text('# Quest confirm/style leaves checkpoint100\n\n'+note+'\n',encoding='utf-8');print('Prepared workspace-only100:2wholemaps/full113+ABI, packed74record, SDKreuse not recompilation. No project writes.')
