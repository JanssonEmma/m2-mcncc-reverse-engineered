#include <map>
#include <set>
#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#undef private
void YieldAPI(){void(quest::CQuestManager::*pause)(quest::QuestState&)=&quest::CQuestManager::GotoPauseState;void(quest::CQuestManager::*input)(quest::QuestState&)=&quest::CQuestManager::GotoInputState;void(quest::CQuestManager::*item)(quest::QuestState&)=&quest::CQuestManager::GotoSelectItemState;void(quest::CQuestManager::*select)(quest::QuestState&)=&quest::CQuestManager::GotoSelectState;void(quest::CQuestManager::*error)(const char*,int,const char*,...)=&quest::CQuestManager::QuestError;(void)pause;(void)input;(void)item;(void)select;(void)error;}
typedef char yield_short2[sizeof(short)==2?1:-1];
typedef char yield_int4[sizeof(int)==4?1:-1];
typedef char yield_suspend1[sizeof(((quest::QuestState*)0)->suspend_state)==1?1:-1];
