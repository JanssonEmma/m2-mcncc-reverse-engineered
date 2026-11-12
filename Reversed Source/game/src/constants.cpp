#include <stdio.h>
#include <cstring>
#include "constants.h"
#include "char.h"
TGuildWarInfo CHN_aGuildWarInfo[3] = {
    {0, 0, 0, 0, 0, 0},
    {103, 0, 100, 50, 0, 100},
    {105, 0, 100, 50, 0, 10},
};

TGuildWarInfo KOR_aGuildWarInfo[3] = {
    {0, 0, 0, 0, 0, 0},
    {110, 0, 100, 50, 0, 100},
    {111, 0, 100, 50, 0, 10},
};

TBattleTypeStat BattleTypeStats[8] = {
    {0, 0, 0, -10},
    {10, -20, -10, -15},
    {-5, -5, 10, 10},
    {0, 0, 0, 0},
    {10, -10, 0, -15},
    {-10, 10, -10, 0},
    {20, -20, 0, -10},
    {-20, 20, -10, 0},
};

const int aiAccessorySocketAddPct[ITEM_ACCESSORY_SOCKET_MAX_NUM] = {50, 50, 50};
const int aiAccessorySocketEffectivePct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] = {0, 10, 20, 40};
const int aiAccessorySocketDegradeTime[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] = {0, 86400, 43200, 21600};
const int aiAccessorySocketPutPct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] = {90, 80, 70, 0};

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

const int aiPolymorphPowerByLevel[41] = {
    10, 11, 11, 12, 13, 13, 14, 15, 16, 17, 18, 19, 20, 22, 23, 24, 26, 27, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 59, 62, 66, 70, 74, 79, 84, 89, 94, 100, 0
};

// Append to game/src/constants.cpp after including constants.h declarations.
// Mutable pointer initiallyNULL (.bss); selection is intentionally not implemented here.
const DWORD* exp_table=0;

// ELF 0x821a100 .rodata,400 bytes, SHA-256 b8af0bcd613daedc89613c22ac727729b39bc3fc9011a3eb462679fe0b3be1f5
Coord aArroundCoords[ARROUND_COORD_MAX_NUM] = {
    {    0,     0}, {    0,    50}, {   35,    35}, {   50,     0},
    {   35,   -35}, {    0,   -50}, {  -35,   -35}, {  -50,     0},
    {  -35,    35}, {    0,   100}, {   71,    71}, {  100,     0},
    {   71,   -71}, {    0,  -100}, {  -71,   -71}, { -100,     0},
    {  -71,    71}, {    0,   150}, {  106,   106}, {  150,     0},
    {  106,  -106}, {    0,  -150}, { -106,  -106}, { -150,     0},
    { -106,   106}, {    0,   200}, {  141,   141}, {  200,     0},
    {  141,  -141}, {    0,  -200}, { -141,  -141}, { -200,     0},
    { -141,   141}, {    0,   250}, {  177,   177}, {  250,     0},
    {  177,  -177}, {    0,  -250}, { -177,  -177}, { -250,     0},
    { -177,   177}, {    0,   300}, {  212,   212}, {  300,     0},
    {  212,  -212}, {    0,  -300}, { -212,  -212}, { -300,     0},
    { -212,   212}, {    0,   350}, {  247,   247}, {  350,     0},
    {  247,  -247}, {    0,  -350}, { -247,  -247}, { -350,     0},
    { -247,   247}, {    0,   400}, {  283,   283}, {  400,     0},
    {  283,  -283}, {    0,  -400}, { -283,  -283}, { -400,     0},
    { -283,   283}, {    0,   450}, {  318,   318}, {  450,     0},
    {  318,  -318}, {    0,  -450}, { -318,  -318}, { -450,     0},
    { -318,   318}, {    0,   500}, {  354,   354}, {  500,     0},
    {  354,  -354}, {    0,  -500}, { -354,  -354}, { -500,     0},
    { -354,   354}, {    0,   550}, {  389,   389}, {  550,     0},
    {  389,  -389}, {    0,  -550}, { -389,  -389}, { -550,     0},
    { -389,   389}, {    0,   600}, {  424,   424}, {  600,     0},
    {  424,  -424}, {    0,  -600}, { -424,  -424}, { -600,     0},
    { -424,   424}, {    0,   650}, {  460,   460}, {  650,     0},
    {  460,  -460}, {    0,  -650}, { -460,  -460}, { -650,     0},
    { -460,   460}, {    0,   700}, {  495,   495}, {  700,     0},
    {  495,  -495}, {    0,  -700}, { -495,  -495}, { -700,     0},
    { -495,   495}, {    0,   750}, {  530,   530}, {  750,     0},
    {  530,  -530}, {    0,  -750}, { -530,  -530}, { -750,     0},
    { -530,   530}, {    0,   800}, {  566,   566}, {  800,     0},
    {  566,  -566}, {    0,  -800}, { -566,  -566}, { -800,     0},
    { -566,   566}, {    0,   850}, {  601,   601}, {  850,     0},
    {  601,  -601}, {    0,  -850}, { -601,  -601}, { -850,     0},
    { -601,   601}, {    0,   900}, {  636,   636}, {  900,     0},
    {  636,  -636}, {    0,  -900}, { -636,  -636}, { -900,     0},
    { -636,   636}, {    0,   950}, {  672,   672}, {  950,     0},
    {  672,  -672}, {    0,  -950}, { -672,  -672}, { -950,     0},
    { -672,   672}, {    0,  1000}, {  707,   707}, { 1000,     0},
    {  707,  -707}, {    0, -1000}, { -707,  -707}, {-1000,     0},
    { -707,   707},
};
const char* c_apszEmpireNames[4] = {"\300\374\301\246\261\271", "\275\305\274\366\261\271", "\303\265\301\266\261\271", "\301\370\263\353\261\271"};
const char* c_apszPrivNames[5] = {"", "\276\306\300\314\305\333\300\314 \263\252\277\303 \310\256\267\374", "\265\267\300\314 \263\252\277\303 \310\256\267\374", "\265\267 \264\353\271\332\300\314 \263\252\277\303 \310\256\267\374", "\260\346\307\350\304\241 \271\350\300\262"};

const TMobRankStat MobRankStats[6] = {{20}, {20}, {25}, {30}, {50}, {100}};
const DWORD guild_exp_table[21] = {0, 15000, 45000, 90000, 160000, 235000, 325000, 430000, 550000, 685000, 835000, 1000000, 1500000, 2100000, 2800000, 3600000, 4500000, 6500000, 8000000, 10000000, 42000000};
const DWORD guild_exp_table2[21] = {0, 6000, 18000, 36000, 64000, 94000, 130000, 172000, 220000, 274000, 334000, 400000, 600000, 840000, 1120000, 1440000, 1800000, 2600000, 3200000, 4000000, 16800000};
const int aiSocketPercentByQty[5][4] = {{0, 0, 0, 0}, {3, 0, 0, 0}, {10, 1, 0, 0}, {15, 10, 1, 0}, {20, 15, 10, 1}};
const int aiWeaponSocketQty[8] = {3, 3, 3, 3, 3, 3, 0, 0};
const int aiArmorSocketQty[7] = {3, 1, 1, 0, 0, 0, 0};
const int aiItemAttributeAddPercent[7] = {100, 80, 60, 50, 30, 0, 0};
const int CHN_aiPartyBonusExpPercentByMemberCount[9] = {0, 0, 12, 18, 26, 40, 53, 70, 100};
const int KOR_aiPartyBonusExpPercentByMemberCount[9] = {0, 0, 30, 60, 75, 90, 105, 110, 140};
const int KOR_aiUniqueItemPartyBonusExpPercentByMemberCount[9] = {0, 0, 30, 42, 52, 60, 66, 70, 80};

const DWORD exp_table_euckr[100] = {
    0UL, 300UL, 800UL, 1500UL, 2500UL, 4300UL, 7200UL, 11000UL, 17000UL, 24000UL,
    33000UL, 43000UL, 58000UL, 76000UL, 100000UL, 130000UL, 169000UL, 219000UL, 283000UL, 365000UL,
    472000UL, 610000UL, 705000UL, 813000UL, 937000UL, 1077000UL, 1237000UL, 1418000UL, 1624000UL, 1857000UL,
    2122000UL, 2421000UL, 2761000UL, 3145000UL, 3580000UL, 4073000UL, 4632000UL, 5194000UL, 5717000UL, 6264000UL,
    6837000UL, 7600000UL, 8274000UL, 8990000UL, 9753000UL, 10560000UL, 11410000UL, 12320000UL, 13270000UL, 14280000UL,
    15340000UL, 16870000UL, 18960000UL, 19980000UL, 21420000UL, 22930000UL, 24530000UL, 26200000UL, 27960000UL, 29800000UL,
    32780000UL, 36060000UL, 39670000UL, 43640000UL, 48000000UL, 52800000UL, 58080000UL, 63890000UL, 70280000UL, 77310000UL,
    85040000UL, 93540000UL, 102900000UL, 113200000UL, 124500000UL, 137000000UL, 150700000UL, 165700000UL, 236990000UL, 260650000UL,
    286780000UL, 315380000UL, 346970000UL, 381680000UL, 419770000UL, 461760000UL, 508040000UL, 558740000UL, 614640000UL, 676130000UL,
    743730000UL, 1041222000UL, 1145344200UL, 1259878620UL, 1385866482UL, 1524453130UL, 1676898443UL, 1844588288UL, 2029047116UL, 2000000000UL,
};

// ELF 0x8219f60 .rodata,400 bytes, SHA-256 a4e5e0d6281999be6aac914cacec45dd217430ac6835a293fbac2fa812a60131
const DWORD exp_table_gb2312[100] = {
    0UL, 300UL, 800UL, 1500UL, 2500UL, 4300UL, 7200UL, 11400UL, 17000UL, 24000UL,
    33000UL, 46000UL, 62000UL, 83000UL, 111000UL, 149000UL, 200000UL, 268000UL, 360000UL, 482000UL,
    647000UL, 868000UL, 996000UL, 1143000UL, 1312000UL, 1506000UL, 1729000UL, 1984000UL, 2277000UL, 2614000UL,
    3000000UL, 3443000UL, 3952000UL, 4536000UL, 5206000UL, 5975000UL, 6858000UL, 7730000UL, 8504000UL, 9307000UL,
    10140000UL, 11330000UL, 12320000UL, 13370000UL, 14490000UL, 15670000UL, 16920000UL, 18240000UL, 19630000UL, 21090000UL,
    22630000UL, 24670000UL, 26890000UL, 29310000UL, 31950000UL, 34820000UL, 37960000UL, 41370000UL, 45100000UL, 49160000UL,
    53580000UL, 58400000UL, 63660000UL, 69390000UL, 75630000UL, 82440000UL, 89860000UL, 97950000UL, 106760000UL, 116370000UL,
    126800000UL, 138300000UL, 150700000UL, 164300000UL, 196900000UL, 234240000UL, 255240000UL, 278280000UL, 303240000UL, 330600000UL,
    390390000UL, 458220000UL, 535200000UL, 622240000UL, 720630000UL, 831600000UL, 1007200000UL, 1207580000UL, 1435920000UL, 1760940000UL,
    1990000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL,
};

// ELF 0x821a780 .rodata,400 bytes, SHA-256 bae30c5a5f88358312f123398e2647208976f75b364ea3d97c955265a7864c52
const DWORD exp_table_sjis[100] = {
    0UL, 300UL, 800UL, 1500UL, 2500UL, 4300UL, 7200UL, 11400UL, 17000UL, 24000UL,
    33000UL, 46000UL, 62000UL, 83000UL, 111000UL, 149000UL, 200000UL, 268000UL, 360000UL, 482000UL,
    647000UL, 868000UL, 996000UL, 1143000UL, 1312000UL, 1506000UL, 1729000UL, 1984000UL, 2277000UL, 2614000UL,
    3000000UL, 3443000UL, 3952000UL, 4536000UL, 5206000UL, 5975000UL, 6858000UL, 7730000UL, 8504000UL, 9307000UL,
    10140000UL, 11330000UL, 12320000UL, 13370000UL, 14490000UL, 15670000UL, 16920000UL, 18240000UL, 19630000UL, 21090000UL,
    22630000UL, 24670000UL, 26890000UL, 29310000UL, 31950000UL, 34820000UL, 37960000UL, 41370000UL, 45100000UL, 49160000UL,
    53580000UL, 58400000UL, 63660000UL, 69390000UL, 75630000UL, 82440000UL, 89860000UL, 97950000UL, 106760000UL, 116370000UL,
    126800000UL, 138300000UL, 150700000UL, 164300000UL, 179000000UL, 195200000UL, 212700000UL, 231900000UL, 252700000UL, 275500000UL,
    300300000UL, 327300000UL, 356800000UL, 388900000UL, 423900000UL, 462000000UL, 503600000UL, 548900000UL, 598300000UL, 652200000UL,
    710900000UL, 789100000UL, 883800000UL, 989900000UL, 1118600000UL, 1264000000UL, 1428300000UL, 1628300000UL, 1856300000UL, 2000000000UL,
};

// ELF 0x821a2a0 .rodata,400 bytes, SHA-256 ae3f5a6793f407684f481a653ea3f0c51f9ccf62c776383a5eb8fcda531aa76a
const DWORD exp_table_hk[100] = {
    22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL,
    22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL,
    22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL, 22UL,
    22UL, 39520UL, 45360UL, 52060UL, 59750UL, 68580UL, 77300UL, 85040UL, 93070UL, 101400UL,
    113300UL, 123200UL, 133700UL, 144900UL, 156700UL, 169200UL, 182400UL, 196300UL, 210900UL, 226300UL,
    246700UL, 537800UL, 586200UL, 639000UL, 696400UL, 759200UL, 827400UL, 902000UL, 983200UL, 1071600UL,
    1168000UL, 1273200UL, 1387800UL, 1512600UL, 1648800UL, 1797200UL, 1959000UL, 2135200UL, 2327400UL, 2536000UL,
    2766000UL, 15070000UL, 16430000UL, 19690000UL, 23424000UL, 25524000UL, 27828000UL, 30324000UL, 33060000UL, 39039000UL,
    45822000UL, 53520000UL, 62224000UL, 72063000UL, 83160000UL, 100720000UL, 120758000UL, 143592000UL, 176094000UL, 199000000UL,
    200000000UL, 200000000UL, 200000000UL, 200000000UL, 200000000UL, 200000000UL, 200000000UL, 200000000UL, 200000000UL, 200000000UL,
};

// ELF 0x821a440 .rodata,400 bytes, SHA-256 3290b6736ed320546fe55a4612d83436269944520c303b1627f0318fdc994c4e
const int aiSkillBookCountForLevelUp[10] = {3, 3, 3, 3, 3, 4, 4, 5, 5, 6};
const int aiGrandMasterSkillBookCountForLevelUp[10] = {3, 3, 5, 5, 7, 7, 10, 10, 10, 20};
const int aiGrandMasterSkillBookMinCount[10] = {1, 1, 1, 2, 2, 3, 3, 4, 5, 6};
const int aiGrandMasterSkillBookMaxCount[10] = {5, 7, 9, 11, 13, 15, 20, 25, 30, 35};

const DWORD exp_table_taiwan[100] = {
    300UL, 800UL, 1500UL, 2500UL, 4300UL, 7200UL, 11400UL, 17000UL, 24000UL, 33000UL,
    46000UL, 62000UL, 83000UL, 111000UL, 149000UL, 200000UL, 268000UL, 360000UL, 482000UL, 647000UL,
    868000UL, 996000UL, 1143000UL, 1312000UL, 1506000UL, 1729000UL, 1984000UL, 2277000UL, 2614000UL, 3000000UL,
    3443000UL, 3952000UL, 4536000UL, 5206000UL, 5975000UL, 6858000UL, 7730000UL, 8504000UL, 9307000UL, 10140000UL,
    11330000UL, 12320000UL, 13370000UL, 14490000UL, 15670000UL, 16920000UL, 18240000UL, 19630000UL, 21090000UL, 22630000UL,
    24670000UL, 26890000UL, 29310000UL, 31950000UL, 34820000UL, 37960000UL, 41370000UL, 45100000UL, 49160000UL, 53580000UL,
    58400000UL, 63660000UL, 69390000UL, 75630000UL, 82440000UL, 89860000UL, 97950000UL, 106760000UL, 136370000UL, 151800000UL,
    168300000UL, 250000000UL, 340000000UL, 450000000UL, 570000000UL, 690000000UL, 810000000UL, 912600000UL, 1004000000UL, 1094000000UL,
    1182000000UL, 1269000000UL, 1354000000UL, 1438000000UL, 1521000000UL, 1603000000UL, 1684000000UL, 1764000000UL, 1844000000UL, 1922000000UL,
    2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL,
};

const DWORD exp_table_newcibn[100] = {
    300UL, 800UL, 1500UL, 2500UL, 4300UL, 7200UL, 11400UL, 17000UL, 24000UL, 33000UL,
    46000UL, 62000UL, 83000UL, 111000UL, 149000UL, 200000UL, 268000UL, 360000UL, 482000UL, 647000UL,
    868000UL, 996000UL, 1143000UL, 1312000UL, 1506000UL, 1729000UL, 1984000UL, 2277000UL, 2614000UL, 3000000UL,
    3443000UL, 3952000UL, 4536000UL, 5206000UL, 5975000UL, 6858000UL, 7730000UL, 8504000UL, 9307000UL, 10140000UL,
    11330000UL, 12320000UL, 13370000UL, 14490000UL, 15670000UL, 16920000UL, 18240000UL, 19630000UL, 21090000UL, 22630000UL,
    24670000UL, 26890000UL, 29310000UL, 31950000UL, 34820000UL, 37960000UL, 41370000UL, 45100000UL, 49160000UL, 53580000UL,
    58400000UL, 63660000UL, 69390000UL, 75630000UL, 82440000UL, 89860000UL, 97950000UL, 106760000UL, 136370000UL, 151800000UL,
    168300000UL, 250000000UL, 340000000UL, 450000000UL, 570000000UL, 690000000UL, 810000000UL, 912600000UL, 1004000000UL, 1094000000UL,
    1182000000UL, 1269000000UL, 1354000000UL, 1438000000UL, 1521000000UL, 1603000000UL, 1684000000UL, 1764000000UL, 1844000000UL, 1922000000UL,
    2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL, 2000000000UL,
};


// Append to original game/src/constants.cpp, with <stdio.h> included.
// Complete old methods; mutable inputs and nonconst getters are intentional.
bool CTableBySkill::Check()
{
    for(int job=0;job<JOB_MAX_NUM*2;++job)
        if(!m_aiSkillPowerByLevelFromType[job]) {
            fprintf(stderr,"[NO SETTING SKILL] aiSkillPowerByLevelFromType[%d]",job);
            return false;
        }
    return true;
}
void CTableBySkill::DeleteAll()
{
    for(int job=0;job<JOB_MAX_NUM*2;++job)
        DeleteSkillPowerByLevelFromType(job);
    DeleteSkillDamageByLevelTable();
}
int CTableBySkill::GetSkillPowerByLevelFromType(int job,int skillgroup,int skilllevel,bool bMob)
{
    if(bMob) return m_aiSkillPowerByLevelFromType[0][skilllevel];
    if(job>3 || skillgroup==0) return 0;
    // Preserve original 32-bit LEA arithmetic even when signed inputs wrap.
    const unsigned int idx=static_cast<unsigned int>(job)*2U+static_cast<unsigned int>(skillgroup)-1U;
    return m_aiSkillPowerByLevelFromType[idx][skilllevel];
}
void CTableBySkill::SetSkillPowerByLevelFromType(int idx,int* aTable)
{
    DeleteSkillPowerByLevelFromType(idx);
    int* aiSkillTable=new int[SKILL_MAX_LEVEL+1];
    for(int i=0;i<SKILL_MAX_LEVEL+1;++i) aiSkillTable[i]=aTable[i];
    m_aiSkillPowerByLevelFromType[idx]=aiSkillTable;
    fprintf(stderr,"AddedSkill[%d][%d]\n",idx/2,idx%2);
}
void CTableBySkill::DeleteSkillPowerByLevelFromType(int idx)
{
    // Original 080e6960: JNZ skips NULL store. No delete[] or nonnull clear.
    if(m_aiSkillPowerByLevelFromType[idx]==0)
        m_aiSkillPowerByLevelFromType[idx]=0;
}
int CTableBySkill::GetSkillDamageByLevel(int Level)
{
    if(static_cast<unsigned int>(Level)<99U) return m_aiSkillDamageByLevel[Level];
    return 0;
}
void CTableBySkill::SetSkillDamageByLevelTable(int* aTable)
{
    DeleteSkillDamageByLevelTable();
    int* aiSkillDamageTable=new int[PLAYER_MAX_LEVEL];
    for(int i=0;i<PLAYER_MAX_LEVEL;++i) aiSkillDamageTable[i]=aTable[i];
    m_aiSkillDamageByLevel=aiSkillDamageTable;
}
void CTableBySkill::DeleteSkillDamageByLevelTable()
{
    if(m_aiSkillDamageByLevel!=0) {
        delete[] m_aiSkillDamageByLevel;
        m_aiSkillDamageByLevel=0;
    }
}

// Append to original constants.cpp after constants.h declarations.
// 0x821b2a0 SHA256 7512e6ba308fa4e93b6e6d303e1e27e534fed8490f6b3c56f664895c1bbda8e2
const int aiChainLightningCountBySkillLevel_euckr[41] = {
    0, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 4,
    4, 4, 4, 4, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5,
};
// 0x821b360 SHA256 bfe4f18732a28e4472bfd57811236a053b0f51c0b68ad9aa67d015149b4eb60f
const int aiChainLightningCountBySkillLevel_gb2312[41] = {
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
    5,
};
// 0x821a920 SHA256 11071daae08a969369db31b767ffdac6d2fde0ad513be160946aad5d002b2dbe
const int aiPercentByDeltaLevForBoss_euckr[31] = {
    1, 3, 5, 7, 15, 30, 60, 90, 91, 92,
    93, 94, 95, 97, 99, 100, 105, 110, 115, 120,
    125, 130, 135, 140, 145, 150, 155, 160, 165, 170,
    180,
};
// 0x821a9a0 SHA256 64253e685b0d03fa09330c8cf6559110d6772548fe722feef25690ec0c4dcfc5
const int aiPercentByDeltaLev_euckr[31] = {
    1, 5, 10, 20, 30, 50, 70, 80, 85, 90,
    92, 94, 96, 98, 100, 100, 105, 110, 115, 120,
    125, 130, 135, 140, 145, 150, 155, 160, 165, 170,
    180,
};
// 0x821aa20 SHA256 f614c1170dd5b3451444b70521c6ce7a082f00d12831835e85ee55ce5a9997ab
const int aiPercentByDeltaLev_gb2312[31] = {
    1, 3, 5, 10, 20, 30, 40, 50, 60, 65,
    70, 80, 85, 90, 95, 100, 105, 110, 115, 120,
    125, 130, 135, 140, 145, 150, 155, 160, 165, 170,
    180,
};
// 0x821aaa0 SHA256 64253e685b0d03fa09330c8cf6559110d6772548fe722feef25690ec0c4dcfc5
const int aiPercentByDeltaLev_sjis[31] = {
    1, 5, 10, 20, 30, 50, 70, 80, 85, 90,
    92, 94, 96, 98, 100, 100, 105, 110, 115, 120,
    125, 130, 135, 140, 145, 150, 155, 160, 165, 170,
    180,
};
const int* aiPercentByDeltaLev=0;
const int* aiPercentByDeltaLevForBoss=0;
const int* aiChainLightningCountBySkillLevel=0;

// Original constants.cpp full owning-by-value maps and exact data.
TItemAttrMap g_map_itemAttr;
TItemAttrMap g_map_itemRare;
const int aiItemMagicAttributePercentHigh[5] = {30,40,20,8,2};
const int aiItemMagicAttributePercentLow[5] = {50,40,10,0,0};

const SStoneDropInfo aStoneDrop[STONE_INFO_MAX_NUM] =
{
    {8005, 60, {30, 30, 30, 9, 1}},
    {8006, 60, {28, 29, 31, 11, 1}},
    {8007, 60, {24, 29, 32, 13, 2}},
    {8008, 60, {22, 28, 33, 15, 2}},
    {8009, 60, {21, 27, 33, 17, 2}},
    {8010, 60, {18, 26, 34, 20, 2}},
    {8011, 60, {14, 26, 35, 22, 3}},
    {8012, 60, {10, 26, 37, 24, 3}},
    {8013, 60, {2, 26, 40, 29, 3}},
    {8014, 60, {0, 26, 41, 30, 3}},
};

// 0821ad74 and 0821ada0, original mob attribute-to-apply tables.
const int aiMobEnchantApplyIdx[6] = {57, 14, 12, 13, 15, 16};
const int aiMobResistsApplyIdx[11] = {29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 41};

// Original 100-entry level weight table at 0821ab20.
const DWORD party_exp_distribute_table[100] = {
    0, 10, 10, 10, 10, 15, 15, 20, 25, 30,
    40, 50, 60, 80, 100, 120, 140, 160, 184, 210,
    240, 270, 300, 330, 360, 390, 420, 450, 480, 510,
    550, 600, 640, 700, 760, 820, 880, 940, 1000, 1100,
    1180, 1260, 1320, 1380, 1440, 1500, 1560, 1620, 1680, 1740,
    1800, 1860, 1920, 2000, 2100, 2200, 2300, 2450, 2600, 2750,
    2900, 3050, 3200, 3350, 3500, 3650, 3800, 3950, 4100, 4250,
    4400, 4600, 4800, 5000, 5200, 5400, 5600, 5800, 6000, 6200,
    6400, 6600, 6900, 7100, 7300, 7600, 7800, 8000, 8300, 8500,
    8800, 9000, 9000, 9000, 9000, 9000, 9000, 9000, 9000, 9000,
};

// 080e6a40
CMoneyTableBySkill::CMoneyTableBySkill()
{
    Release();
}

// 080e6b00
CMoneyTableBySkill::~CMoneyTableBySkill()
{
    Release();
}

// 080e6c50
void CMoneyTableBySkill::Initialize(int* moneyTable)
{
    std::memcpy(m_aiMoneyTable, moneyTable, sizeof(m_aiMoneyTable));
}

// 080e6ce0
int CMoneyTableBySkill::GetLimitMoney(int level)
{
    return m_aiMoneyTable[level - 1];
}

void CMoneyTableBySkill::Release()
{
    for (int& money : m_aiMoneyTable)
        money = 0;
}

const int aiExpLossPercents[100] = {
    0, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};
