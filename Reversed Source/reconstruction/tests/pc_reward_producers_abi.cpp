#include "../../game/src/questpc.h"
#include <cstddef>
typedef char reward_dword4[sizeof(DWORD)==4?1:-1];
typedef char reward_int4[sizeof(int)==4?1:-1];
typedef char reward_record12[sizeof(quest::RewardData)==12?1:-1];
typedef char reward_type0[offsetof(quest::RewardData,type)==0?1:-1];
typedef char reward_first4[offsetof(quest::RewardData,value1)==4?1:-1];
typedef char reward_second8[offsetof(quest::RewardData,value2)==8?1:-1];
typedef char reward_none0[quest::RewardData::REWARD_TYPE_NONE==0?1:-1];
typedef char reward_exp1[quest::RewardData::REWARD_TYPE_EXP==1?1:-1];
typedef char reward_item2[quest::RewardData::REWARD_TYPE_ITEM==2?1:-1];
