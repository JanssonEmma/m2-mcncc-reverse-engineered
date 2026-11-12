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
import json,hashlib
r=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
w=Path('work/npc_set_callers')
files={}
for name in ('questmanager.cpp','questmanager.h'):
    rel=f'game/src/{name}'
    text=(r/rel).read_text()
    frag=(w/(name+'.fragment')).read_text()
    if name.endswith('.cpp'):
        text='#include <climits>\n#include <dirent.h>\n'+text
        text+='\n'+frag[frag.index('namespace quest'):].strip()+'\n'
        assert '#include "config.h"' in text
    else:
        assert text.count('    void Destroy();')==1
        text=text.replace('    void Destroy();','    void Destroy();\n'+frag.rstrip())
    files[rel]=text
rel='reconstruction/tests/quest_npc_set_regression.cpp'
text=(r/rel).read_text()
text=text.replace('#include <cassert>','#include <cassert>\n#include <climits>')
text=text.replace('extern "C" int npc_set_loaded_count(void);','extern "C" int npc_set_loaded_count(void);\nextern "C" void npc_set_expect_tree(const char* name);')
start=text.index('    const char* roots[]')
end=text.index('    g_setQuestObjectDir.clear();',start)
block=text[start:end]
block=block.replace('    const char* roots[]', 'static const char* roots[]').replace('    const char* events[]','static const char* events[]')
split=block.index('    for (int root')
helper=block[:split]+'\nstatic void WriteTree(const std::string& name) {\n'+block[split:].replace('"/script"','"/" + name').replace('"/script/"','"/" + name + "/"')+'}\n\n'
cleanup_start=text.index('    for (int root',end)
cleanup_end=text.index('    g_setQuestObjectDir.clear();',cleanup_start)
cleanup=text[cleanup_start:cleanup_end].replace('"/script"','"/" + name').replace('"/script/"','"/" + name + "/"')
helper+='static void RemoveTree(const std::string& name) {\n'+cleanup+'}\n\n'
text=text[:cleanup_start]+'''    npc_set_expect_tree("script");
    manager.m_mapNPC[UINT_MAX].m_vnum = 1;
    assert(manager.LoadTimerScript("script") == UINT_MAX);
    assert(manager.m_mapNPC[UINT_MAX].m_vnum == UINT_MAX);
    assert(manager.LoadTimerScript("script") == UINT_MAX && npc_set_loaded_count() == 4);
    assert(manager.LoadTimerScript("missing") == UINT_MAX - 1);
    assert(manager.m_mapTimerID.size() == 2);

    manager.RegisterNPCVnum(315);
    assert(manager.m_registeredNPCVnum.count(315) == 1 && manager.m_mapNPC.count(315) == 0);
    MakeDirectory(std::string(roots[0]) + "/315");
    manager.RegisterNPCVnum(315);
    assert(manager.m_mapNPC.count(315) == 0);
    RemoveDirectory(std::string(roots[0]) + "/315");

    WriteTree("314");
    npc_set_expect_tree("314");
    manager.RegisterNPCVnum(314);
    assert(manager.m_registeredNPCVnum.count(314) == 1 && npc_set_loaded_count() == 8);
    const std::vector<char>& registered = manager.m_mapNPC[314].m_mapOwnQuest[5][42][8].m_code;
    assert(std::string(registered.begin(), registered.end()) == "ABCDABCD");
    assert(manager.m_mapNPCNameID.empty());
    manager.RegisterNPCVnum(314);
    assert(npc_set_loaded_count() == 8);
    RemoveTree("314");
    RemoveTree("script");
'''+text[cleanup_end:]
text=text[:start]+'    WriteTree("script");\n'+text[end:]
text=text.replace('int main() {',helper+'int main() {')
text=text.replace('        MakeDirectory(base);','        if (name == "script") MakeDirectory(base);')
text=text.replace('        RemoveDirectory(base);','        if (name == "script") RemoveDirectory(base);')
files[rel]=text

rel='reconstruction/tests/quest_npc_set_desc_output_syscalls.c'
text=(r/rel).read_text()
i=text.index('void sys_log(')
j=text.index('    va_list args;',i)
text=text[:i]+'''static const char* tree = "script";
void npc_set_expect_tree(const char* name) {
    assert(phase == 0);
    tree = name;
    loaded = 0;
}

void sys_log(int level, const char* format, ...) {
    const int slot = loaded % 4;
    char path[100];
    sprintf(path, "npc_set_fixture_%c/%s/%s/q.start", 'a' + slot % 2,
            tree, slot < 2 ? "alpha" : "beta");
'''+text[j:]
text=text.replace(' && loaded < 4','').replace('paths[loaded]','path')
files[rel]=text

ledger=[]
for rel,text in files.items():
    p=r/rel
    before=p.read_bytes()
    payload=text.encode()
    staged=w/'payload'/rel
    staged.parent.mkdir(parents=True,exist_ok=True)
    staged.write_bytes(payload)
    ledger.append({'path':rel,'before':hashlib.sha256(before).hexdigest(),'after':hashlib.sha256(payload).hexdigest()})
(w/'apply-plan.json').write_text(json.dumps(ledger,indent=2)+'\n')
print('Staged caller source and extended existing traversal regression')
