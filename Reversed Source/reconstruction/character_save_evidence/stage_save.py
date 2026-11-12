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
import json,hashlib,re
p=Path(__file__).parent
src=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
h=src.joinpath('game/src/char.h').read_text()
cpp=src.joinpath('game/src/char.cpp').read_text()
assert 'void Save();' not in h and 'void CHARACTER::Save()' not in cpp
needle='    virtual ~CHARACTER();'
assert needle in h
p.joinpath('candidate-char.h').write_text(h.replace(needle,needle+'\n    void Save();'))
body="""//08128b50: register a deferred save; skip guard precedes singleton access.
void CHARACTER::Save() {
    if(!m_bSkipSave) CHARACTER_MANAGER::instance().DelayedSave(this);
}
"""
p.joinpath('character-save-body.inc').write_text(body)
p.joinpath('character-save.cpp').write_text('#include "char.h"\n#include "char_manager.h"\n'+body)
p.joinpath('candidate-char.cpp').write_text(cpp+'\n'+body)
p.joinpath('current-char-manager.h').write_bytes(src.joinpath('game/src/char_manager.h').read_bytes())
p.joinpath('current-char-manager.cpp').write_bytes(src.joinpath('game/src/char_manager.cpp').read_bytes())
for f in p.glob('*-decompile.json'):
 d=json.loads(f.read_text());f.with_suffix('.c').write_text(d['code'])
test=r"""
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/char_manager.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include <cassert>
#include <cstring>
#include <cstdio>
// Reuse established CHARACTER initialization/virtual-lifecycle and clock fixtures.
// Save and DelayedSave must come from actual production/candidate source, never fixtures.
extern "C" void desc_test_allow_clock(void);
static std::vector<unsigned char> Snapshot(const CHARACTER& ch) {
    const unsigned char* begin=reinterpret_cast<const unsigned char*>(&ch);
    return std::vector<unsigned char>(begin,begin+sizeof ch);
}
static void Unchanged(const CHARACTER& ch,const std::vector<unsigned char>& bytes) {
    assert(bytes.size()==sizeof ch && !std::memcmp(&ch,&bytes[0],sizeof ch));
}
int main() {
    desc_test_allow_clock();
    quest::CQuestManager quests;
    CHARACTER first,second;
    assert(!CHARACTER_MANAGER::instance_ptr());
    // Skip must return before even trying to acquire the manager singleton.
    first.m_bSkipSave=true;
    std::vector<unsigned char> snapshot=Snapshot(first);
    first.Save();
    Unchanged(first,snapshot);
    assert(!CHARACTER_MANAGER::instance_ptr());
    {
        CHARACTER_MANAGER manager;
        assert(manager.m_set_pkChrForDelayedSave.empty());
        first.Save();
        assert(manager.m_set_pkChrForDelayedSave.empty());
        Unchanged(first,snapshot);
        // No PID, descriptor, IsPC, or connection filter exists in Save.
        first.m_dwPlayerID=0; first.BindDesc(0); first.m_bSkipSave=false;
        snapshot=Snapshot(first);
        first.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==1);
        assert(manager.m_set_pkChrForDelayedSave.count(&first)==1);
        Unchanged(first,snapshot);
        for(int i=0;i<5;++i) first.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==1);
        Unchanged(first,snapshot);
        // Distinct objects with equal PID are distinct pending saves.
        second.m_dwPlayerID=first.m_dwPlayerID;second.m_bSkipSave=false;
        std::vector<unsigned char> second_snapshot=Snapshot(second);
        second.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==2);
        assert(manager.m_set_pkChrForDelayedSave.count(&first) &&
               manager.m_set_pkChrForDelayedSave.count(&second));
        Unchanged(second,second_snapshot);
        // A later skipped call does not cancel an existing request.
        first.m_bSkipSave=true;snapshot=Snapshot(first);
        first.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==2);
        assert(manager.m_set_pkChrForDelayedSave.count(&first));
        Unchanged(first,snapshot);
        // Existing manager behavior accepts null and retains other entries.
        manager.DelayedSave(0);manager.DelayedSave(0);
        assert(manager.m_set_pkChrForDelayedSave.size()==3 &&
               manager.m_set_pkChrForDelayedSave.count(0));
        second.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==3);
        // Erasing here is explicit test cleanup; Save itself does not process/delete.
        manager.m_set_pkChrForDelayedSave.clear();
        first.Save();assert(manager.m_set_pkChrForDelayedSave.empty());
        first.m_bSkipSave=false;snapshot=Snapshot(first);first.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==1 &&
               manager.m_set_pkChrForDelayedSave.count(&first));
        Unchanged(first,snapshot);
        manager.m_set_pkChrForDelayedSave.clear();
    }
    assert(!CHARACTER_MANAGER::instance_ptr());
    std::puts("CHARACTER::Save passed: skip-before-singleton, actual unique pointer queue, no PC filter, no object mutation or cancellation");
}
"""
p.joinpath('character_save_regression.cpp').write_text(test)
p.joinpath('character_save_abi.cpp').write_text(r"""
#include "../../game/src/char.h"
#include "../../game/src/char_manager.h"
#include <type_traits>
static_assert(sizeof(CHARACTER)==3812,"complete original CHARACTER");
static_assert(sizeof(CHARACTER_MANAGER)==172,"complete original manager");
static_assert(std::is_same<decltype(&CHARACTER::Save),void(CHARACTER::*)()>::value,"Save void nonconst");
static_assert(std::is_same<decltype(&CHARACTER_MANAGER::DelayedSave),void(CHARACTER_MANAGER::*)(LPCHARACTER)>::value,"DelayedSave pointer argument");
""")
# Validate source-only staging without invoking a compiler/build.
ch=p.joinpath('candidate-char.h').read_text()
assert ch.replace('\n    void Save();','',1)==h
assert p.joinpath('candidate-char.cpp').read_text().startswith(cpp)
calls=json.loads(p.joinpath('08128b50-calls.json').read_text())
assert {x['target'] for x in calls['callees']['items']}=={'0818e250','0804c9ac'}
records=json.loads(p.joinpath('full-records-and-save-api.json').read_text())
print(json.dumps([{'record':r['record']['name'],'fields':[m for m in r['members'] if m['name'] in ('m_bSkipSave','m_set_pkChrForDelayedSave')]} for r in records],indent=2))
