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
p=Path(__file__).resolve().parent;w=p.parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));rec=root/'reconstruction';out=p/'prepared';event=w/'quest_event_flags';lua=w/'quest_lua_queries'
load=lambda q:json.loads(Path(q).read_text(encoding='utf-8-sig'))
h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest()
normpath=lambda s:str(s).replace('\\','/')
proofs=[]
def passed(q):
 d=load(q);assert d.get('status')=='passed' and d.get('exit_code')==0 and d.get('finished_utc'),q;proofs.append(q)
 if d.get('log'):
  log=Path(d['log']);assert log.is_file(),log
  if d.get('log_sha256'):assert h(log)==d['log_sha256'],log
  proofs.append(log)
 return d
old=passed(w/'checkpoint-95-validation.receipt.json');full=passed(w/'checkpoint-96-validation.receipt.json');focused=passed(w/'quest-manager-focused-retry.receipt.json')
assert old['scope']=='full' and len(old['suites'])==106 and old['suites']==old['executed_runtime_commands']
for d in [full,focused]:
 assert d['make_exit_code']==0 and d['abi_requested'] and 'check-abi' in d['command']
 assert d['suites']==d['executed_runtime_commands'] and len(d['suites'])==len(set(d['suites']))
newsuites={'quest_event_flags_regression','quest_lua_queries_regression'}
assert full['scope']=='full' and len(full['suites'])==108 and all(full['suites'].count(s)==1 for s in newsuites)
assert [s for s in full['suites'] if s not in newsuites]==old['suites']
assert focused['scope']=='focused' and set(focused['suites'])==newsuites|{'pc_quest_save_regression','quest_pc_lookup_regression'}
failedpath=w/'quest-manager-focused.receipt.json';failed=load(failedpath)
assert failed['status']=='failed' and failed['exit_code']!=0 and failed.get('finished_utc')
proofs.extend([failedpath,Path(failed['log'])]);assert Path(failed['log']).is_file()
fixpath=p/'reviewed-fixes.json';assert fixpath.is_file();proofs.append(fixpath)
for name in ['before.cpp','after.cpp']:
 q=p/'stack_expectation_fix'/name;assert q.is_file();proofs.append(q)
# Event helper8: normal execution already proven by focused receipt.
ev=rec/'tests/quest_event_flags_validation';en=['strict32','item_sql_runtime-o2','item_sql_semaphore-o2','item_sql_api-o2','item_socket_config-o2','quest_event_flags_desc_output_syscalls-o2','o2-build','o2-runtime'];ed={n:passed(ev/('quest-event-flags-'+n+'.receipt.json')) for n in en}
assert all(x in ed['strict32']['command'] for x in ['-std=c++03','-pedantic-errors','-m32','-fsyntax-only','quest_event_flags_abi.cpp','quest_event_flags_regression.cpp','../../game/src/questmanager.cpp'])
# Actual SDK helper39 + sealed index. No intentional original-internal-layout probe in mandatory set.
core='lapi lcode ldebug ldo ldump lfunc lgc llex lmem lobject lopcodes lparser lstate lstring ltable ltests ltm lundump lvm lzio'.split();aux='lauxlib lbaselib ldblib liolib lmathlib ltablib lstrlib loadlib'.split()
lv=rec/'tests/quest_lua_queries_validation';ln=['public-strict32','app-strict32']+['sdk-'+n+'-o2' for n in core+aux]+['sdk-aux-archive','sdk-core-archive','item_sql_runtime-o2','item_sql_semaphore-o2','item_sql_api-o2','item_socket_config-o2','quest_lua_queries_desc_output_syscalls-o2','o2-build','o2-runtime'];assert len(ln)==39
ld={n:passed(lv/('quest-lua-queries-'+n+'.receipt.json')) for n in ln}
assert all(x in ld['public-strict32']['command'] for x in ['-std=c99','-pedantic-errors','-m32','-fsyntax-only','quest_lua_queries_public_abi.c'])
assert all(x in ld['app-strict32']['command'] for x in ['-std=c++03','-pedantic-errors','-m32','-fsyntax-only','quest_lua_queries_regression.cpp','../../game/src/questmanager.cpp'])
indexfile=lv/'quest-lua-queries-PROOF_INDEX.json';idx=load(indexfile);assert idx['status']=='passed' and idx['sdk_C_objects']==28 and idx['sql_observer_objects']==5 and len(idx['receipts'])==39;proofs.append(indexfile)
assert {Path(x['path']).name for x in idx['receipts']}=={'quest-lua-queries-'+n+'.receipt.json' for n in ln}
for x in idx['receipts']:assert h(x['path'])==x['sha256'],x
for key,count in [('sdk_objects',28),('sdk_archives',2),('sql_observer_artifacts',5)]:
 assert len(idx[key])==count and len({x['path'] for x in idx[key]})==count
 for x in idx[key]:assert h(x['path'])==x['sha256'],x
for q,want in idx['source_sha256'].items():assert h(q)==want,q
for n in core+aux:
 c=ld['sdk-'+n+'-o2']['command'];assert '-O2' in c and '-c' in c and c[-2]=='-o' and any(normpath(x['path'])==normpath(c[-1]) for x in idx['sdk_objects'])
for n in ['sdk-aux-archive','sdk-core-archive']:
 c=ld[n]['command'];assert c[1]=='rcs' and any(normpath(x['path'])==normpath(c[2]) for x in idx['sdk_archives'])
for data,names in [(ed,en),(ld,ln)]:
 command=data['o2-build']['command'];assert '-O2' in command and '-DMCNCC_TEST_REAL_EVENTS' in command and '-flto' not in command
 for name in ['event.cpp','event_queue.cpp','questpc.cpp','questevent.cpp','questmanager.cpp','buffer_manager.cpp']:assert '../../game/src/'+name in command,name
 objects=[]
 for n in names:
  if n.startswith(('item_sql_','item_socket_config-','quest_event_flags_desc_','quest_lua_queries_desc_')):
   c=data[n]['command'];assert '-O2' in c and c[-2]=='-o';objects.append(c[-1]);assert c[-1] in command
 assert len(set(objects))==5 and data['o2-runtime']['command']==[command[-1]]
assert all(x['path'] in ld['o2-build']['command'] for x in idx['sdk_archives'])
assert not any(x in ld['o2-build']['command'] for x in ['../../liblua/liblualib_mcncc.a','../../liblua/liblua_mcncc.a'])
plan=load(p/'integration/plan.json');assert plan['runtime_count_after']==108
assert len(plan['files'])==len({normpath(x['path']).lower() for x in plan['files']})==70
review=load(p/'source-guards.json');assert review.get('review');guards={normpath(k):v for k,v in review['guards'].items()};support={normpath(k):v for k,v in review['support_read_guards'].items()}
assert guards=={normpath(x['path']):x['after'] for x in plan['files']} and len(guards)==70
assert support=={normpath(x['path']):x['sha256'] for x in plan['dependencies']} and len(support)==10
for rel,want in {**guards,**support}.items():assert h(root/rel)==want,rel
for row in plan['files']:assert h(p/'integration/payload'/normpath(row['path']))==row['after'],row
for source,want in plan['parent_plan_sha256'].items():assert h(source)==want,source
# Package immutable evidence and explicitly reviewed preparation changes.
overrides={normpath(k):v for k,v in review.get('reviewed_preparation_overrides',{}).items()};evidence={}
frozen=load(p/'evidence-guards.json')['guards'];em=load(event/'MANIFEST.json');lm=load(lua/'MANIFEST.json')
for rel,want in em['evidence'].items():frozen[normpath((event/rel).relative_to(w.parent))]=want
for row in lm['package_files']:frozen[normpath((lua/row['path']).relative_to(w.parent))]=row['sha256']
for key,want in frozen.items():
 key=normpath(key);q=w.parent/key;actual=h(q);name=q.name;immutable=bool(re.match(r'^0[0-9a-f]{7}-',name)) or name.startswith('original-') or name in ['questmanager.cpp.fragment','questmanager.h.fragment','tables.h.fragment','sdk-provenance.json'] or '/sdk/liblua/' in key
 if actual!=want:assert not immutable and overrides.get(key)==actual,(key,'unreviewed preparation change')
 evidence[key]=actual
for rel,want in overrides.items():assert h(w.parent/rel)==want,rel
provenance=load(lua/'sdk-provenance.json');assert len(provenance['files'])==58
for row in provenance['files']:
 rel=normpath(row['path']);want=row['sha256'];assert h(row['reference'])==h(lua/'sdk/liblua'/rel)==h(root/'liblua'/rel)==want,rel
compat=load(lua/'sdk-compatibility-review.json');assert compat['original_internal_layout']['lua_State']==92 and compat['root_reported_MinGW_m32_layout']['lua_State']==96 and compat['original_internal_layout']['TObject']==12 and compat['root_reported_MinGW_m32_layout']['TObject']==16
assert compat['original_internal_layout']['lua_Number']==compat['root_reported_MinGW_m32_layout']['lua_Number']==8
# Failed internal probe is diagnostic evidence only, never passed mandatory ABI.
for name in ['quest_lua_sdk_preflight.log','quest_lua_sdk_preflight-retry.log','quest_lua_sdk_preflight-layout.s']:
 q=w/name;assert q.is_file();proofs.append(q)
source=(root/'game/src/questmanager.cpp').read_text(encoding='utf-8')
for pkg in [event,lua]:assert (pkg/'questmanager.cpp.fragment').read_text(encoding='utf-8-sig').strip() in source,'Whole original fragment absent'
merged=load(rec/'function-coverage-overrides.json');sha=merged['target_sha256'];assert em['target_sha256']==lm['target_sha256']==sha
functions=[]
for pkg,address,n in [(event,'081a5c40',124),(event,'081a5df0',34),(event,'081a6c00',238),(lua,'081a5130',82),(lua,'081a5240',131)]:
 d=load(pkg/(address+'-decompile.json'));assert d['function']['address']==address and d.get('code') and not d.get('truncated');functions.append(d)
 asm=load(pkg/(address+'-assembly.json'));assert asm['has_more'] is False and len(asm['items'])==asm['total']==n
assert len({d['function']['address'] for d in functions})==5

def ref(path,anchor):
 q=root/path;text=q.read_text(encoding='utf-8');assert anchor in text,(path,anchor)
 return dict(path=path,anchor=anchor,line=text[:text.index(anchor)].count('\n')+1,file_sha256=h(q))
limits='Actual supplied Lua5.0.3 source is an external dependency, not exact recovered original5.0 revision. Original internal lua_State92/TObject12(value4) differ MinGW32 SDK96/16(value8); Numberdouble8/CallInfo24 match. Mandatory ABI covers opaque public API only; failed internal probe retained diagnostic, no packing repair. Borrowed raw string lifetime/GC, invalidLua/context and nonfinite/out-of-int rint conversions remain original unsafe/static domains. Event repeated singleton loads are static evidence; tests change futureflags/DESC but not singleton identity. No SDKtarget mappings, broad SetEventFlag/VM/Reward/EndRunning/server/VisualStudio equivalence.'
for d in functions:
 address=d['function']['address'];name=d['function']['name'].split('::')[-1];stem='quest_event_flags' if address in {'081a5c40','081a5df0','081a6c00'} else 'quest_lua_queries'
 r={'address':address,'source_refs':[ref('game/src/questmanager.cpp','CQuestManager::'+name+'(')],'test_refs':[ref('reconstruction/tests/'+stem+'_regression.cpp','int main(')],'review_scope':'Whole original method with actual manager/CHAR/DESC/CLIENT_DESC packet graph or actual Lua SDK query/table/metamethod/error/stack conversion graph. Exact diagnostic/callback/context reload ordering preserved; bounded defined-domain runtime and original static instruction review.','reviewer':'Parent checkpoint96 original/data/publicAPI/provenance review + focused4/eventhelper8/Luahelper39+index/full108+ABI','limitations':limits,'runtime_scope':plan['runtime_scope']}
 prior=next((x for x in merged['reviews'] if x['address']==address),None)
 if prior is None:merged['reviews'].append(r)
 else:prior.clear();prior.update(r)
status=load(rec/'validation-status.json');assert status['checkpoint95_quest_group']['status']=='passed'
note='Checkpoint96 reconstructs3whole eventflag list/request/login methods and2whole Lua state query methods; full packed37-byte eventflag record,5genuine manager declarations and unchanged supplied Lua5.0.3 dependency. Focused4+ABI,eventhelper8,Luahelper39+sealedindex and full108 ordered runtime+ABI pass. CQM publicAPI remains partial; no SDKfunction targetmapping. '+limits
manifest={'checkpoint':96,'status':'passed','full_runtime_commands':108,'focused_runtime_commands':4,'abi':True,'standalone_function_mappings':5,'inline_semantic_bodies':0,'typed_record_size':37,'sdk_source_files':58,'runtime_scope':plan['runtime_scope'],'source_guards':guards,'support_read_guards':support,'source_guard_review':review,'proof_sha256':{str(q.resolve()):h(q) for q in proofs},'original_evidence_sha256':evidence,'input_sha256':{str(q.resolve()):h(q) for q in [p/'integration/plan.json',p/'source-guards.json',p/'evidence-guards.json',p/'reviewed-fixes.json',Path(__file__)]},'baseline_metadata_sha256':{name:h(rec/name) for name in ['function-coverage-overrides.json','validation-status.json','CURRENT_STATUS.md','RESUME_CHECKPOINT.md']},'baseline_overrides_sha256':h(rec/'function-coverage-overrides.json'),'baseline_validation_status_sha256':h(rec/'validation-status.json'),'note':note}
status.update(date=full['finished_utc'][:10],runtime_suites=full['suites'],runtime_suite_count=108,runtime_commands_verified=full['executed_runtime_commands'],runtime_command_count_verified=108,combined_make_check_exit_code=0,make_check_abi_exit_code=0,latest_full_check_exit_code=0,latest_check_abi_exit_code=0,full_validation_log=full['log'],latest_validation_receipt=str((w/'checkpoint-96-validation.receipt.json').resolve()));status['checkpoint96_quest_manager_group']=manifest
record={'target_sha256':sha,'record_ref':ref('common/tables.h','struct SPacketSetEventFlag'),'record_policy':'Full packed37 original record char33@0/long32@33; no inventedenum or SDKaddress.'}
sdkreview={'target_sha256':sha,'compatibility':compat,'provenance':provenance,'public_API_proofs':[str(lv/'quest-lua-queries-public-strict32.receipt.json')],'sdk_policy':limits,'mandatory_internal_layout_match':False,'sdk_target_mappings':0}
out.mkdir(exist_ok=True)
def write(name,data):(out/name).write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')
for name,data in [('RECORD_MANIFEST.json',manifest),('validation-status.json',status),('function-coverage-overrides.json',merged),('quest-manager-group-ghidra.json',{'target_sha256':sha,'functions':functions}),('quest-eventflag-record-review.json',record),('quest-lua-sdk-review.json',sdkreview)]:write(name,data)
(out/'quest-manager-group-recovery.md').write_text('# Quest manager grouped checkpoint96\n\n'+note+'\n',encoding='utf-8')
print('Prepared workspace-only96:5targetmaps/full37record/actualSDKpublic compatibility; full108+ABI. InternalABI mismatch diagnostic only. No project writes.')
