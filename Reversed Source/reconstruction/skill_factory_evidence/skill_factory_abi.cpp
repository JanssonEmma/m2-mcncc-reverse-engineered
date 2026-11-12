#include <cstddef>
#include "../../common/tables.h"
#include "../../game/src/skill.h"
typedef char skill_table_size[sizeof(TSkillTable)==1475?1:-1];
typedef char field_dwVnum[offsetof(TSkillTable,dwVnum)==0?1:-1];
typedef char field_szName[offsetof(TSkillTable,szName)==4?1:-1];
typedef char field_bType[offsetof(TSkillTable,bType)==37?1:-1];
typedef char field_bMaxLevel[offsetof(TSkillTable,bMaxLevel)==38?1:-1];
typedef char field_dwSplashRange[offsetof(TSkillTable,dwSplashRange)==39?1:-1];
typedef char field_szPointOn[offsetof(TSkillTable,szPointOn)==43?1:-1];
typedef char field_szPointPoly[offsetof(TSkillTable,szPointPoly)==107?1:-1];
typedef char field_szSPCostPoly[offsetof(TSkillTable,szSPCostPoly)==208?1:-1];
typedef char field_szDurationPoly[offsetof(TSkillTable,szDurationPoly)==309?1:-1];
typedef char field_szDurationSPCostPoly[offsetof(TSkillTable,szDurationSPCostPoly)==410?1:-1];
typedef char field_szCooldownPoly[offsetof(TSkillTable,szCooldownPoly)==511?1:-1];
typedef char field_szMasterBonusPoly[offsetof(TSkillTable,szMasterBonusPoly)==612?1:-1];
typedef char field_szGrandMasterAddSPCostPoly[offsetof(TSkillTable,szGrandMasterAddSPCostPoly)==713?1:-1];
typedef char field_dwFlag[offsetof(TSkillTable,dwFlag)==814?1:-1];
typedef char field_dwAffectFlag[offsetof(TSkillTable,dwAffectFlag)==818?1:-1];
typedef char field_szPointOn2[offsetof(TSkillTable,szPointOn2)==822?1:-1];
typedef char field_szPointPoly2[offsetof(TSkillTable,szPointPoly2)==886?1:-1];
typedef char field_szDurationPoly2[offsetof(TSkillTable,szDurationPoly2)==987?1:-1];
typedef char field_dwAffectFlag2[offsetof(TSkillTable,dwAffectFlag2)==1088?1:-1];
typedef char field_szPointOn3[offsetof(TSkillTable,szPointOn3)==1092?1:-1];
typedef char field_szPointPoly3[offsetof(TSkillTable,szPointPoly3)==1156?1:-1];
typedef char field_szDurationPoly3[offsetof(TSkillTable,szDurationPoly3)==1257?1:-1];
typedef char field_bLevelStep[offsetof(TSkillTable,bLevelStep)==1358?1:-1];
typedef char field_bLevelLimit[offsetof(TSkillTable,bLevelLimit)==1359?1:-1];
typedef char field_preSkillVnum[offsetof(TSkillTable,preSkillVnum)==1360?1:-1];
typedef char field_preSkillLevel[offsetof(TSkillTable,preSkillLevel)==1364?1:-1];
typedef char field_lMaxHit[offsetof(TSkillTable,lMaxHit)==1365?1:-1];
typedef char field_szSplashAroundDamageAdjustPoly[offsetof(TSkillTable,szSplashAroundDamageAdjustPoly)==1369?1:-1];
typedef char field_bSkillAttrType[offsetof(TSkillTable,bSkillAttrType)==1470?1:-1];
typedef char field_dwTargetRange[offsetof(TSkillTable,dwTargetRange)==1471?1:-1];
static bool (CSkillManager::*initialize)(TSkillTable*,int)=&CSkillManager::Initialize;
static CSkillProto* (CSkillManager::*by_name)(const char*)=&CSkillManager::Get;
static void (CSkillProto::*pointvar)(const std::string&,double)=&CSkillProto::SetPointVar;
static void (CSkillProto::*durationvar)(const std::string&,double)=&CSkillProto::SetDurationVar;
static void (CSkillProto::*spvar)(const std::string&,double)=&CSkillProto::SetSPCostVar;

static bool (CSkillProto::*charge_signature)()=&CSkillProto::IsChargeSkill;
static DWORD CSkillProto::*charge_vnum_type=&CSkillProto::dwVnum;
typedef char skill_flags_width[sizeof(ESkillFlags)==4?1:-1];
typedef char skill_indexes_width[sizeof(ESkillIndexes)==4?1:-1];
typedef char old_charge5[SKILL_TANHWAN==5?1:-1];
typedef char old_horsecharge138[SKILL_HORSE_CHARGE==138?1:-1];
