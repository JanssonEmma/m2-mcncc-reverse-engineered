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
# Rebasable workspace-only preparation; parent serializes apply/build.
from pathlib import Path
import json,hashlib,difflib,re
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));stage=p/'integration-ready';changes={}
def old(rel):
 q=root/rel;return q.read_bytes() if q.exists() else None
def text(rel):return old(rel).decode('utf-8-sig').replace('\r\n','\n')
sha=lambda b:hashlib.sha256(b).hexdigest()
def put(rel,data):
 b=data.encode() if isinstance(data,str) else data;before=old(rel);q=stage/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(b)
 changes[rel]={'before_sha256':sha(before) if before is not None else None,'after_sha256':sha(b)}
 if before is not None:(p/('review-'+rel.replace('/','_')+'.diff')).write_text(''.join(difflib.unified_diff(before.decode('utf-8-sig').replace('\r\n','\n').splitlines(True),b.decode().splitlines(True),fromfile='CURRENT/'+rel,tofile='QUESTINDICES/'+rel)))
rel='game/src/questmanager.h';s=text(rel)
assert 'GetQuestIndexByName' not in s and 'GetQuestNameByIndex' not in s and 'FindNPCIDByName' not in s and 'GetCurrentQuestName' not in s
s=s.replace('#include <set>','#include <set>\n#include <cstddef>',1)
anchor='    PC* GetPCForce(unsigned int pc);';assert s.count(anchor)==1
s=s.replace(anchor,anchor+'\n    unsigned int GetQuestIndexByName(const std::string& name);\n    const std::string& GetQuestNameByIndex(unsigned int index);\n    unsigned int FindNPCIDByName(const std::string& name);\n    const std::string& GetCurrentQuestName();',1)
anchor='struct stringhash { std::size_t operator()(const std::string&) const; };';assert s.count(anchor)==1
s=s.replace(anchor,(p/'stringhash.inline.fragment').read_text().strip(),1);put(rel,s)
rel='game/src/questmanager.cpp';s=text(rel)
for name in ('GetQuestIndexByName','GetQuestNameByIndex','FindNPCIDByName','GetCurrentQuestName'):assert not re.search(r'CQuestManager::'+name+r'\s*\(',s)
s=s.replace('#include "char_manager.h"','#include "char_manager.h"\n#include "../../libthecore/include/log.h"\n#include <cassert>',1)
i=s.rfind('\n}');assert i>=0;put(rel,s[:i]+'\n'+(p/'questmanager.cpp.fragment').read_text()+s[i:])
for name in ('quest_manager_indices_regression.cpp','quest_manager_indices_abi.cpp','quest_manager_indices_desc_fixture.cpp','quest_manager_indices_desc_output_syscalls.c'):
 rel='reconstruction/tests/'+name;assert old(rel) is None;put(rel,(p/name).read_bytes())
rel='reconstruction/tests/Makefile';s=text(rel);assert 'quest_manager_indices_regression' not in s
names=lambda t:next(x for x in t.splitlines() if x.startswith('RUNTIME_TESTS = ')).split('=',1)[1].split()
ns=names(s);s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = quest_manager_indices_regression ',1)
s=s.replace('check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n','check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n\t./quest_manager_indices_regression$(EXEEXT)\n',1)
s=s.replace('check-abi:\n','check-abi:\n\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quest_manager_indices_abi.cpp\n',1)
rows=s.splitlines();i=next(i for i,x in enumerate(rows) if x.startswith('quest_pc_lookup_regression$(EXEEXT):'));lines=[rows[i],rows[i+1]]
for i,line in enumerate(lines):
 lines[i]=line.replace('quest_pc_lookup_regression','quest_manager_indices_regression').replace('quest_pc_lookup_desc_output_syscalls.o','quest_manager_indices_desc_output_syscalls.o').replace('$(TARGET_WALKING_SOURCES)','$(filter-out desc_fixture.cpp,$(TARGET_WALKING_SOURCES)) quest_manager_indices_desc_fixture.cpp')
assert '-DMCNCC_TEST_REAL_EVENTS' in lines[1] and '../../game/src/event.cpp' in lines[1]
lines[0]+=' ../../game/src/questmanager.h desc_fixture.cpp'
s+='\n'+'\n'.join(lines)+'\n\n'
s+='quest_manager_indices_desc_output_syscalls.o: quest_manager_indices_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h\n\t$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c quest_manager_indices_desc_output_syscalls.c -o $@\n'
i=s.index('\n',s.index('clean:'));s=s[:i+1]+'\t$(RM) quest_manager_indices_desc_output_syscalls.o\n'+s[i+1:];assert names(s)==['quest_manager_indices_regression']+ns;put(rel,s)
support={rel:sha(old(rel)) for rel in ('reconstruction/tests/desc_fixture.cpp','reconstruction/tests/desc_output_syscalls.c','reconstruction/tests/quest_flag_fixture.cpp')}
(p/'integration-manifest.json').write_text(json.dumps({'root':str(root),'files':changes,'support_read_guards':support,'runtime_before':len(ns),'runtime_after':len(ns)+1,'baseline_policy':'Re-render CURRENT after parent/other agent guarded changes. Existing runtime order unchanged.'},indent=2));print('Staged',len(changes),'files;',len(ns),'->',len(ns)+1,'canonical runtimes; no Fwrites/builds.')
