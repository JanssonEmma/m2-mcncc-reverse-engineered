#include "../../game/src/questpc.h"
void PCConsumersAPI(){void(quest::PC::*current)(const std::string&)=&quest::PC::SetCurrentQuestStateName;void(quest::PC::*named)(const std::string&,const std::string&)=&quest::PC::SetQuestState;void(quest::PC::*list)(CHARACTER*)=&quest::PC::SendFlagList;(void)current;(void)named;(void)list;}
typedef char consumer_int32[sizeof(int)==4?1:-1];
typedef char consumer_DWORD32[sizeof(DWORD)==4?1:-1];
