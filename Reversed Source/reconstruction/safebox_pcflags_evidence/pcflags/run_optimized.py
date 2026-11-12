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
# Parent-executed optimized focused graph; staging this script does not build.
import subprocess,shlex,json,hashlib,datetime
from pathlib import Path
cwd=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\tests'))
work=Path(__file__).resolve().parent
base=[r'C:\Strawberry\c\bin\gmake.exe','SHELL=cmd.exe','CC=C:/Strawberry/c/bin/gcc.exe','CXX=C:/Strawberry/c/bin/g++.exe']
objects=['item_sql_runtime.o','item_sql_semaphore.o','item_sql_api.o','item_socket_config.o']
mapped={x:x[:-2]+'_pc_flags_timers_o2.o' for x in objects}
commands=[]
def plan(args):
 p=subprocess.run(base+args,cwd=cwd,capture_output=True,text=True,check=True)
 return [shlex.split(s) for s in p.stdout.splitlines() if s.startswith('C:/Strawberry/c/bin/g++.exe ')]
logpath=work/'pc-flags-timers-o2.log'
with logpath.open('w') as log:
 obj=plan(['-Bn']+objects);assert len(obj)==4
 for args in obj:
  assert args[-2]=='-o' and args[-1] in mapped
  args[-1]=mapped[args[-1]];args.insert(1,'-O2');commands.append(args)
  subprocess.run(args,cwd=cwd,stdout=log,stderr=subprocess.STDOUT,check=True)
 links=plan(['-n','-W','../../game/src/item_attribute.cpp','pc_flags_timers_regression'])
 assert len(links)==1
 args=links[0];assert args[-2]=='-o' and args[-1] in ('pc_flags_timers_regression','pc_flags_timers_regression.exe')
 # Both real event translation units must be compiled by this optimized link,
 # never use the default test event lifecycle or stale normal event objects.
 assert '../../game/src/event.cpp' in args and '../../game/src/event_queue.cpp' in args and '-DMCNCC_TEST_REAL_EVENTS' in args
 assert '../../game/src/questpc.cpp' in args and '../../game/src/questevent.cpp' in args and '../../game/src/questmanager.cpp' in args
 args=[mapped.get(x,x) for x in args];args.insert(1,'-O2');args[-1]='pc_flags_timers_o2.exe';commands.append(args)
 subprocess.run(args,cwd=cwd,stdout=log,stderr=subprocess.STDOUT,check=True)
 subprocess.run([str(cwd/'pc_flags_timers_o2.exe')],cwd=cwd,stdout=log,stderr=subprocess.STDOUT,check=True)
sources=['../../game/src/questpc.h','../../game/src/questpc.cpp','../../game/src/questevent.h','../../game/src/questevent.cpp','../../game/src/questmanager.h','../../game/src/questmanager.cpp','../../game/src/event.cpp','../../game/src/event_queue.cpp','quest_flag_fixture.cpp','desc_fixture.cpp','pc_flags_timers_regression.cpp','pc_flags_timers_desc_output_syscalls.c']
receipt={'status':'passed','exit_code':0,'finished_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'commands':commands,'log':str(logpath),'distinct_optimized_SQL_objects':list(mapped.values()),'real_events':True,'source_sha256':{x:hashlib.sha256((cwd/x).read_bytes()).hexdigest() for x in sources}}
(work/'pc-flags-timers-o2.receipt.json').write_text(json.dumps(receipt,indent=2))
print('Optimized actual PC flags/timers/real event graph passed, including four distinct optimized SQL objects and real events.')
