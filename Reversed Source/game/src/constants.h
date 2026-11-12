// Partial old constant table declarations: target symbols/data with reference-guided int spelling.
#ifndef MCNCC_GAME_CONSTANTS_H
#define MCNCC_GAME_CONSTANTS_H
#include "../../common/singleton.h"
enum { ITEM_ACCESSORY_SOCKET_MAX_NUM = 3 };
extern const int aiAccessorySocketAddPct[ITEM_ACCESSORY_SOCKET_MAX_NUM];
extern const int aiAccessorySocketEffectivePct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
extern const int aiAccessorySocketDegradeTime[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
extern const int aiAccessorySocketPutPct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
// Insert into game/src/constants.h; retain existing accessory declarations.
#include "../../common/length.h"
#include "../../libthecore/include/typedef.h"
typedef struct SJobInitialPoints {
    int st;
    int ht;
    int dx;
    int iq;
    int max_hp;
    int max_sp;
    int hp_per_ht;
    int sp_per_iq;
    int hp_per_lv_begin;
    int hp_per_lv_end;
    int sp_per_lv_begin;
    int sp_per_lv_end;
    int max_stamina;
    int stamina_per_con;
    int stamina_per_lv_begin;
    int stamina_per_lv_end;
} TJobInitialPoints;
typedef struct SApplyInfo { BYTE bPointType; } TApplyInfo;
extern TJobInitialPoints JobInitialPoints[JOB_MAX_NUM];
extern const TApplyInfo aApplyInfo[MAX_APPLY_NUM];

// .rodata symbol0821b540,164 bytes; reference-guided const int spelling.
extern const int aiPolymorphPowerByLevel[41];

// Insert in game/src/constants.h before final guard. Exact old100-DWORD symbols.
extern const DWORD party_exp_distribute_table[100];
struct __coord { int x, y; };
typedef __coord Coord;
#define ARROUND_COORD_MAX_NUM 161
extern Coord aArroundCoords[ARROUND_COORD_MAX_NUM];
extern const char* c_apszEmpireNames[4];
extern const char* c_apszPrivNames[5];

struct SMobRankStat { int iGoldPercent; };
typedef SMobRankStat TMobRankStat;
extern const TMobRankStat MobRankStats[6];
extern const DWORD guild_exp_table[21];
extern const DWORD guild_exp_table2[21];
extern const int aiSocketPercentByQty[5][4];
extern const int aiWeaponSocketQty[8];
extern const int aiArmorSocketQty[7];
extern const int aiItemAttributeAddPercent[7];
extern const int CHN_aiPartyBonusExpPercentByMemberCount[9];
extern const int KOR_aiPartyBonusExpPercentByMemberCount[9];
extern const int KOR_aiUniqueItemPartyBonusExpPercentByMemberCount[9];

extern const DWORD exp_table_euckr[100];
extern const DWORD exp_table_gb2312[100];
extern const DWORD exp_table_sjis[100];
extern const int aiSkillBookCountForLevelUp[10];
extern const int aiGrandMasterSkillBookCountForLevelUp[10];
extern const int aiGrandMasterSkillBookMinCount[10];
extern const int aiGrandMasterSkillBookMaxCount[10];
extern const DWORD exp_table_taiwan[100];

extern const DWORD exp_table_hk[100];
extern const DWORD exp_table_newcibn[100];
#include "start_position.h"
extern const int aiExpLossPercents[100];
extern const DWORD* exp_table; // pointer mutable, pointee const; original .bss initial0

// Insert into original game/src/constants.h before its final guard.
// Include ../../common/singleton.h outside this class.
// Old constants.h:193; complete logical class, target32 size40.
class CTableBySkill : public singleton<CTableBySkill>
{
public:
    CTableBySkill()
    {
        for (int job=0; job<JOB_MAX_NUM*2; ++job)
            m_aiSkillPowerByLevelFromType[job]=0;
        m_aiSkillDamageByLevel=0;
    }
    virtual ~CTableBySkill() { DeleteAll(); }
    bool Check();
    void DeleteAll();
    int GetSkillPowerByLevelFromType(int job,int skillgroup,int skilllevel,bool bMob);
    void SetSkillPowerByLevelFromType(int idx,int* aTable);
    void DeleteSkillPowerByLevelFromType(int idx);
    int GetSkillDamageByLevel(int Level);
    void SetSkillDamageByLevelTable(int* aTable);
    void DeleteSkillDamageByLevelTable();
private:
    int* m_aiSkillPowerByLevelFromType[JOB_MAX_NUM*2];
    int* m_aiSkillDamageByLevel;
};

// Append to original constants.h. Original symbol sizes31/41 signed32 ints.
extern const int aiChainLightningCountBySkillLevel_euckr[41];
extern const int aiChainLightningCountBySkillLevel_gb2312[41];
extern const int aiPercentByDeltaLevForBoss_euckr[31];
extern const int aiPercentByDeltaLev_euckr[31];
extern const int aiPercentByDeltaLev_gb2312[31];
extern const int aiPercentByDeltaLev_sjis[31];
extern const int* aiPercentByDeltaLev;
extern const int* aiPercentByDeltaLevForBoss;
extern const int* aiChainLightningCountBySkillLevel;

#include <map>
#include "../../common/tables.h"
typedef std::map<DWORD,TItemAttrTable> TItemAttrMap;
extern TItemAttrMap g_map_itemAttr;
extern TItemAttrMap g_map_itemRare;
extern const int aiItemMagicAttributePercentHigh[5];
extern const int aiItemMagicAttributePercentLow[5];

// Original aStoneDrop table in .rodata at 0821b420.
enum { STONE_INFO_MAX_NUM = 10, STONE_LEVEL_MAX_NUM = 4 };
struct SStoneDropInfo
{
    DWORD dwMobVnum;
    int iDropPct;
    int iLevelPct[STONE_LEVEL_MAX_NUM + 1];
};

inline bool operator<(const SStoneDropInfo& info, DWORD vnum)
{
    return info.dwMobVnum < vnum;
}

extern const SStoneDropInfo aStoneDrop[STONE_INFO_MAX_NUM];

extern const int aiMobEnchantApplyIdx[6];
extern const int aiMobResistsApplyIdx[11];

class CAdditionalDamageByLevel : public singleton<CAdditionalDamageByLevel> {
public:
    CAdditionalDamageByLevel() : m_nDamage() {}
private:
    int m_nDamage[99];
};

struct TGuildWarInfo
{
    long lMapIndex;
    int iWarPrice;
    int iWinnerPotionRewardPctToWinner;
    int iLoserPotionRewardPctToWinner;
    int iInitialScore;
    int iEndScore;
};

struct SBattleTypeStat
{
    int AttGradeBias;
    int DefGradeBias;
    int MagicAttGradeBias;
    int MagicDefGradeBias;
};
typedef SBattleTypeStat TBattleTypeStat;
extern TBattleTypeStat BattleTypeStats[8];
extern TGuildWarInfo CHN_aGuildWarInfo[3];
extern TGuildWarInfo KOR_aGuildWarInfo[3];

class CMoneyTableBySkill : public singleton<CMoneyTableBySkill> {
public:
    CMoneyTableBySkill();
    virtual ~CMoneyTableBySkill();
    void Initialize(int* moneyTable);
    void Release();
    int GetLimitMoney(int level);
private:
    int m_aiMoneyTable[99];
};

#endif
