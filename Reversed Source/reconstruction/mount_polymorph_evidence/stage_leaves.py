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
import json
p=Path(__file__).parent
src=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
guild=src.joinpath('game/src/guild.h').read_text()
assert 'DWORD UnderAnyWar(' not in guild
p.joinpath('candidate-guild.h').write_text(guild.replace('    DWORD GetID()', '    DWORD UnderAnyWar(BYTE type);\n    DWORD GetID()'))
q=src.joinpath('game/src/questmanager.h').read_text()
assert 'void Unmount(' not in q
p.joinpath('candidate-questmanager.h').write_text(q.replace('    CQuestManager();','    CQuestManager();\n    void Unmount(unsigned int pc);'))
p.joinpath('guild-under-any-war.cpp').write_text(r"""
#include "guild.h"
//080add20. Independent actual read-only map leaf; ctor uncertainty is separate.
DWORD CGuild::UnderAnyWar(BYTE type) {
    for(std::map<DWORD,TGuildWar>::iterator it=m_EnemyGuild.begin();it!=m_EnemyGuild.end();++it)
        if((type>2 || it->second.type==type) && it->second.state==6)
            return it->first;
    return 0;
}
""")
p.joinpath('stop-muyeong-event.cpp').write_text(r"""
#include "char.h"
//08073900: original five-instruction tail jump to actual event_cancel.
void CHARACTER::StopMuyeongEvent() { event_cancel(&m_pkMuyeongEvent); }
""")
p.joinpath('independent-leaves-api-abi.cpp').write_text(r"""
#include "../../game/src/char.h"
#include "../../game/src/guild.h"
#include <cstddef>
typedef DWORD(CGuild::*WarSignature)(BYTE);
typedef void(CHARACTER::*LeafSignature)();
WarSignature actual_war_signature=&CGuild::UnderAnyWar;
LeafSignature actual_muyeong_signature=&CHARACTER::StopMuyeongEvent;
LeafSignature actual_empty_skill_signature=&CHARACTER::ComputeSkillPoints;
typedef char DWord32[sizeof(DWORD)==4?1:-1];
typedef char GuildWarTimeAt0[offsetof(TGuildWar,war_start_time)==0?1:-1];
typedef char GuildWarScoreAt4[offsetof(TGuildWar,score)==4?1:-1];
typedef char GuildWarStateAt8[offsetof(TGuildWar,state)==8?1:-1];
typedef char GuildWarTypeAt12[offsetof(TGuildWar,type)==12?1:-1];
typedef char GuildWarMapAt16[offsetof(TGuildWar,map_index)==16?1:-1];
typedef char GuildWarBytes20[sizeof(TGuildWar)==20?1:-1];
// No whole STL class-size assertions, C++11 type_traits, or new data fields.
""")
p.joinpath('stop_muyeong_regression.cpp').write_text(r"""
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/event.h"
#include "../../libthecore/include/main.h"
#include <cassert>
#include <cstdio>
static HEART testHeart={0,{0,0},{0,0},{0,0},25,0};
LPHEART thecore_heart=&testHeart;
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
static int callbacks;
struct CancelInfo { CHARACTER* character; };
static long Never(LPEVENT) { ++callbacks;return 0; }
static long SelfCancel(LPEVENT current) {
    CancelInfo* info=static_cast<CancelInfo*>(event_info(current));
    assert(current->is_processing && info && info->character);
    DWORD id=current->dwInfoID;
    info->character->StopMuyeongEvent();
    assert(!info->character->m_pkMuyeongEvent && current->is_force_to_end);
    assert(FindEventInfo(id)==info); // Deferred free until callback returns.
    ++callbacks;
    return 3; // Real event_process suppresses reschedule after cancellation.
}
static DWORD Attach(CHARACTER& ch,TEVENTFUNC callback,int when) {
    void* raw=0;DWORD id=__AllocEventInfo(sizeof(CancelInfo),&raw);
    static_cast<CancelInfo*>(raw)->character=&ch;
    ch.m_pkMuyeongEvent=event_create_ex(callback,id,when,const_cast<char*>("stop-muyeong-test"),1);
    assert(ch.m_pkMuyeongEvent && FindEventInfo(id)==raw);
    return id;
}
int main() {
    // Use base desc_output_syscalls observer, not target_walk observer requiring callbacks.
    // Existing character/quest virtual-lifecycle boundaries remain test-only.
    desc_test_allow_clock();quest::CQuestManager quests;CHARACTER character;
    assert(!character.m_pkMuyeongEvent && event_count()==0);
    character.StopMuyeongEvent();assert(!character.m_pkMuyeongEvent);
    DWORD id=Attach(character,Never,10);
    assert(event_count()==1);
    character.StopMuyeongEvent();
    assert(!character.m_pkMuyeongEvent && event_count()==0 && !FindEventInfo(id));
    character.StopMuyeongEvent();assert(event_count()==0);
    testHeart.pulse=20;event_process(20);assert(callbacks==0);
    id=Attach(character,SelfCancel,1);
    testHeart.pulse=21;event_process(21);
    assert(callbacks==1 && !character.m_pkMuyeongEvent && !FindEventInfo(id) && event_count()==0);
    testHeart.pulse=30;event_process(30);assert(callbacks==1);
    event_destroy();
    std::puts("StopMuyeongEvent passed: actual queued/processing cancellation, info ownership, null-handle idempotence");
}
""")
# Explicit reference-guided uncertainty; no synthetic constructor implementation.
p.joinpath('UNDER_ANY_WAR_CONSTRUCTOR_GAP.md').write_text("""SGuildWar(BYTE) is declared in complete old header and DWARF guild.h140; inline only. Both retained inline instances zero war_start_time/score/map_index and set type, but constructor state is overwritten before observation: DeclareWar caller state, ReserveWar9. Supplied reference initializes GUILD_WAR_RECV_DECLARE. That initializer is not recoverable as a binary-proven value from these optimized instances. No constructor body or synthetic test replacement has been added. UnderAnyWar actual method is fully recovered; its populated-map regression remains staged as strategy until actual ctor policy is resolved. Empty-map tests alone would not meaningfully cover it.\\n""")
print('Independent actual bodies/C++03 API gates/event regression staged; no invented SGuildWar ctor.')
