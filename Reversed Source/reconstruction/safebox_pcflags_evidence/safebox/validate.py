# Parent-only validation helper. Child does not build or apply.
from pathlib import Path
import argparse,subprocess,shlex,json,datetime
p=Path(__file__).resolve().parent;a=argparse.ArgumentParser();a.add_argument('--root',required=True);a.add_argument('--out');a.add_argument('--cxx',default='C:/Strawberry/c/bin/g++.exe');a.add_argument('--cc',default='C:/Strawberry/c/bin/gcc.exe');a.add_argument('--make',default='C:/Strawberry/c/bin/gmake.exe');v=a.parse_args();cwd=Path(v.root)/'reconstruction/tests';out=Path(v.out) if v.out else p/'validation';out.mkdir(parents=True,exist_ok=True);base=[v.make,'SHELL=cmd.exe','CC='+v.cc,'CXX='+v.cxx]
def run(name,cmd):
 log=out/(name+'.log');r={'started_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'command':cmd,'cwd':str(cwd),'log':str(log)}
 with log.open('w') as f:q=subprocess.run(cmd,cwd=cwd,stdout=f,stderr=subprocess.STDOUT)
 r.update(status='passed' if q.returncode==0 else 'failed',exit_code=q.returncode,finished_utc=datetime.datetime.now(datetime.timezone.utc).isoformat());(out/(name+'.receipt.json')).write_text(json.dumps(r,indent=2))
 if q.returncode:raise SystemExit(q.returncode)
run('strict32',[v.cxx,'-Ifdwatch_shim','-Isocket_shim','-Iposix_shim','-std=c++03','-Wall','-Wextra','-pedantic-errors','-Wno-long-long','-Wno-invalid'+'-offsetof','-m32','-fsyntax-only','safebox_closed_abi.cpp','../../game/src/safebox.cpp'])
objects=['item_sql_runtime.o','item_sql_semaphore.o','item_sql_api.o','item_socket_config.o','safebox_desc_output_syscalls.o'];mapping={};plan=subprocess.run(base+['-Bn']+objects,cwd=cwd,capture_output=True,text=True,check=True)
for line in plan.stdout.splitlines():
 if not (line.startswith(v.cxx+' ') or line.startswith(v.cc+' ')):continue
 cmd=shlex.split(line);assert cmd[-2]=='-o' and cmd[-1] in objects;old=cmd[-1];new=out/(old[:-2]+'_o2.o');mapping[old]=str(new);cmd[-1]=str(new);cmd.insert(-2,'-O2');run(old[:-2]+'-o2',cmd)
assert set(mapping)==set(objects)
plan=subprocess.run(base+['-n','-W','safebox_closed_regression.cpp','safebox_closed_regression'],cwd=cwd,capture_output=True,text=True,check=True);commands=[shlex.split(x) for x in plan.stdout.splitlines() if x.startswith(v.cxx+' ')];assert len(commands)==1;cmd=[mapping.get(x,x) for x in commands[0]];assert cmd[-2]=='-o';exe=out/'safebox_closed_o2.exe';cmd[-1]=str(exe);cmd.insert(-2,'-O2');run('o2-build',cmd);run('o2-runtime',[str(exe)])
