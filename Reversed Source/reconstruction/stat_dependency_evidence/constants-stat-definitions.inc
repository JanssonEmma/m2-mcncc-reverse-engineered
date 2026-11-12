// Insert into game/src/constants.cpp; retain accessory definitions.
// constants.h and char.h (EPointTypes) must be included by the translation unit.
TJobInitialPoints JobInitialPoints[JOB_MAX_NUM] = {
    {6, 4, 3, 3, 600, 200, 40, 20, 36, 44, 18, 22, 800, 5, 1, 3},
    {4, 3, 6, 3, 650, 200, 40, 20, 36, 44, 18, 22, 800, 5, 1, 3},
    {5, 3, 3, 5, 650, 200, 40, 20, 36, 44, 18, 22, 800, 5, 1, 3},
    {3, 4, 3, 6, 700, 200, 40, 20, 36, 44, 18, 22, 800, 5, 1, 3},
};
const TApplyInfo aApplyInfo[MAX_APPLY_NUM] = {
    {POINT_NONE}, // apply 0
    {POINT_MAX_HP}, // apply 1
    {POINT_MAX_SP}, // apply 2
    {POINT_HT}, // apply 3
    {POINT_IQ}, // apply 4
    {POINT_ST}, // apply 5
    {POINT_DX}, // apply 6
    {POINT_ATT_SPEED}, // apply 7
    {POINT_MOV_SPEED}, // apply 8
    {POINT_CASTING_SPEED}, // apply 9
    {POINT_HP_REGEN}, // apply 10
    {POINT_SP_REGEN}, // apply 11
    {POINT_POISON_PCT}, // apply 12
    {POINT_STUN_PCT}, // apply 13
    {POINT_SLOW_PCT}, // apply 14
    {POINT_CRITICAL_PCT}, // apply 15
    {POINT_PENETRATE_PCT}, // apply 16
    {POINT_ATTBONUS_HUMAN}, // apply 17
    {POINT_ATTBONUS_ANIMAL}, // apply 18
    {POINT_ATTBONUS_ORC}, // apply 19
    {POINT_ATTBONUS_MILGYO}, // apply 20
    {POINT_ATTBONUS_UNDEAD}, // apply 21
    {POINT_ATTBONUS_DEVIL}, // apply 22
    {POINT_STEAL_HP}, // apply 23
    {POINT_STEAL_SP}, // apply 24
    {POINT_MANA_BURN_PCT}, // apply 25
    {POINT_DAMAGE_SP_RECOVER}, // apply 26
    {POINT_BLOCK}, // apply 27
    {POINT_DODGE}, // apply 28
    {POINT_RESIST_SWORD}, // apply 29
    {POINT_RESIST_TWOHAND}, // apply 30
    {POINT_RESIST_DAGGER}, // apply 31
    {POINT_RESIST_BELL}, // apply 32
    {POINT_RESIST_FAN}, // apply 33
    {POINT_RESIST_BOW}, // apply 34
    {POINT_RESIST_FIRE}, // apply 35
    {POINT_RESIST_ELEC}, // apply 36
    {POINT_RESIST_MAGIC}, // apply 37
    {POINT_RESIST_WIND}, // apply 38
    {POINT_REFLECT_MELEE}, // apply 39
    {POINT_REFLECT_CURSE}, // apply 40
    {POINT_POISON_REDUCE}, // apply 41
    {POINT_KILL_SP_RECOVER}, // apply 42
    {POINT_EXP_DOUBLE_BONUS}, // apply 43
    {POINT_GOLD_DOUBLE_BONUS}, // apply 44
    {POINT_ITEM_DROP_BONUS}, // apply 45
    {POINT_POTION_BONUS}, // apply 46
    {POINT_KILL_HP_RECOVERY}, // apply 47
    {POINT_IMMUNE_STUN}, // apply 48
    {POINT_IMMUNE_SLOW}, // apply 49
    {POINT_IMMUNE_FALL}, // apply 50
    {POINT_NONE}, // apply 51
    {POINT_BOW_DISTANCE}, // apply 52
    {POINT_ATT_GRADE_BONUS}, // apply 53
    {POINT_DEF_GRADE_BONUS}, // apply 54
    {POINT_MAGIC_ATT_GRADE_BONUS}, // apply 55
    {POINT_MAGIC_DEF_GRADE_BONUS}, // apply 56
    {POINT_CURSE_PCT}, // apply 57
    {POINT_MAX_STAMINA}, // apply 58
    {POINT_ATTBONUS_WARRIOR}, // apply 59
    {POINT_ATTBONUS_ASSASSIN}, // apply 60
    {POINT_ATTBONUS_SURA}, // apply 61
    {POINT_ATTBONUS_SHAMAN}, // apply 62
    {POINT_ATTBONUS_MONSTER}, // apply 63
    {POINT_MALL_ATTBONUS}, // apply 64
    {POINT_MALL_DEFBONUS}, // apply 65
    {POINT_MALL_EXPBONUS}, // apply 66
    {POINT_MALL_ITEMBONUS}, // apply 67
    {POINT_MALL_GOLDBONUS}, // apply 68
    {POINT_MAX_HP_PCT}, // apply 69
    {POINT_MAX_SP_PCT}, // apply 70
    {POINT_SKILL_DAMAGE_BONUS}, // apply 71
    {POINT_NORMAL_HIT_DAMAGE_BONUS}, // apply 72
    {POINT_SKILL_DEFEND_BONUS}, // apply 73
    {POINT_NORMAL_HIT_DEFEND_BONUS}, // apply 74
    {POINT_PC_BANG_EXP_BONUS}, // apply 75
    {POINT_PC_BANG_DROP_BONUS}, // apply 76
    {POINT_NONE}, // apply 77
    {POINT_RESIST_WARRIOR}, // apply 78
    {POINT_RESIST_ASSASSIN}, // apply 79
    {POINT_RESIST_SURA}, // apply 80
    {POINT_RESIST_SHAMAN}, // apply 81
};
