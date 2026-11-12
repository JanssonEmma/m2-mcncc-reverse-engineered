#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#undef private
#include "../../game/src/packet.h"
#include <stddef.h>
typedef char int32[sizeof(int)==4?1:-1];
typedef char info6[sizeof(packet_quest_info)==6?1:-1];
typedef char header0[offsetof(packet_quest_info,header)==0?1:-1];
typedef char size1[offsetof(packet_quest_info,size)==1?1:-1];
typedef char index3[offsetof(packet_quest_info,index)==3?1:-1];
typedef char flag5[offsetof(packet_quest_info,flag)==5?1:-1];
void Gates(){
    void(quest::PC::*start)()=&quest::PC::SetCurrentQuestStartFlag;void(quest::PC::*done)()=&quest::PC::SetCurrentQuestDoneFlag;
    void(quest::PC::*clear)()=&quest::PC::ClearCurrentQuestBeginFlag;void(quest::PC::*begin)()=&quest::PC::SetCurrentQuestBeginFlag;int(quest::PC::*get)()=&quest::PC::GetCurrentQuestBeginFlag;
    void(quest::PC::*title)(const std::string&)=&quest::PC::SetCurrentQuestTitle;void(quest::PC::*other)(const std::string&,const std::string&)=&quest::PC::SetQuestTitle;
    void(quest::PC::*cn)(const std::string&)=&quest::PC::SetCurrentQuestClockName;void(quest::PC::*cv)(int)=&quest::PC::SetCurrentQuestClockValue;
    void(quest::PC::*nn)(const std::string&)=&quest::PC::SetCurrentQuestCounterName;void(quest::PC::*nv)(int)=&quest::PC::SetCurrentQuestCounterValue;
    void(quest::PC::*icon)(const std::string&)=&quest::PC::SetCurrentQuestIconFile;void(quest::PC::*send)()=&quest::PC::SendQuestInfoPakcet;
    CHARACTER*(quest::CQuestManager::*character)()=&quest::CQuestManager::GetCurrentCharacterPtr;
    (void)start;(void)done;(void)clear;(void)begin;(void)get;(void)title;(void)other;(void)cn;(void)cv;(void)nn;(void)nv;(void)icon;(void)send;(void)character;
}
