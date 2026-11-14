// Complete old CHARACTER data and virtual interface; nonvirtual API recovery is partial.
#ifndef MCNCC_GAME_CHAR_H
#define MCNCC_GAME_CHAR_H
#include "constants.h"
#include <cstdint>
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

enum EDamageType {
    DAMAGE_TYPE_NONE = 0,
    DAMAGE_TYPE_NORMAL = 1,
    DAMAGE_TYPE_NORMAL_RANGE = 2,
    DAMAGE_TYPE_MELEE = 3,
    DAMAGE_TYPE_RANGE = 4,
    DAMAGE_TYPE_FIRE = 5,
    DAMAGE_TYPE_ICE = 6,
    DAMAGE_TYPE_ELEC = 7,
    DAMAGE_TYPE_MAGIC = 8,
    DAMAGE_TYPE_POISON = 9,
    DAMAGE_TYPE_SPECIAL = 10,
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
struct TPacketAffectElement;
class CMob;
struct SMobSkillInfo;
class CMobInstance;
class CParty;
struct packet_motion;
struct packet_party_update;
typedef packet_party_update TPacketGCPartyUpdate;
class CSafebox;
class CShop;
class CWarMap;
namespace marriage { class WeddingMap; }
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

    // The 2009 insertion path initializes only bUsed.
    TSkillUseInfo() : bUsed(false) {}
    bool HitOnce(DWORD vnum = 0);
    void ResetHitCount();
    bool UseSkill(bool grandMaster, DWORD vid, DWORD cooldown, int splashCount = 1, int hitCount = -1, int range = -1);
};
enum class PartyJoinError {
    None, Server, Dungeon, Observer, LevelBoundary, LowLevel, HighLevel, AlreadyJoined, Full, Separator, Empire,
};

enum ESex { SEX_MALE = 0, SEX_FEMALE = 1 };
ESex GET_SEX(CHARACTER* character);

class CHARACTER : public CEntity, public CFSM, public CHorseRider {
    friend class ITEM_MANAGER;
    friend class CInputDB;
    friend class CExchange;
    friend class CArena;
    friend class CInputMain;
    friend struct FuncForgetMyAttacker;
public:
    void SetQuestNPCID(DWORD vid);
    void OnClick(LPCHARACTER causer);
    DWORD GetQuestNPCID() { return m_dwQuestNPCVID; }
    DWORD GetQuestBy() { return m_dwQuestByVnum; }
    void SetQuestBy(DWORD index) { m_dwQuestByVnum = index; }
    LPCHARACTER GetQuestNPC();
    LPCHARACTER FindCharacterInView(const char* name, bool pcOnly);
    CItem* GetQuestItemPtr();
    void SetQuestItemPtr(CItem* item);
    void ClearQuestItemPtr();

    // Original char.h API, complete existingCHAR record retained.
    void ConfirmWithMsg(const char*,int,DWORD);

CSafebox* GetSafebox();
    void SetSafeboxSize(int size);
    int GetSafeboxSize();
    void QuerySafeboxSize();
    float GetDistanceFromSafeboxOpen();
    void SetSafeboxOpenPosition();
    void ReqSafeboxLoad(const char* password);
    void LoadSafebox(int size, DWORD gold, int count, TPlayerItem* items);
    void ChangeSafeboxSize(BYTE size);
    void CloseSafebox();
    void LoadMall(int count, TPlayerItem* items);
    void CloseMall();
    void ReqMallLoad(const char* password);
    bool RegisterMobile(const char* number);
    void AuthMobile(const char* code);
    void CloseMallFromClient();
CSafebox* GetMall();

bool IsAffectFlag(DWORD flag);
void CreateFly(BYTE type,LPCHARACTER victim);

    bool BuildUpdatePartyPacket(TPacketGCPartyUpdate& out);
long GetPolymorphPoint(BYTE type);
int GetPolymorphPower();
int GetSkillLevel(DWORD dwVnum);
const TMobTable& GetMobTable();

// Insert in game/src/char.h; original non-const methods.
void BroadcastTargetPacket();
    void SetStamina(int stamina) { m_points.stamina = stamina; }
    int GetStamina() { return m_points.stamina; }
    void SetMaxStamina(int stamina) { m_pointsInstant.iMaxStamina = stamina; }
    int GetMaxStamina() { return m_pointsInstant.iMaxStamina; }

    bool IsWalking() { return m_bNowWalking || GetStamina() <= 0; }
    void SetWalking(bool walking) { m_bWalking = walking; }
    void SetNowWalking(bool bWalkFlag);
    void ResetWalking() { SetNowWalking(m_bWalking); }
void SetWalkingMode(bool walking);

void SetLevel(BYTE level);
BYTE GetGMLevel(); // Old API is NON-CONST, unlike newer reference.
void SetPKMode(BYTE bPKMode);

// Insert in game/src/char.h. Old methods are NON-CONST; verify linkage in metadata.
BYTE GetJob();
bool IsDead();
    bool IsAlive() { return m_pointsInstant.position != POS_DEAD; }
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
    std::uint32_t m_dwPolymorphRace;
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
    const std::list<CAffect*>& GetAffectContainer() { return m_list_pkAffect; }
    CParty *m_pkParty;
    DWORD m_dwLastDeadTime;
    LPEVENT m_pkPartyRequestEvent;
    void SetPartyRequestEvent(LPEVENT event) { m_pkPartyRequestEvent = event; }
    std::map<long unsigned int, event* > m_PartyInviteEventMap;
    CDungeon *m_pkDungeon;
    int m_iEventAttr;
    CGuild *m_pGuild;
    DWORD m_dwUnderGuildWarInfoMessageTime;
    CWarMap *m_pWarMap;
    bool m_bNoOpenedShop;
    bool m_bItemLoaded;
    bool IsItemLoaded() { return m_bItemLoaded; }
    void SetItemLoaded() { m_bItemLoaded = true; }
    int m_iRefineAdditionalCell;
    bool m_bUnderRefine;
    DWORD m_dwRefineNPCVID;
    LPSHOP m_pkShop;
    LPSHOP m_pkMyShop;
    bool m_bNoOpenedShop;
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
    std::uint32_t m_dwMountVnum;
    BYTE m_bSendHorseLevel;
    BYTE m_bSendHorseHealthGrade;
    BYTE m_bSendHorseStaminaGrade;
private:
    long double GetMoveMotionSpeedPrecise();
    bool m_bDetailLog;
    bool m_bMonsterLog;
protected:
    BYTE m_bEmpire;
    PIXEL_POSITION m_posRegen;
    float m_fRegenAngle;
    LPREGEN m_pkRegen;
private:
    std::uint32_t m_dwQuestNPCVID;
    DWORD m_dwQuestByVnum;
    LPITEM m_pQuestItem;
protected:
    DWORD m_dwNextStatePulse;
private:
    marriage::WeddingMap *m_pWeddingMap;
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
    bool IsWarping() { return m_pkWarpEvent != nullptr; }
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
    void SetPotionLimit(int count) { m_nPotionLimit = count; }
    int GetPotionLimit() { return m_nPotionLimit; }
    bool m_isOpenSafebox;
public:
    int m_iRefineTime;
    int m_iSeedTime;
    int GetUseSeedOrMoonBottleTime() { return m_iSeedTime; }
    void SetUseSeedOrMoonBottleTime();
    int m_iExchangeTime;
    int m_iKillCount;
    int m_iKillingCount;
    int m_iMyShopTime;
    int GetMyShopTime() { return m_iMyShopTime; }
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
    void Destroy();
    void SetRotation(float rotation);
    float GetRotation() { return m_pointsInstant.fRot; }
    void fishing();
    void fishing_take();
    bool UseItem(BYTE cell, int destination = -1);
    bool ItemProcess_Polymorph(CItem* item);
    bool UseItemEx(CItem* item, int destination);
    void ClearItem();
    bool UseRefineItem(CItem* item, int destination);
    bool UseItemModification(CItem* item, int destination, BYTE subtype);
    bool RefineItem(CItem* item, CItem* target);
    bool RefineInformation(BYTE cell, BYTE type, int additionalCell);
    void SetRefineMode(int additionalCell);
    void ClearRefineMode();
    bool UsePotionItem(CItem* item, BYTE subtype);
    bool UseConsumableItem(CItem* item, BYTE subtype);
    bool UseAbilityItem(CItem* item, DWORD sharedAffect);
    bool ItemProcess_Hair(CItem* item, int destination);
    bool UseMetinStone(CItem* item, int destination);
    bool UseRecallItem(CItem* item);
    bool GiveRecallItem(CItem* item);
    void ProcessRecallItem(CItem* item);
    bool GiveItem(LPCHARACTER target, BYTE cell);
    bool CanReceiveItem(LPCHARACTER from, LPITEM item);
    void ReceiveItem(LPCHARACTER from, LPITEM item);
    void SetEmpire(BYTE empire);
    void SetShop(LPSHOP shop);
    LPSHOP GetShop() const { return m_pkShop; }
    LPSHOP GetMyShop() const { return m_pkMyShop; }
    void CloseMyShop();
    void OpenMyShop(const char* sign, TShopItemTable* table, BYTE count);
    void SetShopOwner(CHARACTER* owner) { m_pkChrShopOwner = owner; }
    void SetMyShopTime();
    int GetRefineTime() { return m_iRefineTime; }
    void SetRefineTime();
    int GetExchangeTime() { return m_iExchangeTime; }
    void SetExchangeTime();
    CHARACTER* GetShopOwner() const { return m_pkChrShopOwner; }
    long GetAllowedGold();
    void SetGold(long gold) { m_points.gold = gold; }
    int GetGold() { return m_points.gold; }
    bool CanHandleItem(bool skipRefine = false, bool skipObserver = false);
    void PointChange(BYTE type, int amount, bool amountFlag = false, bool broadcast = false);
    void ApplyPoint(BYTE type, long value);
    void ComputeBattlePoints();
    void ComputePoints();
    void ComputeAffect(CAffect* affect, bool add);
    void LoadAffect(DWORD count, TPacketAffectElement* elements);
    void RefreshAffect();
    void StartMuyeongEvent();
    void MountVnum(DWORD vnum);
    void SetPolymorph(DWORD vnum, bool maintainStat);
    LPITEM AutoGiveItem(DWORD vnum, BYTE count, int rarePercent, bool message);
    bool GiveItemFromSpecialItemGroup(DWORD groupVnum, DWORD& itemVnum, DWORD& itemCount, LPITEM& item);
    void CancelSafeboxLoad() { m_bOpeningSafebox = false; }
    void SetOpenSafebox(bool open) { m_isOpenSafebox = open; }
    int GetSafeboxLoadTime() { return m_iSafeboxLoadTime; }
    void SetSafeboxLoadTime();
    int GetMallLoadTime() { return m_iMallLoadTime; }
    void SetMallLoadTime(int pulse) { m_iMallLoadTime = pulse; }
    bool IsOpenSafebox() const { return m_isOpenSafebox; }
    LPITEM* GetCubeItem() { return m_pointsInstant.pCubeItems; }
    void SetCubeNpc(LPCHARACTER npc) { m_pointsInstant.pCubeNpc = npc; }
    bool IsCubeOpen() const { return m_pointsInstant.pCubeNpc != 0; }
    int ChangeEmpire(BYTE empire);
    int GetChangeEmpireCount();
    void SetChangeEmpireCount();
    DWORD GetAID();
    BYTE GetEmpire() { return m_bEmpire; }
    CGuild* GetGuild() { return m_pGuild; }
    void SetGuild(CGuild* guild);
    BYTE GetPKMode();
    int GetAlignment();
    bool IsKillerMode();
    void SetKillerMode(bool on);
    bool RequestToParty(CHARACTER* leader);
    void AcceptToParty(CHARACTER* member);
    void DenyToParty(CHARACTER* member);
    friend long party_request_event(LPEVENT event);
    void PartyInvite(CHARACTER* guest);
    void PartyInviteAccept(CHARACTER* guest);
    void PartyInviteDeny(DWORD guestPid);
    void PartyJoin(CHARACTER* leader);
    static PartyJoinError IsPartyJoinableCondition(CHARACTER* leader, CHARACTER* guest);
    static PartyJoinError IsPartyJoinableMutableCondition(CHARACTER* leader, CHARACTER* guest);
    bool CanSummon(int leadership);
    CParty* GetParty() { return m_pkParty; }
    void SetParty(CParty* party);
    DWORD GetMountVnum() { return m_dwMountVnum; }
    bool IsPolymorphed() { return m_dwPolymorphRace != 0; }
    DWORD GetPolymorphVnum() { return m_dwPolymorphRace; }
    bool IsPolyMaintainStat() { return m_bPolyMaintainStat; }
    bool IsRaceFlag(DWORD bit);
    bool IsBerserk();
    float GetMobDamageMultiply();
    int GetMarriageBonus(DWORD itemVnum, bool sum);
    void AssignTriggers(const TMobTable* table);
    bool OnIdle();
    CExchange* GetExchange() const { return m_pkExchange; }
    void SetExchange(CExchange* exchange);
    bool ExchangeStart(CHARACTER* victim);
    LPCHARACTER GetVictim();
    void ChangeVictimByAggro(int aggro, LPCHARACTER attacker);
    void UpdateAggrPointEx(LPCHARACTER attacker, EDamageType type, int damage, TBattleInfo& info);
    void UpdateAggrPoint(LPCHARACTER attacker, EDamageType type, int damage);
    void SetRider(LPCHARACTER rider);
    void Motion(BYTE motion, LPCHARACTER victim);
    void MotionPacketEncode(BYTE motion, LPCHARACTER victim, packet_motion* packet);
    void SendMovePacket(BYTE function,BYTE argument,DWORD x,DWORD y,DWORD duration,DWORD time,int rotation);
    void Save();
    void StopMuyeongEvent();
    void ComputeSkillPoints();
    int CountEmptyInventory();
    int GetEmptyInventory(BYTE size);
    bool IsEmptyItemGrid(BYTE cell,BYTE size,int exception);
    bool GetQuickslot(BYTE pos,TQuickslot** slot);
    bool SetQuickslot(BYTE pos,TQuickslot& slot);
    bool SwapQuickslot(BYTE first, BYTE second);
    bool DelQuickslot(BYTE pos);
    void SyncQuickslot(BYTE type,BYTE oldPos,BYTE newPos);
    void ChainQuickslotItem(LPITEM item,BYTE type,BYTE oldPos);
    void SetImmuneFlag(DWORD value) { m_pointsInstant.dwImmuneFlag = static_cast<std::uint32_t>(value); }
    void Disconnect();
    void SetWarMap(CWarMap* map);
    CWarMap* GetWarMap() const { return m_pWarMap; }
    CArena* GetArena() const { return m_pArena; }
    void SetArena(CArena* arena) { m_pArena = arena; }
    bool GetArenaObserverMode() const { return m_ArenaObserver; }
    void SetArenaObserverMode(bool enabled) { m_ArenaObserver = enabled; }
    void SetWeddingMap(marriage::WeddingMap* map);
    marriage::WeddingMap* GetWeddingMap() { return m_pWeddingMap; }
    void SetMarryPartner(CHARACTER* partner);
    LPCHARACTER GetMarryPartner();
    bool IsPCBang() { return m_isinPCBang; }
    bool IsInPCBang() const { return m_isinPCBang; }
    void DeathPenalty(BYTE penalty);
    void ItemDropPenalty(LPCHARACTER killer);
    void Stun();
    friend long dead_event(LPEVENT event);
    bool CanDoCube();
    friend void Cube_clean_item(LPCHARACTER character);
    friend void Cube_close(LPCHARACTER character);
    friend void Cube_open(LPCHARACTER character);
    friend void Cube_add_item(LPCHARACTER character, int cube, int inventory);
    friend void Cube_delete_item(LPCHARACTER character, int cube);
    friend void Cube_show_list(LPCHARACTER character);
    friend bool Cube_make(LPCHARACTER character);
    void SaveReal();
    void SetRegen(LPREGEN record);
    static void InvalidateRegen(LPREGEN record);
    CDungeon* GetDungeon() { return m_pkDungeon; }
    CDungeon* GetDungeonForce();
    void SetDungeon(CDungeon* dungeon);
    void SetStone(CHARACTER* stone);
    void ClearStone();
    bool IsSyncOwner(LPCHARACTER owner);
    bool SetSyncOwner(LPCHARACTER owner, bool removeFromList = true);
    void ClearSync();
    void SetAggressive();
    void SetCoward();
    void CreatePlayerProto(TPlayerTable& table);
    void SetPlayerProto(const TPlayerTable* table);
    void SetGMLevel();
    void SetRace(BYTE race);
    void ResetPlayTime(DWORD remaining);
    int GetRealAlignment();
    bool IsEquipUniqueItem(DWORD vnum);
    void UpdateAlignment(int amount);
    void FlushDelayedSaveItem();
    void SaveAffect();
    void Create(const char* name,DWORD vid,bool isPC);
    int CountSpecifyItem(DWORD vnum);
    void RemoveSpecifyItem(DWORD vnum, DWORD count);
    LPITEM GetItem(BYTE cell);
    LPITEM GetWear(BYTE cell);
    void SendEquipment(CHARACTER* viewer);
    void SetWear(BYTE cell, LPITEM item);
    bool RemoveAffect(CAffect* affect);
    friend void do_affect_remove(CHARACTER*, char*, int, int);
    bool RemoveAffect(DWORD type);
    void ClearAffect(bool save = false);
    void CheckMaximumPoints();
    void SetItem(BYTE cell,LPITEM item);
    LPITEM FindSpecifyItem(DWORD vnum);
    void ClearKillCount();
    void SetKillCount(int count) { m_iKillCount = count; }
    int GetKillCount() { return m_iKillCount; }
    void DecKillCount();
    void ClearKillingCount() { m_iKillingCount = 0; }
    void IncKillingCount();
    int GetKillingCount() { return m_iKillingCount; }
    void ResetStopTime();
    void ResetChangeAttackPositionTime();
    void SetChangeAttackPositionTime();
    DWORD GetStopTime();
    void InitMC();
    DWORD GetMC(MONARCH_INDEX index);
    DWORD GetMCL(MONARCH_INDEX index);
    void SetMC(MONARCH_INDEX index);
    bool IsMCOK(MONARCH_INDEX index);
    DWORD GetMCLTime(MONARCH_INDEX index);
    void SetPoint(BYTE type,long value);
    DWORD GetMotionMode();
    float GetMoveSpeed();
    bool IsChangeAttackPosition(LPCHARACTER target);
    bool Return();
    bool Follow(LPCHARACTER target, float minDistance);
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
    bool IsSiegeNPC();
    void DetermineDropMetinStone();
    // Player-ID load corroborated in DestroyCharacter0818c490.
    DWORD GetPlayerID() { return m_dwPlayerID; }
    void SetRandomHP(int hp) { m_points.iRandomHP = hp; }
    void SetRandomSP(int sp) { m_points.iRandomSP = sp; }
    int GetRandomHP() { return m_points.iRandomHP; }
    int GetRandomSP() { return m_points.iRandomSP; }
    int GetHP() { return m_points.hp; }
    int GetMaxHP() { return m_pointsInstant.iMaxHP; }
    void SetHP(int hp) { m_points.hp = hp; }
    void SetMaxHP(int hp) { m_pointsInstant.iMaxHP = hp; }
    void SetSP(int sp) { m_points.sp = sp; }
    int GetSP() { return m_points.sp; }
    void SetMaxSP(int sp) { m_pointsInstant.iMaxSP = sp; }
    int GetMaxSP() { return m_pointsInstant.iMaxSP; }
    void SetExp(DWORD exp) { m_points.exp = exp; }
    DWORD GetExp() { return m_points.exp; }
    void ResetPoint(int level);
    int GetLevel() { return m_points.level; }
    bool ChangeSex();
    void SetSkillGroup(BYTE group);
    BYTE GetSkillGroup() { return m_points.skill_group; }
// Add #include "constants.h" outside the class in char.h.
// Original inline char.h584; nonconstDWORD GetNextExp(), no extra NULL guard.
DWORD GetNextExp() {
    if(m_points.level>=99) return 2000000000UL;
    return exp_table[m_points.level];
}

    const std::string& GetNewName() { return m_strNewName; }
    void SetNewName(std::string name) { m_strNewName = name; }
    const char * GetName();
    void SetName(const char* name) { m_stName.assign(name); }
    const VID& GetVID() const { return m_vid; }
    void MainCharacterPacket();
    void PointsPacket();
    void SkillLevelPacket();
    BYTE GetCharType();
    bool IsWarp() { return m_bCharType == CHAR_TYPE_WARP; }
    bool IsGoto() { return m_bCharType == CHAR_TYPE_GOTO; }
    bool IsPC() { return GetDesc() != nullptr; }
    bool IsNPC() { return m_bCharType != CHAR_TYPE_PC; }
    bool IsDoor() { return m_bCharType == CHAR_TYPE_DOOR; }
    bool IsBuilding() { return m_bCharType == CHAR_TYPE_BUILDING; }
    bool IsMonster() { return m_bCharType == CHAR_TYPE_MONSTER; }
    bool IsStone() { return m_bCharType == CHAR_TYPE_STONE; }
    void StartStateMachine(int nextPulse = 1);
    void StopStateMachine();
    void Command(const char* argument);
    void ChatPacket(BYTE type,const char* format,...);
    void DetailLog() { m_bDetailLog = !m_bDetailLog; }
    void ToggleMonsterLog();
    void MonsterLog(const char* format,...);
    void OnMove(bool isAttack = false);
    bool Move(long x, long y);
    void StartStaminaConsume();
    bool IsStaminaConsume();
    bool IsStaminaHalfConsume();
    void StopStaminaConsume();
    void SetSpeedHackMultipler(float factor);
    DWORD GetMoveDuration() const { return m_dwMoveDuration; }
    DWORD GetCurrentMoveDuration() { return m_dwMoveDuration; }
    DWORD GetWalkStartTime() { return m_dwWalkStartTime; }
    DWORD GetLastMoveTime() { return m_dwLastMoveTime; }
    DWORD GetLastAttackTime() { return m_dwLastAttackTime; }
    bool IsUsableSkillMotion(DWORD motion);
    void mining(CHARACTER* vein);
    void mining_take();
    void SetRefineNPC(CHARACTER* npc);
    bool IsRefineThroughGuild();
    CGuild* GetRefineGuild();
    bool DoRefine(CItem* item, bool moneyOnly);
    bool DoRefineWithScroll(CItem* item);
    int ComputeRefineFee(int cost, int multiplier);
    void PayRefineFee(int total);
    bool CanDeposit();
    void UpdateDepositPulse();
    void mining_cancel();
    bool CannotMoveByAffect();
    bool CanMove();
    void AggregateMonster();
    void ForgetMyAttacker();
    void AttractRanger();
    void PullMonster();
    DWORD GetMobDamageMin();
    DWORD GetMobDamageMax();
    BYTE GetMobSize();
    int GetLeadershipSkillLevel();
    BYTE GetMobBattleType();
    WORD GetMobAttackRange();
    bool CanFight() const;
    bool CanBeginFight();
    bool Attack(LPCHARACTER victim, BYTE type = 0);
    friend bool timed_event_cancel(CHARACTER* character);
    void BeginFight(LPCHARACTER victim);
    void SetVictim(LPCHARACTER victim);
    void SetNextStatePulse(int nextPulse);
    DWORD GetStateDuration() { return m_dwStateDuration; }
    void SetAffectedEunhyung();
    void ClearAffectedEunhyung() { m_dwAffectedEunhyungLevel = 0; }
    bool GetAffectedEunhyung() { return m_dwAffectedEunhyungLevel != 0; }
    void SetPosition(int position);
    void Sitdown(int isGround);
    void Standup();
    BYTE IncreaseChatCounter() { return ++m_bChatCounter; }
    void ResetChatCounter() { m_bChatCounter = 0; }
    DWORD GetLastShoutPulse() { return m_pointsInstant.dwLastShoutPulse; }
    void SetLastShoutPulse(DWORD pulse) { m_pointsInstant.dwLastShoutPulse = pulse; }
    void UpdateStateMachine(DWORD pulse);
    int GetPosition() { return m_pointsInstant.position; }
    bool IsPosition(int position) { return m_pointsInstant.position == position; }
    void SyncPacket();
    bool IsGoodAffect(BYTE type);
    void RemoveBadAffect();
    void RemoveGoodAffect();
    void AttackedByPoison(LPCHARACTER attacker);
    void AttackedByFire(LPCHARACTER attacker, int amount, int count);
    bool IsImmune(DWORD flag);
    BYTE GetMobRank();
    bool HasReviverInParty();
    bool IsRevive();
    void SetRevive(bool mode);
    bool IsReviver();
    LPCHARACTER DistributeExp();
    void DistributeHP(LPCHARACTER killer);
    void RewardGold(LPCHARACTER attacker);
    void Reward(bool itemDrop);
    void GiveGold(int amount);
    bool DropGold(long gold);
    bool DropItem(BYTE cell, BYTE count);
    bool PickupItem(DWORD vid);
    bool MoveItem(BYTE cell, BYTE destination, BYTE count);
    bool SwapItem(BYTE cell, BYTE destination);
    bool EquipItem(CItem* item);
    bool UnequipItem(CItem* item);
    DWORD GetMobDropItemVnum();
    DWORD GetDropMetinStoneVnum() { return m_dwDropMetinStone; }
    BYTE GetDropMetinStonePct() { return m_bDropMetinStonePct; }
    DWORD GetPolymorphItemVnum();
    bool IsOverTime(e_overtime value) { return m_eOverTime == value; }
    e_overtime GetOverTime() const { return m_eOverTime; }
    void SetOverTime(e_overtime value) { m_eOverTime = value; }
    int GetChainLightningIndex() { return m_iChainLightingIndex; }
    void IncChainLightningIndex()
    {
        m_iChainLightingIndex = static_cast<int>(static_cast<unsigned int>(m_iChainLightingIndex) + 1u);
    }
    void AddChainLightningExcept(LPCHARACTER character) { m_setExceptChainLighting.insert(character); }
    void ResetChainLightningIndex()
    {
        m_iChainLightingIndex = 0;
        m_setExceptChainLighting.clear();
    }
    int GetChainLightningMaxCount();
    const std::set<CHARACTER*>& GetChainLightingExcept() { return m_setExceptChainLighting; }
    bool Damage(LPCHARACTER attacker, int amount, EDamageType type = DAMAGE_TYPE_NORMAL);
    bool __Profile__Damage(LPCHARACTER attacker, int amount, EDamageType type = DAMAGE_TYPE_NORMAL);
    void SendDamagePacket(LPCHARACTER attacker, int amount, BYTE flag);
    void SetLastAttacked(DWORD time);
    int GetHPPct();
    DWORD GetAIFlag() { return m_pointsInstant.dwAIFlag; }
    bool IsDeathBlower();
    bool IsDeathBlow();
    bool IsStoneSkinner();
    bool IsAggressive();
    bool IsCoward();
    bool IsBerserker();
    bool IsGodSpeeder();
    bool IsNoAttackShinsu();
    bool IsNoAttackChunjo();
    bool IsNoAttackJinno();
    bool IsAttackMob();
    void SetNoAttackShinsu();
    void SetNoAttackChunjo();
    void SetNoAttackJinno();
    void SetAttackMob();
    void SetBerserk(bool enabled);
    bool IsGodSpeed();
    void SetGodSpeed(bool enabled);
    LPCHARACTER GetNearestVictim(LPCHARACTER origin);
    void __StateIdle_Stone();
    void __StateIdle_Monster();
    void __StateIdle_NPC();
    LPCHARACTER GetProtege();
    DWORD GetSummonVnum();
    bool IsSummonMonster();
    void CowardEscape();
    DWORD GetMonsterDrainSPPoint();
    friend struct FuncSplashDamage;
    friend long ChainLightningEvent(LPEVENT event);
    friend long mob_skill_hit_event(LPEVENT event);
    bool Sync(long x, long y);
    void SetRotationToXY(long x, long y);
    void Dead(LPCHARACTER killer = 0, bool immediate = false);
    void ProcessThreeWayDeath(LPCHARACTER killer);
    friend long poison_event(LPEVENT event);
    friend long fire_event(LPEVENT event);
    void RemovePoison();
    void RemoveFire();
    virtual void SetHorseLevel(int p0);
    bool IsHorseVisible();
    virtual bool StartRiding();
    virtual bool StopRiding();
    void HorseSummon(bool summon, bool fromFar);
    LPCHARACTER GetRider();
    void SetProto(const CMob* mob);
    bool Show(long map, long x, long y, long z, bool spawnMotion);
    bool Goto(long x, long y);
    void StartAffectEvent();
    void StartRecoveryEvent();
    void StartSaveEvent();
    void StartCheckSpeedHackEvent();
    void Say(const std::string& script);
    void SendGreetMessage();
    friend long recovery_event(LPEVENT event);
    void CheckTarget();
    void SetTarget(CHARACTER* target);
    LPCHARACTER GetTarget() { return m_pkChrTarget; }
    void ClearTarget();
    BOOL IsGM();
    void UpdateKillerMode();
    void DistributeSP(CHARACTER* recipient, int method);
    void SetBlockMode(BYTE mode);
    void SetBlockModeForce(BYTE mode);
    BYTE GetBlockMode() const { return m_pointsInstant.bBlockMode; }
    bool IsBlockMode(BYTE mask) { return (m_pointsInstant.bBlockMode & mask) != 0; }
    void ReviveInvisible(int duration);
    void EffectPacket(int effect);
    bool AddAffect(DWORD type, BYTE point, long value, DWORD flag, long duration,
                   long spCost, bool overrideExisting, bool isCube);
    bool UpdateAffect();
    int ProcessAffect();
    int GetPremiumRemainSeconds(BYTE type);
    void SetQuestFlag(const std::string& flag, int value);
    int GetQuestFlag(const std::string& flag);
    BOOL IsHack(bool sendMessage, bool checkShopOwner, int limitTime);
    bool WarpToPID(DWORD pid);
    bool WarpSet(long x, long y, long privateMapIndex);
    void WarpEnd();
    void SaveExitLocation();
    void ExitToSavedLocation();
    void SetWarpLocation(long map, long x, long y);
    void Stop();
    friend long delayed_warp_event(LPEVENT event);
    friend long delayed_timewarp_event(LPEVENT event);
    friend long warp_npc_event(LPEVENT event);
    void MonsterChat(BYTE type);
    bool IsGuardNPC();
    void ApplyMobAttribute(const TMobTable* table);
    void StartWarpNPCEvent();
    friend long kill_ore_load_event(LPEVENT event);
    void StateHorse();
    int ComputeSkill(DWORD vnum, LPCHARACTER victim, BYTE skillLevel = 0);
    int ComputeCooltime(int time);
    bool UseSkill(DWORD vnum, LPCHARACTER victim, bool useGrandMaster = true);
    int ComputeSkillAtPosition(DWORD vnum, const PIXEL_POSITION& position, BYTE skillLevel = 0);
    void SetSkillLevel(DWORD vnum, BYTE level);
    void DisableCooltime();
    bool HasMobSkill();
    int CountMobSkill();
    const SMobSkillInfo* GetMobSkill(int index);
    bool CanUseMobSkill(int index);
    bool UseMobSkill(int index);
    void ResetMobSkillCooltime();
    void ResetSkill();
    void ComputePassiveSkill(DWORD vnum);
    void ClearSkill();
    void ClearSubSkill();
    void ResetOneSkill(DWORD vnum);
    bool UseSkillBookItem(CItem* item);
    void UseSilkBotary();
    void UseSilkBotaryReal(const TPacketMyshopPricelistHeader* packet);
    void __OpenPrivateShop();
    void SendMyShopPriceListCmd(DWORD vnum, DWORD price);
    bool LearnSkillByBook(DWORD vnum, BYTE probability = 0);
    void GiveRandomSkillBook();
    bool SkillLevelDown(DWORD vnum);
    void SkillLearnWaitMoreTimeMessage(DWORD seconds);
    bool LearnGrandMasterSkill(DWORD vnum);
    bool IsLearnableSkill(DWORD vnum);
    void SkillLevelUp(DWORD vnum, BYTE method);
    time_t GetSkillNextReadTime(DWORD vnum);
    void SetSkillNextReadTime(DWORD vnum, time_t time);
    int GetSkillMasterType(DWORD vnum);
    int GetUsedSkillMasterType(DWORD vnum);
    int GetSkillPower(DWORD vnum, BYTE requestedLevel = 0);
    int GetSkillPowerByLevel(int level, bool mob = false);
    bool IsEquipUniqueGroup(DWORD group);
    int GetArrowAndBow(LPITEM* bow, LPITEM* arrow, int count);
    void UseArrow(LPITEM arrow, DWORD count);
    void FlyTarget(DWORD targetVID, long x, long y, BYTE header);
    bool Shoot(BYTE type);
    virtual DWORD GetMyHorseVnum();
    virtual void HorseDie();
    virtual bool ReviveHorse();
    virtual void SendHorseInfo();
    virtual void ClearHorseInfo();
};
#endif


