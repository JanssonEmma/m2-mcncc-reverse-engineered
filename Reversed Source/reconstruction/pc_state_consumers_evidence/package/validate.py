# Parent-only consumer validation; reuse VERIFIED actual SDK O2 archives, no Lua VM substitute.
from pathlib import Path
import argparse,subprocess,shlex,json,datetime,hashlib
p=Path(__file__).resolve().parent;a=argparse.ArgumentParser();a.add_argument('--root',required=True);a.add_argument('--out',required=True);a.add_argument('--sdk-o2-dir',required=True);a.add_argument('--skip-normal',action='store_true');a.add_argument('--cxx',default='C:/Strawberry/c/bin/g++.exe');a.add_argument('--cc',default='C:/Strawberry/c/bin/gcc.exe');a.add_argument('--make',default='C:/Strawberry/c/bin/gmake.exe');v=a.parse_args();root=Path(v.root).resolve();cwd=root/'reconstruction/tests';out=Path(v.out).resolve();out.mkdir(parents=True,exist_ok=True);base=[v.make,'SHELL=cmd.exe','CC='+v.cc,'CXX='+v.cxx];receipts=[];h=lambda q:hashlib.sha256(Path(q).read_bytes()).hexdigest()
def run(name,cmd):
 log=out/('pc-state-consumers-'+name+'.log');r=dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),cwd=str(cwd),command=cmd,log=str(log))
 with log.open('w',encoding='utf-8') as f:q=subprocess.run(cmd,cwd=cwd,stdout=f,stderr=subprocess.STDOUT)
 r.update(status='passed' if q.returncode==0 else 'failed',exit_code=q.returncode,finished_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),log_sha256=h(log));dest=out/('pc-state-consumers-'+name+'.receipt.json');dest.write_text(json.dumps(r,indent=2)+'\n',encoding='utf-8');receipts.append(dest)
 if q.returncode:raise SystemExit(q.returncode)
sdk=Path(v.sdk_o2_dir).resolve();index=sdk/'quest-lua-queries-PROOF_INDEX.json';proof=json.loads(index.read_text(encoding='utf-8-sig'));assert proof['status']=='passed' and proof['sdk_C_objects']==28
for row in proof['receipts']:
 q=Path(row['path']);assert h(q)==row['sha256'];r=json.loads(q.read_text(encoding='utf-8-sig'));assert r['status']=='passed' and r['exit_code']==0 and h(r['log'])==r['log_sha256']
objectsSDK=proof['sdk_objects'];assert len(objectsSDK)==28 and len({r['name'] for r in objectsSDK})==28
for row in objectsSDK:assert h(row['path'])==row['sha256']
archives=proof['sdk_archives'];assert len(archives)==2
for row in archives:assert h(row['path'])==row['sha256']
libraries=[next(row['path'] for row in archives if Path(row['path']).name=='sdk_'+name+'_o2.a') for name in ['aux','core']]
manifest=json.loads((p/'MANIFEST.json').read_text(encoding='utf-8-sig'))
for row in manifest['sdk_expected_files']:assert h(root/row['path'])==row['sha256'],'SDK source/header drift '+row['path']
sdkRoot=(root/'liblua').resolve();checked=0
for source,digest in proof['source_sha256'].items():
 q=Path(source).resolve()
 if sdkRoot in q.parents:assert h(q)==digest;checked+=1
assert checked==28
(out/'pc-state-consumers-sdk-reuse.receipt.json').write_text(json.dumps(dict(status='passed',scope='verified actual SDK O2 reuse, not compilation by consumer',proof_index=str(index),proof_index_sha256=h(index),sdk_archives=archives,sdk_objects=objectsSDK,sdk_source_count=checked,sdk_source_header_guards=len(manifest['sdk_expected_files'])),indent=2)+'\n',encoding='utf-8')
if not v.skip_normal:run('normal-build-runtime-abi',base+['-B','check-pc-state-consumers','check-pc-state-consumers-abi'])
run('strict32',[v.cxx,'-Ifdwatch_shim','-Isocket_shim','-Iposix_shim','-std=c++03','-Wall','-Wextra','-pedantic-errors','-Wno-long-long','-Wno-invalid'+'-offsetof','-m32','-fsyntax-only','pc_state_consumers_abi.cpp','pc_state_consumers_regression.cpp','../../game/src/questpc.cpp'])
objects=['item_sql_runtime.o','item_sql_semaphore.o','item_sql_api.o','item_socket_config.o','pc_state_consumers_desc_output_syscalls.o'];mapping={};plan=subprocess.run(base+['-Bn']+objects,cwd=cwd,capture_output=True,text=True,check=True)
for line in plan.stdout.splitlines():
 if not(line.startswith(v.cxx+' ')or line.startswith(v.cc+' ')):continue
 cmd=shlex.split(line);assert cmd[-2]=='-o' and cmd[-1] in objects;old=cmd[-1];dest=out/('pc_state_consumers_'+old[:-2]+'_o2.o');mapping[old]=str(dest);cmd[-1]=str(dest);cmd.insert(-2,'-O2');run(old[:-2]+'-o2',cmd)
assert set(mapping)==set(objects)
plan=subprocess.run(base+['-n','-W','pc_state_consumers_regression.cpp','pc_state_consumers_regression'],cwd=cwd,capture_output=True,text=True,check=True);commands=[shlex.split(s)for s in plan.stdout.splitlines()if s.startswith(v.cxx+' ')];assert len(commands)==1;cmd=commands[0];assert '-DMCNCC_TEST_REAL_EVENTS' in cmd and '-flto' not in cmd
for required in ['../../game/src/event.cpp','../../game/src/event_queue.cpp','../../game/src/questpc.cpp','../../game/src/questmanager.cpp','../../game/src/char.cpp','../../game/src/desc.cpp','../../game/src/buffer_manager.cpp','buffer_manager_core.o']:assert required in cmd,required
normal=['../../liblua/liblualib_mcncc.a','../../liblua/liblua_mcncc.a'];assert all(lib in cmd for lib in normal)
cmd=[mapping.get(s,libraries[normal.index(s)]if s in normal else s)for s in cmd];assert cmd[-2]=='-o';exe=out/'pc_state_consumers_o2.exe';cmd[-1]=str(exe);cmd.insert(-2,'-O2');run('o2-build',cmd);run('o2-runtime',[str(exe)])
(out/'pc-state-consumers-PROOF_INDEX.json').write_text(json.dumps(dict(status='passed',sdk_reuse_receipt=str(out/'pc-state-consumers-sdk-reuse.receipt.json'),sdk_reuse_receipt_sha256=h(out/'pc-state-consumers-sdk-reuse.receipt.json'),receipts=[dict(path=str(q),sha256=h(q))for q in receipts]),indent=2)+'\n',encoding='utf-8')
