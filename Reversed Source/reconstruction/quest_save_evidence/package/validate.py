# Parent-only helper: actual complete graph/real events, optimized C++ and distinct CSQL/observer objects.
from pathlib import Path
import argparse,subprocess,shlex,json,datetime
p=Path(__file__).resolve().parent;a=argparse.ArgumentParser();a.add_argument('--root',required=True);a.add_argument('--skip-normal',action='store_true',help='Normal suite/ABI already passed via canonical focused receipt');a.add_argument('--cxx',default='C:/Strawberry/c/bin/g++.exe');a.add_argument('--cc',default='C:/Strawberry/c/bin/gcc.exe');a.add_argument('--make',default='C:/Strawberry/c/bin/gmake.exe');a.add_argument('--out',help='Parent-selected output directory; native test directory supported');v=a.parse_args();cwd=Path(v.root)/'reconstruction/tests';out=Path(v.out).resolve() if v.out else p/'validation';out.mkdir(parents=True,exist_ok=True);base=[v.make,'SHELL=cmd.exe','CC='+v.cc,'CXX='+v.cxx]
def run(name,cmd):
 log=out/('pc-quest-save-'+name+'.log');r={'started_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'cwd':str(cwd),'command':cmd,'log':str(log)}
 with log.open('w',encoding='utf-8') as f:q=subprocess.run(cmd,cwd=cwd,stdout=f,stderr=subprocess.STDOUT)
 r.update(status='passed' if q.returncode==0 else 'failed',exit_code=q.returncode,finished_utc=datetime.datetime.now(datetime.timezone.utc).isoformat());(out/('pc-quest-save-'+name+'.receipt.json')).write_text(json.dumps(r,indent=2),encoding='utf-8')
 if q.returncode:raise SystemExit(q.returncode)
if not v.skip_normal:run('normal-build-runtime-abi',base+['-B','check-pc-quest-save','check-pc-quest-save-abi'])
run('strict32',[v.cxx,'-Ifdwatch_shim','-Isocket_shim','-Iposix_shim','-std=c++03','-Wall','-Wextra','-pedantic-errors','-Wno-long-long','-Wno-invalid'+'-offsetof','-m32','-fsyntax-only','pc_quest_save_abi.cpp','pc_quest_save_regression.cpp','../../game/src/questpc.cpp'])
objects=['item_sql_runtime.o','item_sql_semaphore.o','item_sql_api.o','item_socket_config.o','pc_quest_save_desc_output_syscalls.o'];mapping={};plan=subprocess.run(base+['-Bn']+objects,cwd=cwd,capture_output=True,text=True,check=True)
for line in plan.stdout.splitlines():
 if not (line.startswith(v.cxx+' ') or line.startswith(v.cc+' ')):continue
 cmd=shlex.split(line);assert cmd[-2]=='-o' and cmd[-1] in objects;old=cmd[-1];new=out/('pc_quest_save_'+old[:-2]+'_o2.o');mapping[old]=str(new);cmd[-1]=str(new);cmd.insert(-2,'-O2');run(old[:-2]+'-o2',cmd)
assert set(mapping)==set(objects)
plan=subprocess.run(base+['-n','-W','pc_quest_save_regression.cpp','pc_quest_save_regression'],cwd=cwd,capture_output=True,text=True,check=True);commands=[shlex.split(s) for s in plan.stdout.splitlines() if s.startswith(v.cxx+' ')];assert len(commands)==1;cmd=commands[0];assert '-DMCNCC_TEST_REAL_EVENTS' in cmd
for required in ['../../game/src/event.cpp','../../game/src/event_queue.cpp','../../game/src/questpc.cpp','../../game/src/questevent.cpp','../../game/src/questmanager.cpp','../../game/src/buffer_manager.cpp','../../game/src/desc_client.cpp','buffer_manager_core.o']:assert required in cmd,required
cmd=[mapping.get(s,s) for s in cmd];assert cmd[-2]=='-o';exe=out/'pc_quest_save_o2.exe';cmd[-1]=str(exe);cmd.insert(-2,'-O2');run('o2-build',cmd);run('o2-runtime',[str(exe)])

