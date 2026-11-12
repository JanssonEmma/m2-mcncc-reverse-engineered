// Complete old CHARACTER data and virtual interface; nonvirtual API recovery is partial.
#ifndef MCNCC_GAME_CHAR_H
#define MCNCC_GAME_CHAR_H
#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>
#include "../../libthecore/include/typedef.h"
#include "../../common/tables.h"
#include "entity.h"
#include "FSM.h"
#include "horse_rider.h"
#include "event.h"
#include "vid.h"
#include "affect_flag.h"
// Insert in game/src/char.h, original EPointTypes location.
// Insert in game/src/char.h; complete old EPositions enum.
enum EPKModes {
    PK_MODE_PEACE = 0,
    PK_MODE_REVENGE = 1,
    PK_MODE_FREE = 2,
    PK_MODE_PROTECT = 3,
    PK_MODE_GUILD = 4,
    PK_MODE_MAX_NUM = 5
};

enum EPositions {
    POS_DEAD = 0,
    POS_SLEEPING = 1,
    POS_RESTING = 2,
    POS_SITTING = 3,
    POS_FISHING = 4,
    POS_FIGHTING = 5,
    POS_MOUNTING = 6,
    POS_STANDING = 7
};

enum EPointTypes {
    POINT_NONE = 0,
    POINT_LEVEL = 1,
    POINT_VOICE = 2,
    POINT_EXP = 3,
    POINT_NEXT_EXP = 4,
    POINT_HP = 5,
    POINT_MAX_HP = 6,
    POINT_SP = 7,
    POINT_MAX_SP = 8,
    POINT_STAMINA = 9,
    POINT_MAX_STAMINA = 10,
    POINT_GOLD = 11,
    POINT_ST = 12,
    POINT_HT = 13,
    POINT_DX = 14,
    POINT_IQ = 15,
    POINT_DEF_GRADE = 16,
    POINT_ATT_SPEED = 17,
    POINT_ATT_GRADE = 18,
    POINT_MOV_SPEED = 19,
    POINT_CLIENT_DEF_GRADE = 20,
    POINT_CASTING_SPEED = 21,
    POINT_MAGIC_ATT_GRADE = 22,
    POINT_MAGIC_DEF_GRADE = 23,
    POINT_EMPIRE_POINT = 24,
    POINT_LEVEL_STEP = 25,
    POINT_STAT = 26,
    POINT_SUB_SKILL = 27,
    POINT_SKILL = 28,
    POINT_WEAPON_MIN = 29,
    POINT_WEAPON_MAX = 30,
    POINT_PLAYTIME = 31,
    POINT_HP_REGEN = 32,
    POINT_SP_REGEN = 33,
    POINT_BOW_DISTANCE = 34,
    POINT_HP_RECOVERY = 35,
    POINT_SP_RECOVERY = 36,
    POINT_POISON_PCT = 37,
    POINT_STUN_PCT = 38,
    POINT_SLOW_PCT = 39,
    POINT_CRITICAL_PCT = 40,
    POINT_PENETRATE_PCT = 41,
    POINT_CURSE_PCT = 42,
    POINT_ATTBONUS_HUMAN = 43,
    POINT_ATTBONUS_ANIMAL = 44,
    POINT_ATTBONUS_ORC = 45,
    POINT_ATTBONUS_MILGYO = 46,
    POINT_ATTBONUS_UNDEAD = 47,
    POINT_ATTBONUS_DEVIL = 48,
    POINT_ATTBONUS_INSECT = 49,
    POINT_ATTBONUS_FIRE = 50,
    POINT_ATTBONUS_ICE = 51,
    POINT_ATTBONUS_DESERT = 52,
    POINT_ATTBONUS_MONSTER = 53,
    POINT_ATTBONUS_WARRIOR = 54,
    POINT_ATTBONUS_ASSASSIN = 55,
    POINT_ATTBONUS_SURA = 56,
    POINT_ATTBONUS_SHAMAN = 57,
    POINT_ATTBONUS_TREE = 58,
    POINT_RESIST_WARRIOR = 59,
    POINT_RESIST_ASSASSIN = 60,
    POINT_RESIST_SURA = 61,
    POINT_RESIST_SHAMAN = 62,
    POINT_STEAL_HP = 63,
    POINT_STEAL_SP = 64,
    POINT_MANA_BURN_PCT = 65,
    POINT_DAMAGE_SP_RECOVER = 66,
    POINT_BLOCK = 67,
    POINT_DODGE = 68,
    POINT_RESIST_SWORD = 69,
    POINT_RESIST_TWOHAND = 70,
    POINT_RESIST_DAGGER = 71,
    POINT_RESIST_BELL = 72,
    POINT_RESIST_FAN = 73,
    POINT_RESIST_BOW = 74,
    POINT_RESIST_FIRE = 75,
    POINT_RESIST_ELEC = 76,
    POINT_RESIST_MAGIC = 77,
    POINT_RESIST_WIND = 78,
    POINT_REFLECT_MELEE = 79,
    POINT_REFLECT_CURSE = 80,
    POINT_POISON_REDUCE = 81,
    POINT_KILL_SP_RECOVER = 82,
    POINT_EXP_DOUBLE_BONUS = 83,
    POINT_GOLD_DOUBLE_BONUS = 84,
    POINT_ITEM_DROP_BONUS = 85,
    POINT_POTION_BONUS = 86,
    POINT_KILL_HP_RECOVERY = 87,
    POINT_IMMUNE_STUN = 88,
    POINT_IMMUNE_SLOW = 89,
    POINT_IMMUNE_FALL = 90,
    POINT_PARTY_ATTACKER_BONUS = 91,
    POINT_PARTY_TANKER_BONUS = 92,
    POINT_ATT_BONUS = 93,
    POINT_DEF_BONUS = 94,
    POINT_ATT_GRADE_BONUS = 95,
    POINT_DEF_GRADE_BONUS = 96,
    POINT_MAGIC_ATT_GRADE_BONUS = 97,
    POINT_MAGIC_DEF_GRADE_BONUS = 98,
    POINT_RESIST_NORMAL_DAMAGE = 99,
    POINT_HIT_HP_RECOVERY = 100,
    POINT_HIT_SP_RECOVERY = 101,
    POINT_MANASHIELD = 102,
    POINT_PARTY_BUFFER_BONUS = 103,
    POINT_PARTY_SKILL_MASTER_BONUS = 104,
    POINT_HP_RECOVER_CONTINUE = 105,
    POINT_SP_RECOVER_CONTINUE = 106,
    POINT_STEAL_GOLD = 107,
    POINT_POLYMORPH = 108,
    POINT_MOUNT = 109,
    POINT_PARTY_HASTE_BONUS = 110,
    POINT_PARTY_DEFENDER_BONUS = 111,
    POINT_STAT_RESET_COUNT = 112,
    POINT_HORSE_SKILL = 113,
    POINT_MALL_ATTBONUS = 114,
    POINT_MALL_DEFBONUS = 115,
    POINT_MALL_EXPBONUS = 116,
    POINT_MALL_ITEMBONUS = 117,
    POINT_MALL_GOLDBONUS = 118,
    POINT_MAX_HP_PCT = 119,
    POINT_MAX_SP_PCT = 120,
    POINT_SKILL_DAMAGE_BONUS = 121,
    POINT_NORMAL_HIT_DAMAGE_BONUS = 122,
    POINT_SKILL_DEFEND_BONUS = 123,
    POINT_NORMAL_HIT_DEFEND_BONUS = 124,
    POINT_PC_BANG_EXP_BONUS = 125,
    POINT_PC_BANG_DROP_BONUS = 126
};

class CHARACTER;
struct character_point;
struct character_point_instant;
struct TSkillUseInfo;
class CAffect;
class CArena;
class CDungeon;
class CExchange;
class CGuild;
class CItem;
class CMob;
class CMobInstance;
class CParty;
struct packet_party_update;
typedef packet_party_update TPacketGCPartyUpdate;
class CSafebox;
class CShop;
class CWarMap;
class WeddingMap;
class regen;
typedef CItem *LPITEM;
typedef CHARACTER *LPCHARACTER;
typedef character_point CHARACTER_POINT;
typedef character_point_instant CHARACTER_POINT_INSTANT;
typedef std::list<CHARACTER* > CHARACTER_LIST;
typedef CShop *LPSHOP;
typedef regen *LPREGEN;
enum MONARCH_INDEX { MI_HEAL=0, MI_WARP=1, MI_TRANSFER=2, MI_TAX=3, MI_SUMMON=4, MI_MAX=5 };
enum e_overtime { OT_NONE = 0, OT_3HOUR = 1, OT_5HOUR = 2 };
struct character_point {
    long points[255];
    BYTE job;
    BYTE voice;
    BYTE level;
    DWORD exp;
    long gold;
    int hp;
    int sp;
    int iRandomHP;
    int iRandomSP;
    int stamina;
    BYTE skill_group;
};
struct character_point_instant {
    long points[255];
    float fRot;
    int iMaxHP;
    int iMaxSP;
    long position;
    long instant_flag;
    DWORD dwAIFlag;
    DWORD dwImmuneFlag;
    DWORD dwLastShoutPulse;
    WORD parts[4];
    LPITEM pItems[109];
    BYTE bItemGrid[109];
    LPITEM pCubeItems[15];
    LPCHARACTER pCubeNpc;
    LPCHARACTER battle_victim;
    BYTE gm_level;
    BYTE bBasePart;
    int iMaxStamina;
    BYTE bBlockMode;
};
struct CTrigger {
    BYTE bType;
    int (*pFunc)(CHARACTER *, CHARACTER *);
};
struct TSkillUseInfo {
    int iHitCount;
    int iMaxHitCount;
    int iSplashCount;
    DWORD dwNextSkillUsableTime;
    int iRange;
    bool bUsed;
    DWORD dwVID;
    bool isGrandMaster;
};
class CHARACTER : public CEntity, public CFSM, public CHorseRider {
public:
    bool BuildUpdatePartyPacket(TPacketGCPartyUpdate& out);
long GetPolymorphPoint(BYTE type);
int GetPolymorphPower();
int GetSkillLevel(DWORD dwVnum);
const TMobTable& GetMobTable();

// Insert in game/src/char.h; original non-const methods.
void BroadcastTargetPacket();
void SetNowWalking(bool bWalkFlag);

void SetLevel(BYTE level);
BYTE GetGMLevel(); // Old API is NON-CONST, unlike newer reference.
void SetPKMode(BYTE bPKMode);

// Insert in game/src/char.h. Old methods are NON-CONST; verify linkage in metadata.
BYTE GetJob();
bool IsDead();
bool IsStun();

protected:
    void Initialize();
public:
    struct TBattleInfo {
        int iTotalDamage;
        int iAggro;
    };
    struct SAttackLog {
        DWORD dwVID;
        DWORD dwTime;
    };
    struct SAttackedLog {
        DWORD dwPID;
        DWORD dwAttackedTime;
    };
    int m_iAvgDelta;
protected:
    CStateTemplate<CHARACTER> m_stateMove;
    CStateTemplate<CHARACTER> m_stateBattle;
    CStateTemplate<CHARACTER> m_stateIdle;
    DWORD m_dwStateDuration;
    DWORD m_dwPolymorphRace;
    bool m_bPolyMaintainStat;
    DWORD m_dwLoginPlayTime;
    DWORD m_dwPlayerID;
    VID m_vid;
    std::string m_stName;
    BYTE m_bCharType;
    CHARACTER_POINT m_points;
    CHARACTER_POINT_INSTANT m_pointsInstant;
    int m_iMoveCount;
    DWORD m_dwPlayStartTime;
    BYTE m_bAddChrState;
    bool m_bSkipSave;
    std::string m_stMobile;
    char m_szMobileAuth[5];
    BYTE m_bChatCounter;
    float m_fSyncTime;
    LPCHARACTER m_pkChrSyncOwner;
    CHARACTER_LIST m_kLst_pkChrSyncOwned;
    PIXEL_POSITION m_posDest;
    PIXEL_POSITION m_posStart;
    PIXEL_POSITION m_posWarp;
    long m_lWarpMapIndex;
    PIXEL_POSITION m_posExit;
    long m_lExitMapIndex;
    DWORD m_dwMoveStartTime;
    DWORD m_dwMoveDuration;
    DWORD m_dwLastMoveTime;
    DWORD m_dwLastAttackTime;
    DWORD m_dwWalkStartTime;
    DWORD m_dwStopTime;
    bool m_bWalking;
    bool m_bNowWalking;
    bool m_bStaminaConsume;
    TQuickslot m_quickslot[36];
    TAffectFlag m_afAffectFlag;
    std::list<CAffect* > m_list_pkAffect;
    CParty *m_pkParty;
    DWORD m_dwLastDeadTime;
    LPEVENT m_pkPartyRequestEvent;
    std::map<long unsigned int, event* > m_PartyInviteEventMap;
    CDungeon *m_pkDungeon;
    int m_iEventAttr;
    CGuild *m_pGuild;
    DWORD m_dwUnderGuildWarInfoMessageTime;
    CWarMap *m_pWarMap;
    bool m_bNoOpenedShop;
    bool m_bItemLoaded;
    int m_iRefineAdditionalCell;
    bool m_bUnderRefine;
    DWORD m_dwRefineNPCVID;
    LPSHOP m_pkShop;
    LPSHOP m_pkMyShop;
    std::string m_stShopSign;
    LPCHARACTER m_pkChrShopOwner;
    CExchange *m_pkExchange;
    DWORD m_dwFlyTargetID;
    std::vector<long unsigned int > m_vec_dwFlyTargets;
    std::map<VID, TBattleInfo > m_map_kDamage;
    DWORD m_dwKillerPID;
    int m_iAlignment;
    int m_iRealAlignment;
    int m_iKillerModePulse;
    BYTE m_bPKMode;
    float m_fSpeedHackMultipler;
    DWORD m_dwLastVictimSetTime;
    int m_iMaxAggro;
    LPCHARACTER m_pkChrStone;
    std::set<CHARACTER* > m_set_pkChrSpawnedBy;
    DWORD m_dwDropMetinStone;
    BYTE m_bDropMetinStonePct;
private:
    bool m_bDisableCooltime;
    BYTE m_bComboIndex;
    DWORD m_dwLastSkillTime;
protected:
    DWORD m_adwMobSkillCooltime[5];
private:
    LPEVENT m_pkMuyeongEvent;
    int m_iChainLightingIndex;
    std::set<CHARACTER* > m_setExceptChainLighting;
    DWORD m_dwAffectedEunhyungLevel;
protected:
    TPlayerSkill *m_pSkillLevels;
    std::map<int, int > m_SkillDamageBonus;
    std::map<int, TSkillUseInfo > m_SkillUseInfo;
public:
    VID m_kVIDVictim;
protected:
    DWORD m_dwLastChangeAttackPositionTime;
    CTrigger m_triggerOnClick;
    LPCHARACTER m_pkChrTarget;
    std::set<CHARACTER* > m_set_pkChrTargetedBy;
    CSafebox *m_pkSafebox;
    int m_iSafeboxSize;
    int m_iSafeboxLoadTime;
    bool m_bOpeningSafebox;
    CSafebox *m_pkMall;
    int m_iMallLoadTime;
    PIXEL_POSITION m_posSafeboxOpen;
    LPCHARACTER m_chHorse;
    LPCHARACTER m_chRider;
    DWORD m_dwMountVnum;
    BYTE m_bSendHorseLevel;
    BYTE m_bSendHorseHealthGrade;
    BYTE m_bSendHorseStaminaGrade;
private:
    bool m_bDetailLog;
    bool m_bMonsterLog;
protected:
    BYTE m_bEmpire;
    PIXEL_POSITION m_posRegen;
    float m_fRegenAngle;
    LPREGEN m_pkRegen;
private:
    DWORD m_dwQuestNPCVID;
    DWORD m_dwQuestByVnum;
    LPITEM m_pQuestItem;
protected:
    DWORD m_dwNextStatePulse;
private:
    WeddingMap *m_pWeddingMap;
    LPCHARACTER m_pkChrMarried;
public:
    LPEVENT m_pkDeadEvent;
    LPEVENT m_pkStunEvent;
    LPEVENT m_pkSaveEvent;
    LPEVENT m_pkRecoveryEvent;
    LPEVENT m_pkTimedEvent;
    LPEVENT m_pkFishingEvent;
    LPEVENT m_pkAffectEvent;
    LPEVENT m_pkPoisonEvent;
    LPEVENT m_pkFireEvent;
    std::map<int, event* > m_mapMobSkillEvent;
    LPEVENT m_pkWarpNPCEvent;
    LPEVENT m_pkMiningEvent;
    LPEVENT m_pkWarpEvent;
    LPEVENT m_pkCheckSpeedHackEvent;
    bool m_bHasPoisoned;
    const CMob *m_pkMobData;
    CMobInstance *m_pkMobInst;
private:
    int m_aiPremiumTimes[9];
    bool m_isinPCBang;
    CArena *m_pArena;
    bool m_ArenaObserver;
    int m_nPotionLimit;
    bool m_isOpenSafebox;
public:
    int m_iRefineTime;
    int m_iSeedTime;
    int m_iExchangeTime;
    int m_iKillCount;
    int m_iKillingCount;
    int m_iMyShopTime;
    DWORD m_dwMonarchCooltime[5];
    DWORD m_dwMonarchCooltimelimit[5];
private:
    e_overtime m_eOverTime;
    DWORD m_deposit_pulse;
public:
    SAttackLog m_kAttackLog;
    SAttackedLog m_AttackedLog;
    int m_speed_hack_count;
private:
    std::string m_strNewName;
protected:
    virtual void EncodeInsertPacket(CEntity *p0);
    virtual void EncodeRemovePacket(CEntity *p0);
public:
    virtual void StateMove();
    virtual void StateBattle();
    virtual void StateIdle();
    virtual void StateFlag();
    virtual void StateFlagBase();
    virtual void BeginStateEmpty();
    virtual void EndStateEmpty();
    CHARACTER();
    virtual ~CHARACTER();
    void SetPolymorph(DWORD race,bool maintain);
    void MountVnum(DWORD vnum);
    void ComputePoints();
    void ComputeSkillPoints();
    void ComputeBattlePoints();
    void PointChange(BYTE type,int amount,bool includeAmount,bool broadcast);
    void RefreshAffect();
    void ComputeAffect(CAffect* affect,bool add);
    void StartMuyeongEvent();
    void StopMuyeongEvent();
    void HorseSummon(bool summon,bool fromFar);
    void Save();
    void Disconnect();
    void Create(const char* name,DWORD vid,bool isPC);
    LPITEM GetItem(BYTE cell);
    LPITEM GetWear(BYTE cell);
    void SetItem(BYTE cell,LPITEM item);
    LPITEM FindSpecifyItem(DWORD vnum);
    void ClearKillCount();
    void ResetStopTime();
    DWORD GetStopTime();
    void InitMC();
    DWORD GetMC(MONARCH_INDEX index);
    DWORD GetMCL(MONARCH_INDEX index);
    void SetMC(MONARCH_INDEX index);
    bool IsMCOK(MONARCH_INDEX index);
    DWORD GetMCLTime(MONARCH_INDEX index);
    void SetPoint(BYTE type,long value);
    DWORD GetMotionMode();
    float GetMoveMotionSpeed();
    void CalculateMoveDuration();
    long GetRealPoint(BYTE type);
    void SetRealPoint(BYTE type,long value);
    long GetPoint(BYTE type);
    long GetLimitPoint(BYTE type);
    void SetPart(BYTE part,WORD value);
    WORD GetPart(BYTE part);
    WORD GetOriginalPart(BYTE part);
    void UpdatePacket();
    void ShowAlignment(bool show);
    CAffect* FindAffect(DWORD type,BYTE apply = 0);
    WORD GetRaceNum();
    // Player-ID load corroborated in DestroyCharacter0818c490.
    DWORD GetPlayerID() { return m_dwPlayerID; }
    int GetLevel() { return m_points.level; }
    const char * GetName();
    const VID& GetVID() const { return m_vid; }
    BYTE GetCharType();
    void StartStateMachine(int nextPulse = 1);
    void StopStateMachine();
    void ChatPacket(BYTE type,const char* format,...);
    void MonsterLog(const char* format,...);
    void SetPosition(int position);
    virtual void SetHorseLevel(int p0);
    virtual bool StartRiding();
    virtual bool StopRiding();
    virtual DWORD GetMyHorseVnum();
    virtual void HorseDie();
    virtual bool ReviveHorse();
    virtual void SendHorseInfo();
    virtual void ClearHorseInfo();
};
#endif


