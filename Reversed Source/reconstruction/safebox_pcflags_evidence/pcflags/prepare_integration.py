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
# Workspace-only additive PC flag/timer package; rebase after root88/Safebox89.
from pathlib import Path
import json,hashlib,difflib,re
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));stage=p/'integration-ready';changes={}
def old(rel):
 q=root/rel;return q.read_bytes() if q.exists() else None
def text(rel):return old(rel).decode('utf-8-sig').replace('\r\n','\n')
def put(rel,data):
 b=data.encode() if isinstance(data,str) else data;before=old(rel);q=stage/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(b)
 sha=lambda v:hashlib.sha256(v).hexdigest()
 changes[rel]={'before_sha256':sha(before) if before is not None else None,'after_sha256':sha(b)}
 if before is not None:(p/('review-'+rel.replace('/','_')+'.diff')).write_text(''.join(difflib.unified_diff(before.decode('utf-8-sig').replace('\r\n','\n').splitlines(True),b.decode().splitlines(True),fromfile='CURRENT/'+rel,tofile='PCSTORAGE/'+rel)))
rel='game/src/questpc.cpp';s=text(rel)
for method in ('SetFlag','GetFlag','DeleteFlag','SaveFlag','AddTimer','RemoveTimer','RemoveTimerNotCancel'):
 assert not re.search(r'PC::'+method+r'\s*\(',s),method
s=s.replace('#include "questevent.h"','#include "questevent.h"\n#include "config.h"',1)
i=s.rfind('\n}');assert i>=0
put(rel,s[:i]+'\n'+(p/'questpc.cpp.fragment').read_text()+s[i:])
for name in ('pc_flags_timers_regression.cpp','pc_flags_timers_abi.cpp','pc_flags_timers_desc_output_syscalls.c'):
 rel='reconstruction/tests/'+name;assert old(rel) is None;put(rel,(p/name).read_bytes())
rel='reconstruction/tests/Makefile';s=text(rel);assert 'pc_flags_timers_regression' not in s
names=lambda t:next(x for x in t.splitlines() if x.startswith('RUNTIME_TESTS = ')).split('=',1)[1].split()
before_names=names(s)
s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = pc_flags_timers_regression ',1)
s=s.replace('check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n','check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n\t./pc_flags_timers_regression$(EXEEXT)\n',1)
s=s.replace('check-abi:\n','check-abi:\n\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only pc_flags_timers_abi.cpp\n',1)
rows=s.splitlines();i=next(i for i,x in enumerate(rows) if x.startswith('quest_pc_lookup_regression$(EXEEXT):'))
target=rows[i].replace('quest_pc_lookup_regression','pc_flags_timers_regression').replace('quest_pc_lookup_desc_output_syscalls','pc_flags_timers_desc_output_syscalls')
recipe=rows[i+1].replace('quest_pc_lookup_regression','pc_flags_timers_regression').replace('quest_pc_lookup_desc_output_syscalls','pc_flags_timers_desc_output_syscalls')
assert '-DMCNCC_TEST_REAL_EVENTS' in recipe and '../../game/src/event.cpp' in recipe and '../../game/src/event_queue.cpp' in recipe
s+='\n'+target+'\n'+recipe+'\n\n'
s+='pc_flags_timers_desc_output_syscalls.o: pc_flags_timers_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h\n'
s+='\t$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c pc_flags_timers_desc_output_syscalls.c -o $@\n'
i=s.index('\n',s.index('clean:'));s=s[:i+1]+'\t$(RM) pc_flags_timers_desc_output_syscalls.o\n'+s[i+1:]
assert names(s)==['pc_flags_timers_regression']+before_names
put(rel,s)
(p/'integration-manifest.json').write_text(json.dumps({'root':str(root),'files':changes,'runtime_before':len(before_names),'runtime_after':len(before_names)+1,'baseline_policy':'Rerender after parent88 and Safebox89 Makefile changes; no fixed stale snapshot.'},indent=2))
print('Staged',len(changes),'files;',len(before_names),'existing names/order preserved; no Fwrites/builds.')
