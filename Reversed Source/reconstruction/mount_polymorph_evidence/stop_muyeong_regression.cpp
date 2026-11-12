
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
#include <cstdlib>
// TEST ONLY fail-fast boundary for unrelated server shutdown.
void ShutdownOnFatalError() { std::abort(); }
static HEART testHeart={0,{0,0},{0,0},{0,0},25,0};
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
static int callbacks,forced_diagnostics;
extern "C" void (*desc_test_event_force_error)(void);
static void ExpectedForceDiagnostic() { assert(callbacks==1); ++forced_diagnostics; }
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
    LPHEART previousHeart=thecore_heart;thecore_heart=&testHeart;
    desc_test_allow_clock();quest::CQuestManager quests;CHARACTER character;
    assert(!character.m_pkMuyeongEvent && event_count()==0);
    character.StopMuyeongEvent();assert(!character.m_pkMuyeongEvent);
    DWORD id=Attach(character,Never,10);
    assert(event_count()==1);
    character.StopMuyeongEvent();
    assert(!character.m_pkMuyeongEvent && event_count()==1 && !FindEventInfo(id));
    character.StopMuyeongEvent();assert(event_count()==1);
    testHeart.pulse=20;event_process(20);assert(callbacks==0 && event_count()==0);
    id=Attach(character,SelfCancel,1);
    desc_test_event_force_error=ExpectedForceDiagnostic;
    testHeart.pulse=21;event_process(21);
    desc_test_event_force_error=0;assert(forced_diagnostics==1);
    assert(callbacks==1 && !character.m_pkMuyeongEvent && !FindEventInfo(id) && event_count()==0);
    testHeart.pulse=30;event_process(30);assert(callbacks==1);
    event_destroy();
    thecore_heart=previousHeart; // Existing DESC fixture owns the global definition.
    std::puts("StopMuyeongEvent passed: actual queued/processing cancellation, info ownership, null-handle idempotence");
}
