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
import json,hashlib
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';pkg=w/'pc_quest_save';out=p/'prepared'
load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'))
h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest()
proofs=[]
def passed(q):
 d=load(q);assert d.get('status')=='passed' and d.get('exit_code')==0 and d.get('finished_utc'),q;proofs.append(q)
 if d.get('log'):
  log=Path(d['log']);assert log.is_file(),log;proofs.append(log)
 return d
old=passed(w/'checkpoint-93-validation.receipt.json');full=passed(w/'checkpoint-94-validation.receipt.json');focused=passed(w/'quest-save-focused.receipt.json')
assert old['scope']=='full' and len(old['suites'])==103 and old['suites']==old['executed_runtime_commands']
for d in [full,focused]:
 assert d['make_exit_code']==0 and d['abi_requested'] and 'check-abi' in d['command']
 assert d['suites']==d['executed_runtime_commands'] and len(d['suites'])==len(set(d['suites']))
assert full['scope']=='full' and len(full['suites'])==104 and full['suites'].count('pc_quest_save_regression')==1
assert [s for s in full['suites'] if s!='pc_quest_save_regression']==old['suites']
assert focused['scope']=='focused' and set(focused['suites'])=={'pc_quest_save_regression','pc_quest_info_regression','pc_flags_timers_regression'}
v=rec/'tests/pc_quest_save_validation';receipts={}
names=['strict32','item_sql_runtime-o2','item_sql_semaphore-o2','item_sql_api-o2','item_socket_config-o2','pc_quest_save_desc_output_syscalls-o2','o2-build','o2-runtime']
for name in names:
 d=passed(v/('pc-quest-save-'+name+'.receipt.json'));receipts[name]=d
 if name.endswith('-o2') or name=='o2-build':assert '-O2' in d['command'] and '-flto' not in d['command']
assert all(x in receipts['strict32']['command'] for x in ['-std=c++03','-pedantic-errors','-m32','-fsyntax-only','pc_quest_save_abi.cpp','pc_quest_save_regression.cpp','../../game/src/questpc.cpp'])
build=receipts['o2-build']['command'];assert '-DMCNCC_TEST_REAL_EVENTS' in build and 'buffer_manager_core.o' in build
for path in ['event.cpp','event_queue.cpp','questpc.cpp','questevent.cpp','questmanager.cpp','buffer_manager.cpp','desc_client.cpp']:assert '../../game/src/'+path in build,path
objects=[]
for name in names[1:6]:
 c=receipts[name]['command'];assert c[-2]=='-o';objects.append(c[-1]);assert c[-1] in build
assert len(set(objects))==5 and receipts['o2-runtime']['command']==[build[-1]]
plan=load(pkg/'integration/plan.json');assert len(plan['files'])==6 and len(plan['dependencies'])==5 and plan['runtime_count_after']==104
review=load(p/'source-guards.json');assert review.get('review')
guards=review['guards'];support=review['support_read_guards'];assert len(guards)==6 and support=={x['path']:x['sha256'] for x in plan['dependencies']}
assert set(guards)=={x['path'] for x in plan['files']}
for rel,want in {**guards,**support}.items():assert h(root/rel)==want,rel
for row in plan['files']:
 assert row['after']==guards[row['path']],row
 assert h(pkg/'integration/payload'/row['path'])==row['after'],row
manifest0=load(pkg/'MANIFEST.json');overrides=review.get('reviewed_preparation_overrides',{});evidence={}
immutable={'08060770-calls.json','08060770-decompile.c','08060770-decompile.json','save-assembly.json','save-record-dwarf.json','questpc.cpp.fragment','tables.h.fragment'}
for rel,want in manifest0['evidence'].items():
 q=pkg/rel;key=str(q.relative_to(w.parent)).replace('\\','/');actual=h(q)
 if actual!=want:assert rel not in immutable and overrides.get(key)==actual,(key,'unreviewed frozen preparation change')
 evidence[key]=actual
q=pkg/'integration/plan.json';key=str(q.relative_to(w.parent)).replace('\\','/')
if h(q)!=manifest0['integration_plan_sha256']:assert overrides.get(key)==h(q),'unreviewed plan change'
for key,want in overrides.items():assert h(w.parent/key)==want,key
merged=load(rec/'function-coverage-overrides.json');sha=merged['target_sha256'];d=load(pkg/'08060770-decompile.json')
assert manifest0['target_sha256']==sha and d['function']['address']=='08060770' and d.get('code') and not d.get('truncated')
original=load(w/'quest_level/08060770-decompile.json');assert original['function']['address']=='08060770' and original.get('code') and not original.get('truncated') and original['code']==d['code']
asm=load(pkg/'save-assembly.json');assert asm['has_more'] is False and len(asm['items'])==asm['total']==280
proofs.extend([w/'quest_level/08060770-decompile.json',pkg/'08060770-decompile.json',pkg/'save-assembly.json',pkg/'save-record-dwarf.json'])
def ref(path,anchor):
 q=root/path;text=q.read_text(encoding='utf-8');assert anchor in text,(path,anchor)
 return dict(path=path,anchor=anchor,line=text[:text.index(anchor)].count('\n')+1,file_sha256=h(q))
limits='Map-growing/erase/clear callbacks and reentrant Save can invalidate iterators, borrowed keys or static-vector storage and overrun initially sized rows; original unsafe domains are preserved and not forced. Huge size/find/count conversion, null/dangling DB and original COW SDK domains remain unproven. Test lifecycle and external OS/MySQL/log observers are explicit; actual DB header/body, PC, event and buffer methods remain complete. No Lua/VM/full-server linking or VisualStudio production-build claim.'
r={'address':'08060770','source_refs':[ref('game/src/questpc.cpp','PC::Save(')],'test_refs':[ref('reconstruction/tests/pc_quest_save_regression.cpp','int main(')],'review_scope':'Complete original PC::Save with typed106-byte TQuestTable, static vector resize/stale-row tails and zero-grown rows, first-dot/length-vs-strcpy rules, diagnostic ordering, cached value/current PID callback behavior, late global DB reload across actual header/body and final live pending-map clear.','reviewer':'Parent checkpoint94 original280 instructions/DWARF/full body review + focused3/strict32/O2/full104+ABI','limitations':limits}
prior=next((x for x in merged['reviews'] if x['address']==r['address']),None)
if prior is None:merged['reviews'].append(r)
else:prior.clear();prior.update(r)
status=load(rec/'validation-status.json');assert status['checkpoint93_quest_info']['status']=='passed'
note='Checkpoint94 reconstructs the complete original PC::Save08060770 and full packed106-byte SQuestTable/TQuestTable with original DWORD/long32 and char[33]/char[65] fields at0/4/37/102. Focused3+ABI, strict C++03/m32 syntax, actual optimized PC/event/manager/CLIENT_DESC/buffer graph with5 distinct SQL/observer objects and full104 exact ordered runtimes+ABI pass. Static-vector resize, reused stale tails/zero-grown rows, cached pre-log values/current PID reload, actual separate DB header/body(global pointer reloaded), and late live pending-map clear retained. No invented named packet/quest enum or production repair. '+limits
manifest={'checkpoint':94,'status':'passed','full_runtime_commands':104,'focused_runtime_commands':3,'abi':True,'standalone_function_mappings':1,'inline_semantic_bodies':0,'typed_record_size':106,'source_guards':guards,'support_read_guards':support,'source_guard_review':review,'proof_sha256':{str(q.resolve()):h(q) for q in proofs},'original_evidence_sha256':evidence,'input_sha256':{str(q.resolve()):h(q) for q in [pkg/'MANIFEST.json',pkg/'integration/plan.json',p/'source-guards.json',Path(__file__)]},'baseline_metadata_sha256':{name:h(rec/name) for name in ['function-coverage-overrides.json','validation-status.json','CURRENT_STATUS.md','RESUME_CHECKPOINT.md']},'baseline_overrides_sha256':h(rec/'function-coverage-overrides.json'),'baseline_validation_status_sha256':h(rec/'validation-status.json'),'note':note}
status.update(date=full['finished_utc'][:10],runtime_suites=full['suites'],runtime_suite_count=104,runtime_commands_verified=full['executed_runtime_commands'],runtime_command_count_verified=104,combined_make_check_exit_code=0,make_check_abi_exit_code=0,latest_full_check_exit_code=0,latest_check_abi_exit_code=0,full_validation_log=full['log'],latest_validation_receipt=str((w/'checkpoint-94-validation.receipt.json').resolve()));status['checkpoint94_quest_save']=manifest
record={'target_sha256':sha,'record_ref':ref('common/tables.h','struct SQuestTable'),'original_record_dwarf':load(pkg/'save-record-dwarf.json'),'record_metadata':manifest0['record'],'policy':'Full original106-byte SQuestTable/TQuestTable record; no invented standalone code address, explicit constructor or named packet enum.'}
out.mkdir(exist_ok=True)
def write(name,data):(out/name).write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')
for name,data in [('RECORD_MANIFEST.json',manifest),('validation-status.json',status),('function-coverage-overrides.json',merged),('pc-quest-save-ghidra.json',{'target_sha256':sha,'functions':[d]}),('quest-save-record-review.json',record)]:write(name,data)
for name in ['CURRENT_STATUS.md','RESUME_CHECKPOINT.md']:(out/name).write_text((rec/name).read_text(encoding='utf-8')+'\n\n'+note+'\n',encoding='utf-8')
(out/'quest-save-recovery.md').write_text('# Quest Save checkpoint94\n\n'+note+'\n',encoding='utf-8')
print('Prepared workspace-only94:1 original Save mapping/full106byte record; full104+ABI. No production writes.')
