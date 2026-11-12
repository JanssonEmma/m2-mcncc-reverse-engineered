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
#include "../../game/src/packet.h"
#include <cstddef>
void LifecycleAPI(){quest::QuestState(quest::CQuestManager::*open)(const std::string&,int)=&quest::CQuestManager::OpenState;void(quest::CQuestManager::*close)(quest::QuestState&)=&quest::CQuestManager::CloseState;void(quest::CQuestManager::*clear)()=&quest::CQuestManager::ClearScript;void(quest::CQuestManager::*append)(const std::string&)=&quest::CQuestManager::AddScript;void(quest::CQuestManager::*send)()=&quest::CQuestManager::SendScript;void(quest::CQuestManager::*done)(quest::QuestState&)=&quest::CQuestManager::GotoEndState;(void)open;(void)close;(void)clear;(void)append;(void)send;(void)done;}
typedef char script_size6[sizeof(packet_script)==6?1:-1];
typedef char script_header0[offsetof(packet_script,header)==0?1:-1];
typedef char script_size1[offsetof(packet_script,size)==1?1:-1];
typedef char script_skin3[offsetof(packet_script,skin)==3?1:-1];
typedef char script_source4[offsetof(packet_script,src_size)==4?1:-1];
typedef char script_byte1[sizeof(BYTE)==1?1:-1];
typedef char script_word2[sizeof(WORD)==2?1:-1];
