#include "../../game/src/char.h"
#define CHECK(name, cond) typedef char name[(cond)?1:-1]
CHECK(point_size, sizeof(CHARACTER_POINT)==1056);
CHECK(instant_size, sizeof(CHARACTER_POINT_INSTANT)==1688);
CHECK(vid_size, sizeof(VID)==8);
CHECK(affect_size, sizeof(TAffectFlag)==8);
CHECK(trigger_size, sizeof(CTrigger)==8);
CHECK(skill_use_size, sizeof(TSkillUseInfo)==32);
CHECK(quick_size, sizeof(TQuickslot)==2);
CHECK(battle_size, sizeof(CHARACTER::TBattleInfo)==8);
CHECK(attack_log_size, sizeof(CHARACTER::SAttackLog)==8);
CHECK(attacked_log_size, sizeof(CHARACTER::SAttackedLog)==8);
CHECK(horse_size, sizeof(CHorseRider)==24);
