# Parent-only validation helper. Child never runs it or builds production.
from pathlib import Path
import argparse,json,datetime,subprocess,shlex
p=Path(__file__).resolve().parent;a=argparse.ArgumentParser();a.add_argument('--root',required=True);a.add_argument('--cxx',default='C:/Strawberry/c/bin/g++.exe');a.add_argument('--cc',default='C:/Strawberry/c/bin/gcc.exe');a.add_argument('--make',default='C:/Strawberry/c/bin/gmake.exe');v=a.parse_args();cwd=Path(v.root)/'reconstruction/tests';out=p/'validation';out.mkdir(exist_ok=True);base=[v.make,'SHELL=cmd.exe','CC='+v.cc,'CXX='+v.cxx]
def run(name,cmd):
 log=out/(name+'.log');r={'started_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'cwd':str(cwd),'command':cmd,'log':str(log)}
 with log.open('w') as f:q=subprocess.run(cmd,cwd=cwd,stdout=f,stderr=subprocess.STDOUT)
 r.update(status='passed' if q.returncode==0 else 'failed',exit_code=q.returncode,finished_utc=datetime.datetime.now(datetime.timezone.utc).isoformat());(out/(name+'.receipt.json')).write_text(json.dumps(r,indent=2))
 if q.returncode:raise SystemExit(q.returncode)
run('strict32',[v.cxx,'-Ifdwatch_shim','-Isocket_shim','-Iposix_shim','-std=c++03','-Wall','-Wextra','-pedantic-errors','-Wno-long-long','-m32','-fsyntax-only','affect_packet_leaves_abi.cpp','../../game/src/char_affect.cpp','../../game/src/char_battle.cpp'])
objects=['item_sql_runtime.o','item_sql_semaphore.o','item_sql_api.o','item_socket_config.o','desc_output_syscalls.o'];mapping={};plan=subprocess.run(base+['-Bn']+objects,cwd=cwd,capture_output=True,text=True,check=True)
for line in plan.stdout.splitlines():
 if not (line.startswith(v.cxx+' ') or line.startswith(v.cc+' ')):continue
 cmd=shlex.split(line);assert cmd[-2]=='-o' and cmd[-1] in objects;old=cmd[-1];new=out/(old[:-2]+'_o2.o');mapping[old]=str(new);cmd[-1]=str(new);cmd.insert(-2,'-O2');run(old[:-2]+'-o2',cmd)
assert set(mapping)==set(objects)
plan=subprocess.run(base+['-n','-W','affect_packet_leaves_regression.cpp','affect_packet_leaves_regression'],cwd=cwd,capture_output=True,text=True,check=True);commands=[shlex.split(l) for l in plan.stdout.splitlines() if l.startswith(v.cxx+' ')];assert len(commands)==1;cmd=[mapping.get(s,s) for s in commands[0]];assert cmd[-2]=='-o';exe=out/'affect_packet_leaves_o2.exe';cmd[-1]=str(exe);cmd.insert(-2,'-O2');run('o2-build',cmd);run('o2-runtime',[str(exe)])
