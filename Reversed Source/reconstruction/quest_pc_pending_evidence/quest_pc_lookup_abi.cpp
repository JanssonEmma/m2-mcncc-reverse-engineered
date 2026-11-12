#include "../../game/src/questmanager.h"
#include "../../game/src/questevent.h"
#include <cstddef>
typedef char timer_size[sizeof(quest::quest_event_info)==16?1:-1];
typedef char server_timer_size[sizeof(quest::quest_server_event_info)==16?1:-1];
typedef char timer_name_offset[offsetof(quest::quest_event_info,name)==12?1:-1];
typedef char reward_size[sizeof(quest::RewardData)==12?1:-1];
void CheckPCAPI() {
    void (quest::PC::*set)(DWORD)=&quest::PC::SetID;
    void (quest::PC::*destroy)()=&quest::PC::Destroy;
    void (quest::PC::*clear)()=&quest::PC::ClearTimer;
    bool (quest::PC::*loaded)()=&quest::PC::IsLoaded;
    const std::string& (quest::PC::*name)()=&quest::PC::GetCurrentQuestName;
    quest::PC* (quest::CQuestManager::*get)(unsigned int)=&quest::CQuestManager::GetPC;
    quest::PC* (quest::CQuestManager::*force)(unsigned int)=&quest::CQuestManager::GetPCForce;
    void (*cancel)(LPEVENT*)=quest::CancelTimerEvent;
    (void)set;(void)destroy;(void)clear;(void)loaded;(void)name;(void)get;(void)force;(void)cancel;
}
// Never assert old PC108/CQM216/QuestState60 against modern host STL.
