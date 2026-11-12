#include <map>
#include <string>
#include <vector>
#define private public
#include "../../game/src/questpc.h"
#undef private
void CheckPCStateAPI() {
    bool (quest::PC::*running)()=&quest::PC::IsRunning;
    quest::QuestState* (quest::PC::*state)()=&quest::PC::GetRunningQuestState;
    bool (quest::PC::*reward)()=&quest::PC::HasReward;
    void (quest::PC::*done)()=&quest::PC::SetSendDoneFlag;
    bool (quest::PC::*reset)()=&quest::PC::GetAndResetDoneFlag;
    void (quest::PC::*set)(DWORD)=&quest::PC::SetConfirmWait;
    void (quest::PC::*clear)()=&quest::PC::ClearConfirmWait;
    bool (quest::PC::*wait)(DWORD)=&quest::PC::IsConfirmWait;
    void (quest::PC::*bits)(int)=&quest::PC::SetSendFlag;
    (void)running;(void)state;(void)reward;(void)done;(void)reset;(void)set;(void)clear;(void)wait;(void)bits;
}
typedef char state_int32[sizeof(int)==4?1:-1];
// C++03 typed signatures; no whole STL-containing class size assumptions.
