#include "../../common/tables.h"
#include "../../game/src/questmanager.h"
#include <cstddef>
typedef char eventflag_long4[sizeof(long)==4?1:-1];
typedef char eventflag_packet37[sizeof(TPacketSetEventFlag)==37?1:-1];
typedef char eventflag_name0[offsetof(TPacketSetEventFlag,szFlagName)==0?1:-1];
typedef char eventflag_name33[sizeof(((TPacketSetEventFlag*)0)->szFlagName)==33?1:-1];
typedef char eventflag_value33[offsetof(TPacketSetEventFlag,lValue)==33?1:-1];
void EventFlagAPI(){void(quest::CQuestManager::*list)(CHARACTER*)=&quest::CQuestManager::SendEventFlagList;void(quest::CQuestManager::*request)(const std::string&,int)=&quest::CQuestManager::RequestSetEventFlag;void(quest::CQuestManager::*login)(CHARACTER*)=&quest::CQuestManager::BroadcastEventFlagOnLogin;(void)list;(void)request;(void)login;}
