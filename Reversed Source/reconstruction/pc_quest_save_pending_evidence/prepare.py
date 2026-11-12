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
import json,hashlib,shutil
from pathlib import Path
p=Path(__file__).resolve().parent;root=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'));h=lambda q:hashlib.sha256(q.read_bytes()).hexdigest()
src=root/'game/src/questpc.cpp';tab=root/'common/tables.h';before=src.read_text(encoding='utf-8');beforetab=tab.read_text(encoding='utf-8');fragment=(p/'questpc.cpp.fragment').read_text(encoding='utf-8-sig');table=(p/'tables.h.fragment').read_text(encoding='utf-8-sig')
assert 'void PC::Save()' not in before and before.rstrip().endswith('}')
assert 'SQuestTable' not in beforetab and beforetab.rstrip().endswith('#endif')
new='#include "../../common/tables.h"\n#include "desc_client.h"\n#include <cstring>\n'+before.rstrip()[:-1]+fragment+'\n}\n'
newtab=beforetab.rstrip()[:-len('#endif')]+table+'\n#endif\n'
(p/'questpc.cpp.candidate').write_text(new,encoding='utf-8');(p/'tables.h.candidate').write_text(newtab,encoding='utf-8')
for name in ['08060770-decompile.json','08060770-decompile.c','08060770-calls.json']:shutil.copyfile(Path('work/quest_level')/name,p/name)
manifest={'target_sha256':'61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c','status':'prepared_only_uncompiled','function':{'name':'quest::PC::Save','address':'08060770'},'assembly_complete':{'instructions':280,'has_more':False},'record':{'name':'SQuestTable','typedef':'TQuestTable','size':106,'owner':'common/tables.h','fields':[{'name':'dwPID','offset':0,'type':'DWORD=unsigned long32'},{'name':'szName','offset':4,'type':'char[33]'},{'name':'szState','offset':37,'type':'char[65]'},{'name':'lValue','offset':102,'type':'long32'}]},'files':[{'path':'game/src/questpc.cpp','before':h(src),'candidate':'questpc.cpp.candidate','after':h(p/'questpc.cpp.candidate'),'insertion':'additional original-owner includes and complete method immediately before final quest namespace brace'},{'path':'common/tables.h','before':h(tab),'candidate':'tables.h.candidate','after':h(p/'tables.h.candidate'),'insertion':'complete packed original record immediately before include guard endif'}],'dependencies':[{'path':q,'sha256':h(root/q)} for q in ['game/src/questpc.h','game/src/desc_client.h','game/src/desc_client.cpp','libthecore/include/log.h','libthecore/include/typedef.h']],'evidence':{},'limitations':['No builds, project writes, ABI/runtime pass or coverage promotion','Full target32 ABI mandatory; no LP64 type substitution','All application paths are complete; unsafe map/vector/callback domains preserved']}
for q in p.iterdir():
 if q.is_file() and q.name not in ('MANIFEST.json',):manifest['evidence'][q.name]=h(q)
(p/'MANIFEST.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8')
print('Prepared whole Save and full original table; no project changes or builds')
