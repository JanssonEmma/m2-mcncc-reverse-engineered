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
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';out=p/'prepared';buildpkg=w/'pc_build_cancel';rewardpkg=w/'pc_reward_producers'
load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'))
h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest()
normpath=lambda s:str(s).replace('\\','/')
proofs=[]
def passed(q):
 d=load(q);assert d.get('status')=='passed' and d.get('exit_code')==0 and d.get('finished_utc'),q;proofs.append(q)
 if d.get('log'):
  log=Path(d['log']);assert log.is_file(),log;proofs.append(log)
 return d
old=passed(w/'checkpoint-94-validation.receipt.json');full=passed(w/'checkpoint-95-validation.receipt.json');focused=passed(w/'quest-group-focused.receipt.json')
assert old['scope']=='full' and len(old['suites'])==104 and old['suites']==old['executed_runtime_commands']
for d in [full,focused]:
 assert d['make_exit_code']==0 and d['abi_requested'] and 'check-abi' in d['command']
 assert d['suites']==d['executed_runtime_commands'] and len(d['suites'])==len(set(d['suites']))
newsuites={'pc_build_cancel_regression','pc_reward_producers_regression'}
assert full['scope']=='full' and len(full['suites'])==106
assert all(full['suites'].count(s)==1 for s in newsuites)
assert [s for s in full['suites'] if s not in newsuites]==old['suites']
assert focused['scope']=='focused' and set(focused['suites'])==newsuites|{'pc_quest_save_regression','pc_state_helpers_regression'}
for stem,prefix in [('pc_build_cancel','pc-build-cancel'),('pc_reward_producers','pc-reward-producers')]:
 v=rec/'tests'/(stem+'_validation');receipts={};names=['strict32','item_sql_runtime-o2','item_sql_semaphore-o2','item_sql_api-o2','item_socket_config-o2',stem+'_desc_output_syscalls-o2','o2-build','o2-runtime']
 for name in names:
  d=passed(v/(prefix+'-'+name+'.receipt.json'));receipts[name]=d
  if name.endswith('-o2') or name=='o2-build':assert '-O2' in d['command'] and '-flto' not in d['command']
 assert all(x in receipts['strict32']['command'] for x in ['-std=c++03','-pedantic-errors','-m32','-fsyntax-only',stem+'_abi.cpp',stem+'_regression.cpp','../../game/src/questpc.cpp'])
 command=receipts['o2-build']['command'];assert '-DMCNCC_TEST_REAL_EVENTS' in command
 for name in ['event.cpp','event_queue.cpp','questpc.cpp','questevent.cpp','questmanager.cpp','buffer_manager.cpp']:assert '../../game/src/'+name in command,name
 objects=[]
 for name in names[1:6]:
  c=receipts[name]['command'];assert c[-2]=='-o';objects.append(c[-1]);assert c[-1] in command
 assert len(set(objects))==5 and receipts['o2-runtime']['command']==[command[-1]]
plan=load(p/'integration/plan.json');assert plan['runtime_count_after']==106
assert len(plan['files'])==len({normpath(x['path']) for x in plan['files']})==9,'Windows-equivalent paths must be unique (9 finalpaths)'
review=load(p/'source-guards.json');assert review.get('review');guards={normpath(k):v for k,v in review['guards'].items()};support={normpath(k):v for k,v in review['support_read_guards'].items()}
assert guards=={normpath(x['path']):x['after'] for x in plan['files']} and len(guards)==9
assert support=={normpath(x['path']):x['sha256'] for x in plan['dependencies']} and len(support)==5
for rel,want in {**guards,**support}.items():assert h(root/rel)==want,rel
for row in plan['files']:assert h(p/'integration/payload'/normpath(row['path']))==row['after'],row
for source,want in plan['parent_plan_sha256'].items():assert h(source)==want,source
runtime_scope=plan['runtime_scope'];assert runtime_scope['Build_matching_suffix']=='static_only_indeterminate_counter_copy' and runtime_scope['Build_runtime']=='empty_nonmatching_only' and runtime_scope['Reward_execution']=='not_in_scope'
overrides={normpath(k):v for k,v in review.get('reviewed_preparation_overrides',{}).items()};evidence={};packages={}
for pkg in [buildpkg,rewardpkg]:
 m=load(pkg/'MANIFEST.json');packages[pkg.name]=m
 index=m.get('evidence') or {r['path']:r['sha256'] for r in m['files']}
 for rel,want in index.items():
  q=pkg/rel;key=normpath(q.relative_to(w.parent));actual=h(q)
  immutable=bool(re.match(r'^0[0-9a-f]{7}-',rel)) or rel in {'questpc.cpp.fragment','questpc.h.fragment','setloaded-dwarf.json','setloaded-parent-asm.json','build-suffix-bytes.json','reward-and-state-ctor-dwarf.json','reward-producers-addressed-ghidra.json','original-api.json'}
  if actual!=want:assert not immutable and overrides.get(key)==actual,(key,'unreviewed preparation change')
  evidence[key]=actual
 for q in [pkg/'MANIFEST.json',pkg/'integration/plan.json']:evidence[normpath(q.relative_to(w.parent))]=h(q)
for rel,want in overrides.items():assert h(w.parent/rel)==want,rel
source=(root/'game/src/questpc.cpp').read_text(encoding='utf-8')
for pkg in [buildpkg,rewardpkg]:assert (pkg/'questpc.cpp.fragment').read_text(encoding='utf-8-sig').strip() in source,'Complete frozen fragment absent'
merged=load(rec/'function-coverage-overrides.json');sha=merged['target_sha256'];assert all(m['target_sha256']==sha for m in packages.values())
functions=[]
for pkg,address,n in [(buildpkg,'08061310',323),(buildpkg,'080603c0',8),(rewardpkg,'08060e60',106),(rewardpkg,'08060fd0',103)]:
 d=load(pkg/(address+'-decompile.json'));assert d['function']['address']==address and d.get('code') and not d.get('truncated');functions.append(d)
 asm=load(pkg/(address+('-asm.json' if pkg==buildpkg else '-assembly.json')));assert asm['has_more'] is False and len(asm['items'])==asm['total']==n
assert len({d['function']['address'] for d in functions})==4
inline_m=packages['pc_build_cancel']['inline'];assert len(inline_m)==1 and inline_m[0]['range']==['080e9b5e','080e9b62'] and inline_m[0]['synthetic_address'] is False

def ref(path,anchor):
 q=root/path;text=q.read_text(encoding='utf-8');assert anchor in text,(path,anchor)
 return dict(path=path,anchor=anchor,line=text[:text.index(anchor)].count('\n')+1,file_sha256=h(q))
limits='Whole source recovered; Build qualifying .__status/default QuestState copies read original indeterminate counters even on duplicates, so matching/unknown0/duplicate/state insertion are STATIC-ONLY, not executed or runtime-equivalent. Build runtime exercises only empty/nonmatching gates. No zero repair, replacement copy, fake VM or narrowed body. Reward producers queue only: actual Reward/AutoGiveItem/PointChange/Lua/EndRunning and full server/VisualStudio production build remain unresolved. Original unsafe alias/invalidation/concurrency/allocation domains preserved and excluded.'
scopes={
 '08061310':'Complete323-instruction Build; live sorted flag iteration/final9-byte suffix/manager query/value reload/real default state/copy/unique insertion statically reviewed. Runtime empty/nonmatching branches only.',
 '080603c0':'Complete8-instruction CancelRunning; exactly ordered runningNULL/send0/donefalse, preserving unrelated state under actual PC/event/manager lifecycle regression.',
 '08060e60':'Whole GiveItem queue producer: original log then live current quest/label query, actual GetFlag callback rereads, zero appends actual item RewardData, nonzero sets sticky bool; no flag write/grant.',
 '08060fd0':'Whole GiveExp queue producer: original log then live current quest/label query, zero appends actual EXP RewardData with defaultthird0, nonzero sticky bool; no EXP/stat execution.'}
for d in functions:
 address=d['function']['address'];name=d['function']['name'].split('::')[-1];suite='pc_build_cancel' if address in {'08061310','080603c0'} else 'pc_reward_producers'
 r={'address':address,'source_refs':[ref('game/src/questpc.cpp','PC::'+name+'(')],'test_refs':[ref('reconstruction/tests/'+suite+'_regression.cpp','int main(')],'review_scope':scopes[address],'reviewer':'Parent checkpoint95 fulloriginal/inline review + focused4/strict32/O2 pergroup/full106+ABI','limitations':limits,'runtime_scope':runtime_scope if address=='08061310' else 'Bounded actual lifecycle/queue regression, not full server equivalence'}
 prior=next((x for x in merged['reviews'] if x['address']==address),None)
 if prior is None:merged['reviews'].append(r)
 else:prior.clear();prior.update(r)
status=load(rec/'validation-status.json');assert status['checkpoint94_quest_save']['status']=='passed'
note='Checkpoint95 reconstructs whole Build08061310/CancelRunning080603c0, original true SetLoaded inline and whole GiveItem08060e60/GiveExp08060fd0 queue producers. Focused4+ABI, strict32 and optimized actual graphs with5distinct SQL/observer objects pergroup, and single full106 exact ordered runtimes+ABI pass. Build matching suffix branch remains STATIC-ONLY because default QuestState copies indeterminate counters; tests cover only empty/nonmatching gates. Full matching body preserved, no zero repair. Producers retain log/query/callback/queue/sticky flag semantics and do not execute rewards. '+limits
manifest={'checkpoint':95,'status':'passed','full_runtime_commands':106,'focused_runtime_commands':4,'abi':True,'standalone_function_mappings':4,'inline_semantic_bodies':1,'runtime_scope':runtime_scope,'static_only_scope':{'Build':'Every qualifying .__status branch including unknown0/duplicate/insertion; original indeterminate-counter copies preserved'},'source_guards':guards,'support_read_guards':support,'source_guard_review':review,'proof_sha256':{str(q.resolve()):h(q) for q in proofs},'original_evidence_sha256':evidence,'input_sha256':{str(q.resolve()):h(q) for q in [p/'integration/plan.json',p/'source-guards.json',Path(__file__)]},'baseline_metadata_sha256':{name:h(rec/name) for name in ['function-coverage-overrides.json','validation-status.json','CURRENT_STATUS.md','RESUME_CHECKPOINT.md']},'baseline_overrides_sha256':h(rec/'function-coverage-overrides.json'),'baseline_validation_status_sha256':h(rec/'validation-status.json'),'note':note}
status.update(date=full['finished_utc'][:10],runtime_suites=full['suites'],runtime_suite_count=106,runtime_commands_verified=full['executed_runtime_commands'],runtime_command_count_verified=106,combined_make_check_exit_code=0,make_check_abi_exit_code=0,latest_full_check_exit_code=0,latest_check_abi_exit_code=0,full_validation_log=full['log'],latest_validation_receipt=str((w/'checkpoint-95-validation.receipt.json').resolve()));status['checkpoint95_quest_group']=manifest
inline={'target_sha256':sha,'source_ref':ref('game/src/questpc.h','SetLoaded()'),'original_inline':inline_m,'dwarf':load(buildpkg/'setloaded-dwarf.json'),'instruction_parent':load(buildpkg/'setloaded-parent-asm.json'),'policy':'True byte store PC+0x5c only, no Build call, no invented standalone address; range high exclusive080e9b62.'}
out.mkdir(exist_ok=True)
def write(name,data):(out/name).write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')
for name,data in [('RECORD_MANIFEST.json',manifest),('validation-status.json',status),('function-coverage-overrides.json',merged),('pc-quest-group-ghidra.json',{'target_sha256':sha,'functions':functions}),('quest-setloaded-inline-review.json',inline)]:write(name,data)
(out/'quest-group-recovery.md').write_text('# Quest grouped checkpoint95\n\n'+note+'\n',encoding='utf-8')
print('Prepared workspace-only95:4maps/1trueinline; full106+ABI. Build matching static-only. No project writes.')
