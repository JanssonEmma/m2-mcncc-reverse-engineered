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
import json,hashlib,difflib,re
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));stage=p/'integration-ready';changes={}
sha=lambda b:hashlib.sha256(b).hexdigest()
def old(rel):
 q=root/rel;return q.read_bytes() if q.exists() else None
def text(rel):return old(rel).decode('utf-8-sig').replace('\r\n','\n')
def put(rel,data):
 b=data.encode() if isinstance(data,str) else data;a=old(rel);q=stage/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(b);changes[rel]={'before_sha256':sha(a) if a is not None else None,'after_sha256':sha(b)}
 if a is not None:(p/('review-'+rel.replace('/','_')+'.diff')).write_text(''.join(difflib.unified_diff(a.decode('utf-8-sig').replace('\r\n','\n').splitlines(True),b.decode().splitlines(True),fromfile='CURRENT/'+rel,tofile='REGISTRATION/'+rel)))
rel='game/src/questmanager.h';s=text(rel);assert 'GetQuestIndexByName' in s and 'LoadStartQuest' not in s and 'RegisterQuest' not in s
anchor='    unsigned int GetQuestIndexByName(const std::string& name);';assert s.count(anchor)==1;s=s.replace(anchor,'    void LoadStartQuest(const std::string& name,unsigned int index);\n    void RegisterQuest(const std::string& name,unsigned int index);\n'+anchor,1);put(rel,s)
rel='game/src/questmanager.cpp';s=text(rel)
for name in ('LoadStartQuest','RegisterQuest'):assert not re.search(r'CQuestManager::'+name+r'\s*\(',s)
anchor='#include <cassert>';assert anchor in s;s=s.replace(anchor,anchor+'\n#include "config.h"\n#include <fstream>\n#include <iterator>\n#include <algorithm>',1);i=s.rfind('\n}');assert i>=0;put(rel,s[:i]+'\n'+(p/'questmanager.cpp.fragment').read_text(encoding='utf-8')+s[i:])
for name in ('quest_registration_regression.cpp','quest_registration_abi.cpp','quest_registration_desc_output_syscalls.c'):
 rel='reconstruction/tests/'+name;assert old(rel) is None;put(rel,(p/name).read_bytes())
rel='reconstruction/tests/Makefile';s=text(rel);assert 'quest_registration_regression' not in s
names=lambda t:next(x for x in t.splitlines() if x.startswith('RUNTIME_TESTS = ')).split('=',1)[1].split();ns=names(s)
s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = quest_registration_regression ',1)
s=s.replace('check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n','check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n\t./quest_registration_regression$(EXEEXT)\n',1)
s=s.replace('check-abi:\n','check-abi:\n\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quest_registration_abi.cpp\n',1)
rows=s.splitlines();i=next(i for i,x in enumerate(rows) if x.startswith('quest_pc_lookup_regression$(EXEEXT):'));lines=[rows[i],rows[i+1]]
for i,line in enumerate(lines):lines[i]=line.replace('quest_pc_lookup_regression','quest_registration_regression').replace('quest_pc_lookup_desc_output_syscalls.o','quest_registration_desc_output_syscalls.o')
assert '-DMCNCC_TEST_REAL_EVENTS' in lines[1];lines[0]+=' ../../game/src/questmanager.h ../../game/src/config.h';s+='\n'+'\n'.join(lines)+'\n\n'
s+='quest_registration_desc_output_syscalls.o: quest_registration_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h\n\t$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c quest_registration_desc_output_syscalls.c -o $@\n'
i=s.index('\n',s.index('clean:'));s=s[:i+1]+'\t$(RM) quest_registration_desc_output_syscalls.o\n'+s[i+1:];assert names(s)==['quest_registration_regression']+ns;put(rel,s)
support={r:sha(old(r)) for r in ('game/src/config.h','game/src/config.cpp','reconstruction/tests/desc_fixture.cpp','reconstruction/tests/desc_output_syscalls.c','reconstruction/tests/quest_flag_fixture.cpp')}
(p/'integration-manifest.json').write_text(json.dumps({'root':str(root),'files':changes,'support_read_guards':support,'runtime_before':len(ns),'runtime_after':len(ns)+1,'baseline_policy':'Rerender after parent PC storage current-tree edits; no frozen sibling payload modifications.'},indent=2));print('Staged',len(changes),'files;',len(ns),'->',len(ns)+1,'canonical runtimes; no Fwrites/builds.')
