#include <cstddef>
#include "../../game/src/char.h"
#include "../../game/src/constants.h"
long (CHARACTER::*helper_point_signature)(BYTE)=&CHARACTER::GetPolymorphPoint;
int (CHARACTER::*helper_power_signature)()=&CHARACTER::GetPolymorphPower;
int (CHARACTER::*helper_skill_signature)(DWORD)=&CHARACTER::GetSkillLevel;
const TMobTable& (CHARACTER::*helper_mob_reference_signature)()=&CHARACTER::GetMobTable;
typedef char helper_skill_size[(sizeof(TPlayerSkill)==6)?1:-1];
typedef char helper_skill_level_offset[(offsetof(TPlayerSkill,bLevel)==1)?1:-1];
typedef char helper_skill_time_offset[(offsetof(TPlayerSkill,tNextRead)==2)?1:-1];
typedef char helper_power_count[(sizeof(aiPolymorphPowerByLevel)/sizeof(int)==41)?1:-1];
const int* helper_const_power_table=aiPolymorphPowerByLevel;
