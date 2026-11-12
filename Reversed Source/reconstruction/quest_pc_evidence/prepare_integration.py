"""Workspace-only PC integration renderer. No production write/build mode."""
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
import json,hashlib,re,difflib
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));stage=p/'integration-ready';changes={}
def before(rel):
 q=root/rel;return q.read_bytes() if q.exists() else None
def txt(rel):return before(rel).decode('utf-8-sig').replace('\r\n','\n')
def put(rel,value):
 b=value.encode() if isinstance(value,str) else value;old=before(rel);q=stage/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(b)
 h=lambda x:hashlib.sha256(x).hexdigest()
 changes[rel]={'before_sha256':h(old) if old is not None else None,'after_sha256':h(b)}
 if old is not None:
  (p/('review-'+rel.replace('/','_')+'.diff')).write_text(''.join(difflib.unified_diff(old.decode('utf-8-sig').replace('\r\n','\n').splitlines(True),b.decode().splitlines(True),fromfile='CURRENT/'+rel,tofile='PC/'+rel)))
# Full existing PC data/RewardData preserved; APIs derived from old metadata.
rel='game/src/questpc.h';s=txt(rel)
assert re.search(r'public:\s*PC\(\);\s*~PC\(\);\s*private:',s)
api=(p/'pc-public-api.fragment').read_text().replace('long unsigned int','DWORD').replace('EVENT*','LPEVENT')
api=api.replace('    bool IsLoaded();','    // Old nonconst inline read witnessed by LevelUp.\n    bool IsLoaded() { return m_bLoaded; }')
s=re.sub(r'public:\s*PC\(\);\s*~PC\(\);\s*private:','public:\n'+api+'\nprivate:\n'+(p/'pc-private-api.fragment').read_text(),s,count=1)
s=s.replace('struct TQuestStateChangeInfo { DWORD quest_idx; int prev_state; int next_state; };','struct TQuestStateChangeInfo { DWORD quest_idx; int prev_state; int next_state; TQuestStateChangeInfo(DWORD,int,int); };',1)
s=s.replace('#include <map>','#include <map>\nclass CHARACTER;',1)
assert 'TQuestStateChangeInfo(DWORD,int,int);' in s
put(rel,s)
rel='game/src/questmanager.h';s=txt(rel);assert 'PC* GetPC(' not in s
s=s.replace('    int GetEventFlag(const std::string& name);','    int GetEventFlag(const std::string& name);\n'+(p/'questmanager.h.fragment').read_text(),1)
put(rel,s)
rel='game/src/questmanager.cpp';s=txt(rel);assert 'CQuestManager::CQuestManager(' not in s and 'CQuestManager::GetPC(' not in s
s=s.replace('#include "questmanager.h"','#include "questmanager.h"\n#include "char_manager.h"',1)
end=s.rfind('\n}');assert end>=0
put(rel,s[:end]+'\n'+(p/'questmanager.cpp.fragment').read_text()+s[end:])
for name in ('questpc.cpp','questevent.cpp','questevent.h'):
 rel='game/src/'+name;assert before(rel) is None;put(rel,(p/(name+'.candidate')).read_bytes())
# Retire exactly recovered ctor/PC dtor. Keep strict no-VM/NPC fixture boundary.
rel='reconstruction/tests/quest_flag_fixture.cpp';old=txt(rel)
assert 'CQuestManager::CQuestManager() {}' in old and 'PC::~PC() { std::abort(); }' in old
put(rel,(p/'quest_flag_fixture.cpp.candidate').read_bytes())
# Legacy empty timer registry boundaries explicit; real suite bypasses them.
for name in ('desc_fixture.cpp','entity_packet_fixture.cpp'):
 rel='reconstruction/tests/'+name;s=txt(rel);assert 'void* FindEventInfo(' not in s
 assert 'thecore_shutdown' in s
 put(rel,s+'\n'+(p/'quest_pc_event_boundary.fragment').read_text())
rel='reconstruction/tests/quest_flag_regression.cpp';s=txt(rel);assert 'int max_bytes_written' not in s
s=s.replace('int main() {','int max_bytes_written,current_bytes_written,total_bytes_written;\nint main() {',1);put(rel,s)
for name in ('quest_pc_lookup_regression.cpp','quest_pc_lookup_abi.cpp','quest_pc_lookup_desc_output_syscalls.c'):
 rel='reconstruction/tests/'+name;assert before(rel) is None;put(rel,(p/name).read_bytes())
# Source graph closure everywhere literal original manager source occurs.
rel='reconstruction/tests/Makefile';s=txt(rel);assert 'quest_pc_lookup_regression' not in s
oldnames=next(x for x in s.splitlines() if x.startswith('RUNTIME_TESTS = ')).split('=',1)[1].split()
assert len(oldnames)==95
src='../../game/src/questmanager.cpp';deps=['../../game/src/questpc.cpp','../../game/src/questevent.cpp']
pat=lambda x:r'(?<!\S)'+re.escape(x)+r'(?!\S)'
lines=[]
for line in s.splitlines(True):
 if not line.lstrip().startswith('#') and re.search(pat(src),line):
  for dep in deps:line=re.sub(pat(dep)+r'[ \t]*','',line)
  line=re.sub(pat(src),lambda _:src+' '+' '.join(deps),line)
 lines.append(line)
s=''.join(lines)
# Legacy item recipe already used manager source absent from its prerequisite list.
rows=s.splitlines();j=next(i for i,x in enumerate(rows) if x.startswith('item_regression$(EXEEXT):'))
for dep in [src]+deps:
 if not re.search(pat(dep),rows[j]):rows[j]+=' '+dep
s='\n'.join(rows)+'\n'
# Tiny flag target must use actual FindByPID/CHAR graph now; no stub/LTO.
lines=s.splitlines();i=next(i for i,x in enumerate(lines) if x.startswith('quest_flag_regression$(EXEEXT):'))
assert lines[i+1].startswith('\t$(CXX)')
base='$(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o'
lines[i]='quest_flag_regression$(EXEEXT): quest_flag_regression.cpp '+base
lines[i+1]='\t$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_flag_regression.cpp '+base+' $(LDFLAGS) -o $@'
s='\n'.join(lines)+'\n'
s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = quest_pc_lookup_regression ',1)
s=s.replace('check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n','check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n\t./quest_pc_lookup_regression$(EXEEXT)\n',1)
s=s.replace('check-abi:\n','check-abi:\n\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quest_pc_lookup_abi.cpp\n',1)
graph='$(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_pc_lookup_desc_output_syscalls.o ../../game/src/event.cpp ../../game/src/event_queue.cpp'
s+='\nquest_pc_lookup_regression$(EXEEXT): quest_pc_lookup_regression.cpp '+graph+'\n'
s+='\t$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_pc_lookup_regression.cpp '+graph+' $(LDFLAGS) -o $@\n'
s+='\nquest_pc_lookup_desc_output_syscalls.o: quest_pc_lookup_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h\n'
s+='\t$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c quest_pc_lookup_desc_output_syscalls.c -o $@\n'
i=s.index('\n',s.index('clean:'));s=s[:i+1]+'\t$(RM) quest_pc_lookup_desc_output_syscalls.o\n'+s[i+1:]
newnames=next(x for x in s.splitlines() if x.startswith('RUNTIME_TESTS = ')).split('=',1)[1].split()
assert len(newnames)==96 and newnames[1:]==oldnames
put(rel,s)
(p/'integration-manifest.json').write_text(json.dumps({'root':str(root),'files':changes,'runtime_before':95,'runtime_after':96},indent=2))
print('Staged',len(changes),'files; existing95 names/order preserved, new96th suite. No Fwrites/builds.')
