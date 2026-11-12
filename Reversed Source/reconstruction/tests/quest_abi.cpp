#include <cstddef>
#include "../../game/src/questmanager.h"
#define CHECK_SIZE(T,N) typedef char size_##T[(sizeof(quest::T)==N)?1:-1]
#define CHECK_OFFSET(T,M,N) typedef char offset_##T##_##M[(offsetof(quest::T,M)==N)?1:-1]
CHECK_SIZE(AStateScriptType,12);
CHECK_SIZE(AArgScript,36);
CHECK_OFFSET(AArgScript,arg,0);
CHECK_OFFSET(AArgScript,when_condition,4);
CHECK_OFFSET(AArgScript,script,16);
CHECK_OFFSET(AArgScript,quest_index,28);
CHECK_OFFSET(AArgScript,state_index,32);
CHECK_SIZE(QuestState,60);
CHECK_OFFSET(QuestState,co,0);
CHECK_OFFSET(QuestState,ico,4);
CHECK_OFFSET(QuestState,args,8);
CHECK_OFFSET(QuestState,suspend_state,10);
CHECK_OFFSET(QuestState,iIndex,12);
CHECK_OFFSET(QuestState,bStart,16);
CHECK_OFFSET(QuestState,st,20);
CHECK_OFFSET(QuestState,_title,24);
CHECK_OFFSET(QuestState,_clock_name,28);
CHECK_OFFSET(QuestState,_counter_name,32);
CHECK_OFFSET(QuestState,_clock_value,36);
CHECK_OFFSET(QuestState,_counter_value,40);
CHECK_OFFSET(QuestState,_icon_file,44);
CHECK_OFFSET(QuestState,chat_scripts,48);
CHECK_SIZE(RewardData,12);
CHECK_OFFSET(RewardData,type,0);
CHECK_OFFSET(RewardData,value1,4);
CHECK_OFFSET(RewardData,value2,8);
