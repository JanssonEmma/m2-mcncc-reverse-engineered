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
# Workspace-only additive full weighted group renderer. No production writes.
from pathlib import Path
import json,hashlib,shutil
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));stage=p/'integration-ready';changes={}
def original(rel):
 f=root/rel;return f.read_bytes() if f.exists() else None
def text(rel):return original(rel).decode('utf-8-sig').replace('\r\n','\n')
def put(rel,data):
 b=data.encode() if isinstance(data,str) else data;old=original(rel);q=stage/rel;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(b)
 sha=lambda x:hashlib.sha256(x).hexdigest()
 changes[rel]={'before_sha256':sha(old) if old is not None else None,'after_sha256':sha(b)}
def inside_guard(rel,fragment,absent):
 s=text(rel);assert absent not in s
 i=s.rfind('#endif');assert i>=0
 put(rel,s[:i]+(p/fragment).read_text()+'\n'+s[i:])
inside_guard('common/tables.h','tables.h.fragment','struct SItemAttrTable')
inside_guard('game/src/constants.h','constants.h.fragment','extern TItemAttrMap g_map_itemAttr;')
s=text('game/src/constants.cpp');assert 'TItemAttrMap g_map_itemAttr;' not in s
put('game/src/constants.cpp',s+'\n'+(p/'constants.cpp.fragment').read_text())
s=text('game/src/item.h');assert 'void AddAttr(' not in s
assert s.count('private:')==1
put('game/src/item.h',s.replace('private:',(p/'item.h.fragment').read_text()+'\nprivate:',1))
s=text('game/src/item_attribute.cpp');assert 'void CItem::AddAttr(' not in s
s=s.replace('#include "item.h"','#include "item.h"\n#include "constants.h"\n#include "../../libthecore/include/utils.h"\n#include <vector>',1)
put('game/src/item_attribute.cpp',s+'\n'+(p/'item_attribute.cpp.fragment').read_text())
s=text('game/src/item.cpp');assert 'void CItem::AlterToMagicItem(' not in s
assert '#include "constants.h"' in s
put('game/src/item.cpp',s+'\n'+(p/'item.cpp.fragment').read_text())
for name in ('weighted_attribute_regression.cpp','weighted_attribute_abi.cpp','weighted_desc_output_syscalls.c'):
 rel='reconstruction/tests/'+name;assert original(rel) is None;put(rel,(p/name).read_bytes())
rel='reconstruction/tests/Makefile';s=text(rel);assert 'weighted_attribute_regression' not in s
before_names=next(x for x in s.splitlines() if x.startswith('RUNTIME_TESTS = '))
s=s.replace('RUNTIME_TESTS = ','RUNTIME_TESTS = weighted_attribute_regression ',1)
s=s.replace('check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n','check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))\n\t./weighted_attribute_regression$(EXEEXT)\n',1)
s=s.replace('check-abi:\n','check-abi:\n\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only weighted_attribute_abi.cpp\n',1)
lines=s.splitlines();target=next(x for x in lines if x.startswith('item_attribute_regression$(EXEEXT):'));recipe=lines[lines.index(target)+1]
target=target.replace('item_attribute_regression','weighted_attribute_regression').replace('item_attribute_desc_output_syscalls','weighted_desc_output_syscalls')
recipe=recipe.replace('item_attribute_regression','weighted_attribute_regression').replace('item_attribute_desc_output_syscalls','weighted_desc_output_syscalls')
assert '../../game/src/item_addon.cpp' in recipe and '../../game/src/item_attribute.cpp' in recipe and '-DMCNCC_TEST_REAL_EVENTS' in recipe
s+='\n'+target+'\n'+recipe+'\n\n'
s+='weighted_desc_output_syscalls.o: weighted_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h\n'
s+='\t$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c weighted_desc_output_syscalls.c -o $@\n'
i=s.index('\n',s.index('clean:'));s=s[:i+1]+'\t$(RM) weighted_desc_output_syscalls.o\n'+s[i+1:]
after_names=next(x for x in s.splitlines() if x.startswith('RUNTIME_TESTS = '))
assert after_names.replace('weighted_attribute_regression ','',1)==before_names
put(rel,s)
(p/'manifest.json').write_text(json.dumps({'root':str(root),'files':changes},indent=2))
shutil.copyfile(p.parent/'item_attributes/apply_guarded.py',p/'apply_guarded.py')
print('Staged',len(changes),'files; no production writes/builds.')
