#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#undef private
#include <stddef.h>
typedef char DWORD32[sizeof(DWORD)==4?1:-1];
typedef char info12[sizeof(quest::PC::TQuestStateChangeInfo)==12?1:-1];
typedef char info_index0[offsetof(quest::PC::TQuestStateChangeInfo,quest_idx)==0?1:-1];
typedef char info_prev4[offsetof(quest::PC::TQuestStateChangeInfo,prev_state)==4?1:-1];
typedef char info_next8[offsetof(quest::PC::TQuestStateChangeInfo,next_state)==8?1:-1];
void Gates(){
    typedef quest::PC::QuestInfoIterator I;
    bool(quest::PC::*has)(const std::string&)=&quest::PC::HasQuest;
    quest::QuestState&(quest::PC::*get)(const std::string&)=&quest::PC::GetQuest;
    I(quest::PC::*begin)()=&quest::PC::quest_begin;I(quest::PC::*end)()=&quest::PC::quest_end;
    I(quest::PC::*find)(DWORD)=&quest::PC::quest_find;
    int(quest::PC::*current)()=&quest::PC::GetCurrentQuestIndex;
    void(quest::PC::*set)(const std::string&,quest::QuestState&)=&quest::PC::SetQuest;
    void(quest::PC::*setState)(const std::string&,int)=&quest::PC::SetQuestState;
    void(quest::PC::*change)(const std::string&,int,int)=&quest::PC::AddQuestStateChange;
    void(quest::PC::*clear)(const std::string&)=&quest::PC::ClearQuest;
    unsigned int(quest::CQuestManager::*index)(const std::string&)=&quest::CQuestManager::GetQuestIndexByName;
    (void)has;(void)get;(void)begin;(void)end;(void)find;(void)current;(void)set;(void)setState;(void)change;(void)clear;(void)index;
}
