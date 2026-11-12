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
import hashlib,json
r=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
w=Path('work/quest_global_startup')
payload={}
def src(rel): return (r/rel).read_text()
def add(rel,text): payload[rel]=text
h='game/src/questmanager.h'
old=src(h)
assert 'GetServerTimerArg' not in old
add(h,old.replace('    void LoadTimerScript(', '    void LoadTimerScript(') if False else old.replace('    void Destroy();','    void Destroy();\n    DWORD GetServerTimerArg();',1))
p='game/src/questmanager.cpp'
add(p,src(p)+'\nnamespace quest {\nDWORD CQuestManager::GetServerTimerArg() { return m_dwServerTimerArg; }\n}\n')
p='game/src/questlua.cpp'
fragment=(w/'questlua.cpp.fragment').read_text()
fragment=fragment.replace('    CQuestManager::Instance().AddScript(output.str() + "[ENTER]");','    const std::string script = output.str() + "[ENTER]";\n    CQuestManager::Instance().AddScript(script);')
fragment=fragment.replace('        const int style = LuaTruncateInt(rint(lua_tonumber(state, -1)));\n        CQuestManager::Instance().SetSkinStyle(style);','        const double rounded = rint(lua_tonumber(state, -1));\n        CQuestManager& manager = CQuestManager::Instance();\n        manager.SetSkinStyle(LuaTruncateInt(rounded));')
add(p,'#include <climits>\n#include <cstdio>\n#include <fenv.h>\n#include <math.h>\n#include "char.h"\n#include "locale_service.h"\n#include "../../libthecore/include/utils.h"\n'+src(p)+'\n'+fragment)
p='reconstruction/tests/quest_lua_queries_regression.cpp'
text=src(p).replace('#include "../../game/src/config.h"','#include "../../game/src/config.h"\n#include "../../game/src/locale_service.h"')
text=text.replace('int main(){',(w/'tests.fragment').read_text()+'int main(){',1)
text=text.replace(' TestStartupHelpers(qm);',' TestStartupHelpers(qm);\n TestDirectStartupCallbacks(qm);',1)
add(p,text)
p='reconstruction/tests/quest_lua_queries_desc_output_syscalls.c'
text=src(p).replace('#define sys_log','#define random quest_lua_unused_random\n#define sys_log',1)
text=text.replace('#undef sys_log','#undef sys_log\n#undef random\n\nlong random(void) { return 23; }\nextern void quest_lua_callback_error(const char*, int, const char*);',1)
text=text.replace(' else quest_lua_unused_sys_err(function,line,format);',''' else if (!strcmp(function, "_set_skin") || !strcmp(function, "_raw_script")) {
     assert(!strcmp(format, "%s"));
     quest_lua_callback_error(function, line, va_arg(args, const char*));
 }
 else quest_lua_unused_sys_err(function,line,format);''',1)
add(p,text)
rows=[]
sha=lambda b:hashlib.sha256(b).hexdigest()
for rel,data in payload.items():
    dest=w/'payload'/rel
    dest.parent.mkdir(parents=True,exist_ok=True)
    new=data.encode()
    dest.write_bytes(new)
    rows.append(dict(path=rel,before=sha((r/rel).read_bytes()),after=sha(new)))
(w/'apply-plan.json').write_text(json.dumps(rows,indent=2)+'\n')
print(len(rows),'guarded changes staged')
