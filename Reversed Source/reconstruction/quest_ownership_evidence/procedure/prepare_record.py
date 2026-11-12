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
import json,hashlib,re
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';pkg=w/'quest_ownership';out=p/'prepared'
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
old=passed(w/'checkpoint-100-validation.receipt.json');full=passed(w/'checkpoint-101-validation.receipt.json');focused=passed(w/'quest-ownership-focused.receipt.json')
assert old['scope']=='full' and len(old['suites'])==113 and old['suites']==old['executed_runtime_commands']
for d in [full,focused]:
 assert d['make_exit_code']==0 and d['abi_requested'] and 'check-abi' in d['command']
 assert d['suites']==d['executed_runtime_commands'] and len(d['suites'])==len(set(d['suites']))
assert full['scope']=='full' and len(full['suites'])==114 and full['suites'].count('quest_ownership_regression')==1
assert [s for s in full['suites'] if s!='quest_ownership_regression']==old['suites']
assert focused['scope']=='focused' and set(focused['suites'])=={'quest_ownership_regression','quest_pc_lookup_regression','pc_flags_timers_regression','quest_lua_queries_regression'}
v=rec/'tests/quest_ownership_validation';names=['strict32','item_sql_runtime-o2','item_sql_semaphore-o2','item_sql_api-o2','item_socket_config-o2','quest_ownership_desc_output_syscalls-o2','o2-build','o2-runtime'];ds={n:passed(v/('quest-ownership-'+n+'.receipt.json')) for n in names}
assert all(x in ds['strict32']['command'] for x in ['-std=c++03','-pedantic-errors','-m32','-fsyntax-only','quest_ownership_abi.cpp','quest_ownership_regression.cpp','../../game/src/questmanager.cpp','../../game/src/questnpc.cpp'])
command=ds['o2-build']['command'];assert '-O2' in command and '-DMCNCC_TEST_REAL_EVENTS' in command and '-flto' not in command
for name in ['questnpc.cpp','event.cpp','event_queue.cpp','questpc.cpp','questmanager.cpp','char.cpp','desc.cpp','buffer_manager.cpp']:assert '../../game/src/'+name in command,name
assert 'buffer_manager_core.o' in command and command.count('../../game/src/questnpc.cpp')==1
objects=[]
for n in names[1:6]:
 c=ds[n]['command'];assert '-O2' in c and c[-2]=='-o';objects.append(c[-1]);assert c[-1] in command
assert len(set(objects))==5 and ds['o2-runtime']['command']==[command[-1]]
# Explicit SDK reuse: do not treat old query proof's application source hash as current PC guard.
reusepath=v/'quest-ownership-sdk-reuse.receipt.json';reuse=load(reusepath);assert reuse['status']=='passed' and reuse['sdk_source_count']==28 and reuse['sdk_source_header_guards']==59;proofs.append(reusepath)
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
consumerindexpath=v/'quest-ownership-PROOF_INDEX.json';ci=load(consumerindexpath);assert ci['status']=='passed' and len(ci['receipts'])==8 and h(ci['sdk_reuse_receipt'])==ci['sdk_reuse_receipt_sha256'];proofs.append(consumerindexpath)
assert {Path(r['path']).name for r in ci['receipts']}=={'quest-ownership-'+n+'.receipt.json' for n in names}
for row in ci['receipts']:assert h(row['path'])==row['sha256']
m0=load(pkg/'MANIFEST.json');assert len(m0['sdk_expected_files'])==59
for row in m0['sdk_expected_files']:assert h(root/row['path'])==row['sha256'],row
prov=load(w/'quest_lua_queries/sdk-provenance.json');assert len(prov['files'])==58
for row in prov['files']:assert h(root/'liblua'/row['path'])==h(row['reference'])==row['sha256'],row
plan=load(pkg/'integration/plan.json');review=load(p/'source-guards.json');assert review.get('review') and plan['runtime_count_after']==114
assert len(plan['files'])==len({r['path'].replace('\\','/').lower() for r in plan['files']})==8
assert review['guards']=={r['path']:r['after'] for r in plan['files']}
assert review['support_read_guards']=={r['path']:r['sha256'] for r in plan['dependencies']} and len(review['support_read_guards'])==67
for rel,want in {**review['guards'],**review['support_read_guards']}.items():assert h(root/rel)==want,rel
for row in plan['files']:assert h(pkg/'integration/payload'/row['path'])==row['after'],row
independent=review['independent_review'];independent_path=w.parent/independent['path'];assert h(independent_path)==independent['sha256']
independent_data=load(independent_path);assert independent_data['status']=='passed_no_actionable_mismatch' and independent_data['actionable_findings']==[];proofs.append(independent_path)
changes=review.get('reviewed_preparation_overrides',{});evidence={}
for rel,want in m0['package_files'].items():
 q=pkg/rel;key=str(q.relative_to(w.parent)).replace('\\','/');actual=h(q);immutable=bool(re.match(r'^0[0-9a-f]{7}-',q.name)) or q.name in ['ELF_OWNERSHIP_API_AUDIT.json','questmanager.cpp.fragment','questmanager.h.fragment','questnpc.cpp.candidate']
 if actual!=want:assert not immutable and changes.get(key)==actual,(key,'unreviewed preparation change')
 evidence[key]=actual
for rel,want in changes.items():assert h(w.parent/rel)==want,rel
assert (pkg/'questmanager.cpp.fragment').read_text(encoding='utf-8-sig').strip() in (root/'game/src/questmanager.cpp').read_text(encoding='utf-8')
assert h(pkg/'questnpc.cpp.candidate')==h(root/'game/src/questnpc.cpp'),'Whole NPC constructor/destructor candidate drift'
fixture=(root/'reconstruction/tests/quest_flag_fixture.cpp').read_text(encoding='utf-8-sig');code=re.sub(r'//[^\n]*|/\*.*?\*/','',fixture,flags=re.S)
assert not re.search(r'CQuestManager\s*::\s*(?:~CQuestManager|Destroy)\s*\(',code),'Old fake owning lifecycle still present'
prop=load(pkg/'MAKEFILE_PROPAGATION_AUDIT.json');make_row=next(r for r in plan['files'] if r['path']=='reconstruction/tests/Makefile');assert prop['before_sha256']==make_row['before']
assert prop['actualmanager_occurrences']==prop['paired_actualNPC_occurrences']==13
make=(root/'reconstruction/tests/Makefile').read_text(encoding='utf-8-sig');lines=[s for s in make.splitlines() if '../../game/src/questmanager.cpp' in s]
assert len(lines)==13 and all(s.count('../../game/src/questmanager.cpp')==s.count('../../game/src/questnpc.cpp')==1 for s in lines),'Global manager/NPC source pairing drift'
assert make.count('../../game/src/questnpc.cpp')==13
assert len(prop['rows'])==13
# Whole semantic bodies and their genuine compiler-generated address variants, not invented wrappers.
spec=[('081a0720',16,'_ZN5quest13CQuestManager7DestroyEv','game/src/questmanager.cpp','CQuestManager::Destroy(','CQM Destroy'),('0819fa20',272,'_ZN5quest13CQuestManagerD2Ev','game/src/questmanager.cpp','CQuestManager::~CQuestManager(','CQM component destructor D2'),('0819fe70',272,'_ZN5quest13CQuestManagerD1Ev','game/src/questmanager.cpp','CQuestManager::~CQuestManager(','CQM complete destructor D1'),('081a02c0',274,'_ZN5quest13CQuestManagerD0Ev','game/src/questmanager.cpp','CQuestManager::~CQuestManager(','CQM deleting destructor D0'),('081adf50',95,'_ZN5quest3NPCC2Ev','game/src/questnpc.cpp','NPC::NPC(','NPC base constructor C2'),('081ae0c0',52,'_ZN5quest3NPCC1Ev','game/src/questnpc.cpp','NPC::NPC(','NPC complete constructor C1'),('081ae180',72,'_ZN5quest3NPCD2Ev','game/src/questnpc.cpp','NPC::~NPC(','NPC component destructor D2'),('081ae260',72,'_ZN5quest3NPCD1Ev','game/src/questnpc.cpp','NPC::~NPC(','NPC complete destructor D1')]
audit=load(pkg/'ELF_OWNERSHIP_API_AUDIT.json');owners={x['address']:x['name'] for x in audit['symbols']};merged=load(rec/'function-coverage-overrides.json');sha=merged['target_sha256']
assert m0['target_sha256']==audit['target_sha256']==sha and m0['semantic_methods']==4 and m0['original_address_mappings']==8
functions=[]
for a,n,symbol,path,anchor,role in spec:
 assert owners[a]==symbol,(a,symbol)
 d=load(pkg/(a+'-decompile.json'));assert d['function']['address']==a and d.get('code') and not d.get('truncated');functions.append(d)
 asm=load(pkg/(a+'-assembly.json'));assert asm['has_more'] is False and len(asm['items'])==asm['total']==n
assert '081ab460' not in {a for a,*rest in spec} and owners['081ab460']=='_ZN5quest3NPCC1ERKS0_'

def ref(path,anchor):
 q=root/path;t=q.read_text(encoding='utf-8');assert anchor in t,(path,anchor)
 return dict(path=path,anchor=anchor,line=t[:t.index(anchor)].count('\n')+1,file_sha256=h(q))
limits='Eight original address variants represent four semantic source methods, not eight independent features. NPC implicit copy081ab460 is compiler context only, not a default constructor/new mapping. Full original NPC484/CQM216 data and all40NPC maps retained; empty NPC destructor user body still invokes complete member destruction, not a no-op stub. Populated byvalue AArgScript runtime setup is excluded because its actual default constructor remains unproven; empty vectors/reserved capacity and ordinary scripts are defined tested domains. Destroy closes initial currentL then setsL0 after finalizers, overwriting callback replacements; no added PC/NPC/timer/context clear or raw server-event ownership. Invalid/reentrant-close/borrowed/allocator/concurrent states remain original unsafe domains excluded. Actual SDK5.0.3 public compatibility only, original5.0/internalABI equality unproven (96/16 vs92/12); SDK28objects/2archives reusedstrictSHA/all59guards, no SDK mapping/recompilation. Prior OpenState/Build matching remain static-only; VM/dispatch/Reward/EndRunning/server/VisualStudio unfinished.'
for (a,n,symbol,path,anchor,role),d in zip(spec,functions):
 scope=role+': whole genuine source lifecycle and compiler member/base teardown variant; original full instruction audit, actual SDK/finalizer/NPC-container/PC-timer ownership tests and global recipe linkage. '
 if a=='081a0720':scope+='NULL L no-op; actual lua_close before L0; finalizer replacement L overwritten, no unrelated cleanup.'
 elif a in ['081adf50','081ae0c0']:scope+='All20plain+20arg maps construct before body m_vnum0.'
 elif a in ['081ae180','081ae260']:scope+='All20arg maps then20plain maps destroyed; populated AArgScript elements static-only/unforced.'
 else:scope+='Destroy first then full reverse fields/singleton base; deleting variant uses native operator delete; raw server-event pointees remain nonowned.'
 r={'address':a,'source_refs':[ref(path,anchor)],'test_refs':[ref('reconstruction/tests/quest_ownership_regression.cpp','int main(')],'review_scope':scope,'reviewer':'Parent checkpoint101 rawELF/API/fullinstruction ownership review +global13recipe/fixture retirement +focused4/strict32/newO2appNPC+SQL/verifiedSDKreuse/full114+ABI','limitations':limits,'semantic_variant_role':role}
 prior=next((x for x in merged['reviews'] if x['address']==a),None)
 if prior is None:merged['reviews'].append(r)
 else:prior.clear();prior.update(r)
status=load(rec/'validation-status.json');assert status['checkpoint100_quest_confirm_leaves']['status']=='passed'
note='Checkpoint101 integrates real CQM Destroy/destructor and full NPC default constructor/destructor:8genuine original address variants/4source methods. Full114 ordered runtimes+ABI,focused4,strict32,5newO2SQLobserver+actualNPC/app graph and verifiedSDKreuse/index pass. Old owning-Lua/emptyNPC destructor fixture removed globally; all13manager source/dependency/recipe literals pair actualNPC.cpp. RealSDK finalizers, completeNPCordinary containers and actualPC/event cleanup tested; raw server-event pointers remain nonowned. '+limits
manifest={'checkpoint':101,'status':'passed','full_runtime_commands':114,'focused_runtime_commands':4,'abi':True,'standalone_function_mappings':8,'semantic_methods':4,'inline_semantic_bodies':0,'compiler_copy_context_only':'081ab460','owning_fixture_boundary':'retired by genuine production CQM/NPC lifecycle and full114 proof; unrelated fixture limits remain','AArgScript_constructor':'unproven; populated byvalue-element runtime domain excluded','global_manager_NPC_pairs':13,'source_guards':review['guards'],'support_read_guards':review['support_read_guards'],'source_guard_review':review,'sdk_reuse':{'compiled_by_this_group':False,'sdk_objects':28,'archives':2,'all_expected_files':59,'original_revision_recovered':False,'old_application_hash_ignored_only_for_SDK_reuse':True},'proof_sha256':{str(q.resolve()):h(q) for q in proofs},'original_evidence_sha256':evidence,'input_sha256':{str(q.resolve()):h(q) for q in [pkg/'integration/plan.json',pkg/'MANIFEST.json',pkg/'ELF_OWNERSHIP_API_AUDIT.json',pkg/'MAKEFILE_PROPAGATION_AUDIT.json',p/'source-guards.json',Path(__file__)]},'baseline_metadata_sha256':{name:h(rec/name) for name in ['function-coverage-overrides.json','validation-status.json','CURRENT_STATUS.md','RESUME_CHECKPOINT.md']},'baseline_overrides_sha256':h(rec/'function-coverage-overrides.json'),'baseline_validation_status_sha256':h(rec/'validation-status.json'),'note':note}
status.update(date=datetime.fromisoformat(full['finished_utc']).astimezone(ZoneInfo('Europe/Berlin')).date().isoformat(),runtime_suites=full['suites'],runtime_suite_count=114,runtime_commands_verified=full['executed_runtime_commands'],runtime_command_count_verified=114,combined_make_check_exit_code=0,make_check_abi_exit_code=0,latest_full_check_exit_code=0,latest_check_abi_exit_code=0,full_validation_log=full['log'],latest_validation_receipt=str((w/'checkpoint-101-validation.receipt.json').resolve()));status['checkpoint101_quest_ownership']=manifest
out.mkdir(exist_ok=True)
def write(name,data):(out/name).write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')
for name,data in [('RECORD_MANIFEST.json',manifest),('validation-status.json',status),('function-coverage-overrides.json',merged),('quest-ownership-ghidra.json',{'target_sha256':sha,'functions':functions}),('quest-ownership-owner-sdk-review.json',{'target_sha256':sha,'owner_API_data_audit':audit,'Makefile_propagation':prop,'fixture_ref':ref('reconstruction/tests/quest_flag_fixture.cpp','#include'),'SDK_reuse':reuse,'SDK_only_source_sha256':sdksources,'mapping_variants':[dict(address=a,instructions=n,symbol=symbol,source_owner=path,semantic_role=role)for a,n,symbol,path,anchor,role in spec],'copy_context_only':'081ab460 implicit NPC copy; no standalone source implementation/mapping added','policy':limits})]:write(name,data)
(out/'quest-ownership-recovery.md').write_text('# Genuine quest ownership checkpoint101\n\n'+note+'\n',encoding='utf-8');print('Prepared workspace-only101:8address variants/4wholemethods/full114+ABI, owning fixture retired by actual lifecycle. No project writes.')
