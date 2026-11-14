// Partial old account tables, recovered from desc.cpp DWARF.
#ifndef MCNCC_COMMON_TABLES_H
#define MCNCC_COMMON_TABLES_H
#include "../libthecore/include/typedef.h"
#include <cstdint>
#pragma pack(push, 1)
typedef struct SSimplePlayer {
    std::uint32_t dwID;
    char szName[25];
    BYTE byJob;
    BYTE byLevel;
    std::uint32_t dwPlayMinutes;
    BYTE byST;
    BYTE byHT;
    BYTE byDX;
    BYTE byIQ;
    WORD wMainPart;
    BYTE bChangeName;
    WORD wHairPart;
    BYTE bDummy[4];
    std::int32_t x;
    std::int32_t y;
    std::int32_t lAddr;
    WORD wPort;
    BYTE skill_group;
} TSimplePlayer;
static_assert(sizeof(TSimplePlayer) == 63, "2009 account character record");
typedef struct SPacketDGChangeName {
    std::uint32_t pid;
    char name[25];
} TPacketDGChangeName;
static_assert(sizeof(TPacketDGChangeName) == 29, "2009 packet width");
typedef struct SPacketDGCreateSuccess {
    BYTE bAccountCharacterIndex;
    TSimplePlayer player;
} TPacketDGCreateSuccess;
static_assert(sizeof(TPacketDGCreateSuccess) == 64, "2009 DB create success packet");
typedef struct SAccountTable {
    std::uint32_t id;
    char login[31];
    char passwd[17];
    char social_id[19];
    char status[9];
    BYTE bEmpire;
    TSimplePlayer players[4];
} TAccountTable;
static_assert(sizeof(TAccountTable) == 333, "2009 DB account record");
#pragma pack(pop)

// Recovered old mob, horse and character support records.
// Old packed mob records, recovered from char.cpp DWARF.
#ifndef MCNCC_COMMON_MOB_TABLES_FRAGMENT
#define MCNCC_COMMON_MOB_TABLES_FRAGMENT
#pragma pack(push, 1)
typedef struct SMobSkillLevel {
    std::uint32_t dwVnum;
    BYTE bLevel;
} TMobSkillLevel;
typedef struct SMobTable {
    std::uint32_t dwVnum;
    char szName[25];
    char szLocaleName[25];
    BYTE bType;
    BYTE bRank;
    BYTE bBattleType;
    BYTE bLevel;
    BYTE bSize;
    std::uint32_t dwGoldMin;
    std::uint32_t dwGoldMax;
    std::uint32_t dwExp;
    std::uint32_t dwMaxHP;
    BYTE bRegenCycle;
    BYTE bRegenPercent;
    WORD wDef;
    std::uint32_t dwAIFlag;
    std::uint32_t dwRaceFlag;
    std::uint32_t dwImmuneFlag;
    BYTE bStr;
    BYTE bDex;
    BYTE bCon;
    BYTE bInt;
    std::uint32_t dwDamageRange[2];
    short int sAttackSpeed;
    short int sMovingSpeed;
    BYTE bAggresiveHPPct;
    WORD wAggressiveSight;
    WORD wAttackRange;
    char cEnchants[6];
    char cResists[11];
    std::uint32_t dwResurrectionVnum;
    std::uint32_t dwDropItemVnum;
    BYTE bMountCapacity;
    BYTE bOnClickType;
    BYTE bEmpire;
    char szFolder[65];
    float fDamMultiply;
    std::uint32_t dwSummonVnum;
    std::uint32_t dwDrainSP;
    std::uint32_t dwMobColor;
    std::uint32_t dwPolymorphItemVnum;
    TMobSkillLevel Skills[5];
    BYTE bBerserkPoint;
    BYTE bStoneSkinPoint;
    BYTE bGodSpeedPoint;
    BYTE bDeathBlowPoint;
    BYTE bRevivePoint;
} TMobTable;
static_assert(sizeof(TMobSkillLevel) == 5, "2009 boot mob skill entry");
static_assert(sizeof(TMobTable) == 255, "2009 boot mob record");
#pragma pack(pop)

#endif

#pragma pack(push, 1)
struct SQuickslot {
    BYTE type;
    BYTE pos;
};
typedef SQuickslot TQuickslot;
#pragma pack(pop)

#pragma pack(push, 1)
struct THorseInfo {
    BYTE bLevel;
    BYTE bRiding;
    short sStamina;
    short sHealth;
    std::uint32_t dwHorseHealthDropTime;
};
static_assert(sizeof(THorseInfo) == 10, "2009 player horse record");
#pragma pack(pop)


typedef struct SLogoutPacket { char login[31]; char passwd[17]; } TLogoutPacket;
static_assert(sizeof(TLogoutPacket) == 48, "2009 DB logout packet");

// Old item records: item.cpp DWARF, packed layout.
#pragma pack(push, 1)
struct TPlayerItemAttribute {
    BYTE bType;
    short int sValue;
};
struct SItemLimit {
    BYTE bType;
    std::int32_t lValue;
};
typedef SItemLimit TItemLimit;
struct SItemApply {
    BYTE bType;
    std::int32_t lValue;
};
typedef SItemApply TItemApply;
struct SItemTable {
    std::uint32_t dwVnum;
    char szName[25];
    char szLocaleName[25];
    BYTE bType;
    BYTE bSubType;
    BYTE bWeight;
    BYTE bSize;
    std::uint32_t dwAntiFlags;
    std::uint32_t dwFlags;
    std::uint32_t dwWearFlags;
    std::uint32_t dwImmuneFlag;
    std::uint32_t dwGold;
    std::uint32_t dwShopBuyPrice;
    TItemLimit aLimits[2];
    TItemApply aApplies[3];
    std::int32_t alValues[6];
    std::int32_t alSockets[3];
    std::uint32_t dwRefinedVnum;
    WORD wRefineSet;
    BYTE bAlterToMagicItemPct;
    BYTE bSpecular;
    BYTE bGainSocketPct;
    short int sAddonType;
};
typedef SItemTable TItemTable;
static_assert(sizeof(TItemLimit) == 5, "2009 boot item limit entry");
static_assert(sizeof(TItemApply) == 5, "2009 boot item apply entry");
static_assert(sizeof(TItemTable) == 154, "2009 boot item prototype");
struct SPlayerItem {
    std::uint32_t id;
    BYTE window;
    WORD pos;
    std::uint32_t count;
    std::uint32_t vnum;
    std::int32_t alSockets[3];
    TPlayerItemAttribute aAttr[7];
    std::uint32_t owner;
};
typedef SPlayerItem TPlayerItem;
static_assert(sizeof(TPlayerItem) == 52, "2009 player item record");
struct TPacketAffectElement {
    std::uint32_t dwType;
    BYTE bApplyOn;
    std::int32_t lApplyValue;
    std::uint32_t dwFlag;
    std::int32_t lDuration;
    std::int32_t lSPCost;
};
static_assert(sizeof(TPacketAffectElement) == 21, "2009 affect wire size");
// Old DB party-member-level payload (party.cpp DWARF; common reference placement).
struct SPacketPartyCreate { std::uint32_t dwLeaderPID; };
typedef SPacketPartyCreate TPacketPartyCreate;
static_assert(sizeof(TPacketPartyCreate) == 4, "2009 DB party record");
struct SPacketPartyAdd { std::uint32_t dwLeaderPID; std::uint32_t dwPID; BYTE bState; };
typedef SPacketPartyAdd TPacketPartyAdd;
static_assert(sizeof(TPacketPartyAdd) == 9, "2009 DB party record");
struct SPacketPartyDelete { std::uint32_t dwLeaderPID; };
typedef SPacketPartyDelete TPacketPartyDelete;
static_assert(sizeof(TPacketPartyDelete) == 4, "2009 DB party record");
struct SPacketPartyRemove { std::uint32_t dwLeaderPID; std::uint32_t dwPID; };
typedef SPacketPartyRemove TPacketPartyRemove;
static_assert(sizeof(TPacketPartyRemove) == 8, "2009 DB party record");
struct SPacketPartySetMemberLevel { std::uint32_t dwLeaderPID; std::uint32_t dwPID; BYTE bLevel; };
typedef SPacketPartySetMemberLevel TPacketPartySetMemberLevel;
static_assert(sizeof(TPacketPartySetMemberLevel) == 9, "2009 DB party record");
typedef struct SPacketPartyStateChange
{
    std::uint32_t dwLeaderPID;
    std::uint32_t dwPID;
    BYTE bRole;
    BYTE bFlag;
} TPacketPartyStateChange;
static_assert(sizeof(TPacketPartyStateChange) == 10, "2009 DB party record");
// Insert into common/tables.h with the original packed records.
// Old DWARF: time_t -> __time_t -> __int32_t -> signed int (4 bytes).
// Store that exact underlying scalar explicitly; host CRT time_t may be 8 bytes.
// Do not redefine the CRT's global time_t or include time.h for this record.
#pragma pack(push, 1)
typedef struct SPlayerSkill {
    BYTE bMasterType;
    BYTE bLevel;
    int tNextRead; // original time_t, target signed int32, offset2
} TPlayerSkill;
#pragma pack(pop)

#pragma pack(pop)
// Insert in common/tables.h; complete old shared record (14 packed bytes).
#pragma pack(push, 1)
typedef struct SPacketGuildChangeMemberData {
    std::uint32_t guild_id;
    std::uint32_t pid;
    std::uint32_t offer;
    BYTE level;
    BYTE grade;
} TPacketGuildChangeMemberData;

static_assert(sizeof(TPacketGuildChangeMemberData) == 14, "2009 guild member update");

struct TPacketGuild
{
    std::uint32_t dwGuild;
    std::uint32_t dwInfo;
};
static_assert(sizeof(TPacketGuild) == 8, "2009 DB guild member record");

struct TPacketGDGuildAddMember
{
    std::uint32_t dwPID;
    std::uint32_t dwGuild;
    BYTE bGrade;
};
static_assert(sizeof(TPacketGDGuildAddMember) == 9, "2009 DB guild member record");

struct TPacketDGGuildMember
{
    std::uint32_t dwPID;
    std::uint32_t dwGuild;
    BYTE bGrade;
    BYTE isGeneral;
    BYTE bJob;
    BYTE bLevel;
    std::uint32_t dwOffer;
    char szName[25];
};
static_assert(sizeof(TPacketDGGuildMember) == 41, "2009 DB guild member record");


typedef struct SPacketGuildWar
{
    BYTE bType;
    BYTE bWar;
    std::uint32_t dwGuildFrom;
    std::uint32_t dwGuildTo;
    std::int32_t lWarPrice;
    std::int32_t lInitialScore;
} TPacketGuildWar;
static_assert(sizeof(TPacketGuildWar) == 18, "2009 DB guild war record");

struct TPacketGuildWarScore
{
    std::uint32_t dwGuildGainPoint;
    std::uint32_t dwGuildOpponent;
    std::int32_t lScore;
    std::int32_t lBetScore;
};
static_assert(sizeof(TPacketGuildWarScore) == 16, "2009 DB guild war score record");

struct TPacketGDGuildMoney {
    std::uint32_t dwGuild;
    std::int32_t lGold;
};
static_assert(sizeof(TPacketGDGuildMoney) == 8, "2009 DB guild treasury record");

struct TPacketDGGuildMoneyChange {
    std::uint32_t dwGuild;
    std::int32_t lTotalGold;
};
static_assert(sizeof(TPacketDGGuildMoneyChange) == 8, "2009 DB guild treasury record");

struct TPacketDGGuildMoneyWithdraw {
    std::uint32_t dwGuild;
    std::int32_t lChangeGold;
};
static_assert(sizeof(TPacketDGGuildMoneyWithdraw) == 8, "2009 DB guild treasury record");

struct TPacketGDGuildMoneyWithdrawGiveReply {
    std::uint32_t dwGuild;
    std::int32_t lChangeGold;
    BYTE bGiveSuccess;
};
static_assert(sizeof(TPacketGDGuildMoneyWithdrawGiveReply) == 9, "2009 DB guild treasury record");

struct TPacketGuildSkillUpdate
{
    std::uint32_t guild_id;
    std::int32_t amount;
    BYTE skill_levels[12];
    BYTE skill_point;
    BYTE save;
};
static_assert(sizeof(TPacketGuildSkillUpdate) == 22, "2009 DB guild record");

struct TPacketGuildExpUpdate
{
    std::uint32_t guild_id;
    std::int32_t amount;
};
static_assert(sizeof(TPacketGuildExpUpdate) == 8, "2009 DB guild record");

struct TPacketGuildLadderPoint
{
    std::uint32_t dwGuild;
    std::int32_t lChange;
};
static_assert(sizeof(TPacketGuildLadderPoint) == 8, "2009 DB guild record");

struct TPacketGuildUseSkill
{
    std::uint32_t dwGuild;
    std::uint32_t dwSkillVnum;
    std::uint32_t dwCooltime;
};
static_assert(sizeof(TPacketGuildUseSkill) == 12, "2009 DB guild record");

struct TPacketGuildSkillUsableChange
{
    std::uint32_t dwGuild;
    std::uint32_t dwSkillVnum;
    BYTE bUsable;
};
static_assert(sizeof(TPacketGuildSkillUsableChange) == 9, "2009 DB guild record");

#pragma pack(pop)

#pragma pack(push,1)
typedef struct SPacketMoneyLog {
    BYTE type;
    std::uint32_t vnum;
    std::int32_t gold;
} TPacketMoneyLog;
static_assert(sizeof(TPacketMoneyLog) == 9, "2009 money log record");
#pragma pack(pop)

// Original common/tables.h932..934; complete char-only record, no invented fields.
#include "length.h"
typedef struct SBanwordTable {
    char szWord[BANWORD_MAX_LEN+1];
} TBanwordTable;


// COMPLETE original field list from skill.cpp DWARF; original size1475.
#pragma pack(push,1)
typedef struct SSkillTable {
    std::uint32_t dwVnum;
    char szName[33];
    BYTE bType;
    BYTE bMaxLevel;
    std::uint32_t dwSplashRange;
    char szPointOn[64];
    char szPointPoly[101];
    char szSPCostPoly[101];
    char szDurationPoly[101];
    char szDurationSPCostPoly[101];
    char szCooldownPoly[101];
    char szMasterBonusPoly[101];
    char szGrandMasterAddSPCostPoly[101];
    std::uint32_t dwFlag;
    std::uint32_t dwAffectFlag;
    char szPointOn2[64];
    char szPointPoly2[101];
    char szDurationPoly2[101];
    std::uint32_t dwAffectFlag2;
    char szPointOn3[64];
    char szPointPoly3[101];
    char szDurationPoly3[101];
    BYTE bLevelStep;
    BYTE bLevelLimit;
    std::uint32_t preSkillVnum;
    BYTE preSkillLevel;
    std::int32_t lMaxHit;
    char szSplashAroundDamageAdjustPoly[101];
    BYTE bSkillAttrType;
    std::uint32_t dwTargetRange;
} TSkillTable;
static_assert(sizeof(TSkillTable) == 1475, "2009 boot skill record");
#pragma pack(pop)

// COMPLETE old packed item attribute row, common/tables.h metadata69 bytes.
#pragma pack(push,1)
typedef struct SItemAttrTable {
    char szApply[33];
    std::uint32_t dwApplyIndex;
    std::uint32_t dwProb;
    std::int32_t lValues[5];
    BYTE bMaxLevelBySet[8];
} TItemAttrTable;
static_assert(sizeof(TItemAttrTable) == 69, "2009 item attribute boot row");
#pragma pack(pop)


#pragma pack(push,1)
typedef struct SPacketGDRemoveAffect { std::uint32_t dwPID; std::uint32_t dwType; BYTE bApplyOn; } TPacketGDRemoveAffect;
static_assert(sizeof(TPacketGDRemoveAffect) == 9, "2009 affect wire size");
typedef struct SPacketGDAddAffect {
    std::uint32_t dwPID;
    TPacketAffectElement elem;
} TPacketGDAddAffect;
static_assert(sizeof(TPacketGDAddAffect) == 25, "2009 affect wire size");
#pragma pack(pop)


// Original common/tables.h678, full old11-byte record.
#pragma pack(push,1)
typedef struct SSafeboxTable {
    std::uint32_t dwID;
    BYTE bSize;
    std::uint32_t dwGold;
    WORD wItemCount;
} TSafeboxTable;
static_assert(sizeof(TSafeboxTable) == 11, "2009 safebox header");
#pragma pack(pop)

// Original common/tables.h566-571, questpc.cpp CU DIE0x1c8413.
// Original 106-byte quest load and save wire record.
#pragma pack(push,1)
typedef struct SQuestTable {
    std::uint32_t dwPID;
    char szName[33];
    char szState[65];
    std::int32_t lValue;
} TQuestTable;
static_assert(sizeof(TQuestTable) == 106, "2009 quest record");
#pragma pack(pop)

// Original common/tables.h1107-1110, questmanager.cpp CU SPacketSetEventFlag DIE0x2bcc270.
#pragma pack(push,1)
typedef struct SPacketSetEventFlag { char szFlagName[33]; std::int32_t lValue; } TPacketSetEventFlag;
static_assert(sizeof(TPacketSetEventFlag) == 37, "2009 event flag record");
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct SShopItemTable {
    std::uint32_t vnum;
    BYTE count;
    BYTE pos;
    std::uint32_t price;
    BYTE display_pos;
} TShopItemTable;
static_assert(sizeof(TShopItemTable) == 11, "2009 shop listing record");

typedef struct SShopTable {
    std::uint32_t dwVnum;
    std::uint32_t dwNPCVnum;
    BYTE byItemCount;
    TShopItemTable items[40];
} TShopTable;
static_assert(sizeof(TShopTable) == 449, "2009 boot shop record");
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct SPacketGDVCard {
    std::uint32_t dwID;
    char szSellCharacter[25];
    char szSellAccount[31];
    char szBuyCharacter[25];
    char szBuyAccount[31];
} TPacketGDVCard;
static_assert(sizeof(TPacketGDVCard) == 116, "2009 DB wire size");
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct tBlockException {
    BYTE cmd;
    char login[31];
} TPacketBlockException;
#pragma pack(pop)

// Original DB billing/auth packet layouts, 36 and 115 bytes on i386.
#pragma pack(push, 1)
typedef struct SPacketBillingExpire {
    char szLogin[31];
    BYTE bBillType;
    std::uint32_t dwRemainSeconds;
} TPacketBillingExpire;
static_assert(sizeof(TPacketBillingExpire) == 36, "2009 DB wire size");

typedef struct SPacketGDAuthLogin {
    std::uint32_t dwID;
    std::uint32_t dwLoginKey;
    char szLogin[31];
    char szSocialID[19];
    std::uint32_t adwClientKey[4];
    BYTE bBillType;
    std::uint32_t dwBillID;
    int iPremiumTimes[9];
} TPacketGDAuthLogin;
static_assert(sizeof(TPacketGDAuthLogin) == 0x73, "2009 GD auth login");
#pragma pack(pop)

// Original 104-byte admin record.
typedef struct TAdminInfo {
    int m_ID;
    char m_szAccount[32];
    char m_szName[32];
    char m_szContactIP[16];
    char m_szServerIP[16];
    int m_Authority;
} tAdminInfo;

#pragma pack(push, 1)
typedef struct SPlayerTable {
    std::uint32_t id;
    char name[25];
    char ip[16];
    WORD job;
    BYTE voice, level, level_step;
    short st, ht, dx, iq;
    std::uint32_t exp;
    std::int32_t gold;
    BYTE dir;
    std::int32_t x, y, z;
    std::int32_t lMapIndex, lExitX, lExitY, lExitMapIndex;
    short hp, sp, sRandomHP, sRandomSP;
    int playtime;
    short stat_point, skill_point, sub_skill_point, horse_skill_point;
    TPlayerSkill skills[255];
    TQuickslot quickslot[36];
    BYTE part_base;
    WORD parts[4];
    short stamina;
    BYTE skill_group;
    std::int32_t lAlignment;
    char szMobile[33];
    short stat_reset_count;
    THorseInfo horse;
    std::uint32_t logoff_interval;
    int aiPremiumTimes[9];
} TPlayerTable;
static_assert(sizeof(TPlayerTable) == 1818, "2009 DB player table");

typedef struct SPlayerCountPacket {
    std::uint32_t dwCount;
} TPlayerCountPacket;
static_assert(sizeof(TPlayerCountPacket) == 4, "2009 heartbeat player count");

typedef struct SPacketGDLoginByKey {
    char szLogin[31];
    std::uint32_t dwLoginKey;
    std::uint32_t adwClientKey[4];
    char szIP[16];
} TPacketGDLoginByKey;
static_assert(sizeof(TPacketGDLoginByKey) == 67, "2009 GD login by key");

typedef struct SPlayerLoadPacket {
    std::uint32_t account_id;
    std::uint32_t player_id;
    BYTE account_index;
} TPlayerLoadPacket;
static_assert(sizeof(TPlayerLoadPacket) == 9, "2009 GD player load");

typedef struct SPlayerCreatePacket {
    char login[31];
    char passwd[17];
    std::uint32_t account_id;
    BYTE account_index;
    TPlayerTable player_table;
} TPlayerCreatePacket;
static_assert(sizeof(TPlayerCreatePacket) == 1871, "2009 GD player create packet");

typedef struct SPlayerDeletePacket {
    char login[31];
    std::uint32_t player_id;
    BYTE account_index;
    char private_code[8];
} TPlayerDeletePacket;
static_assert(sizeof(TPlayerDeletePacket) == 44, "2009 packet width");

typedef struct SEmpireSelectPacket {
    std::uint32_t dwAccountID;
    BYTE bEmpire;
} TEmpireSelectPacket;
static_assert(sizeof(TEmpireSelectPacket) == 5, "2009 packet width");

typedef struct SPacketGDChangeName {
    std::uint32_t pid;
    char name[25];
} TPacketGDChangeName;
static_assert(sizeof(TPacketGDChangeName) == 29, "2009 packet width");
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct SRefineMaterial {
    std::uint32_t vnum;
    std::int32_t count;
} TRefineMaterial;

typedef struct SRefineTable {
    std::uint32_t id;
    BYTE material_count;
    std::int32_t cost;
    std::int32_t prob;
    TRefineMaterial materials[5];
} TRefineTable;
static_assert(sizeof(TRefineMaterial) == 8, "2009 refine material record");
static_assert(sizeof(TRefineTable) == 53, "2009 boot refine record");
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct SSafeboxLoadPacket {
    std::uint32_t dwID;
    char szLogin[31];
    char szPassword[7];
} TSafeboxLoadPacket;
static_assert(sizeof(TSafeboxLoadPacket) == 42, "2009 storage load request");
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    std::uint32_t dwPID1, dwPID2;
    std::int32_t lLovePoint;
    BYTE byMarried;
} TPacketMarriageUpdate;
static_assert(sizeof(TPacketMarriageUpdate) == 13, "2009 marriage update record");
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    std::uint32_t dwPID1;
    std::uint32_t dwPID2;
    std::uint32_t dwMapIndex;
} TPacketWeddingReady;
static_assert(sizeof(TPacketWeddingReady) == 12, "2009 wedding ready record");
#pragma pack(pop)


#pragma pack(push, 1)
struct TPacketDBHeader {
    BYTE bHeader;
    std::uint32_t dwHandle;
    std::uint32_t dwSize;
};
static_assert(sizeof(TPacketDBHeader) == 9, "2009 DB frame header");

typedef struct SPacketGDBoot {
    std::uint32_t dwItemIDRange[2];
    char szIP[16];
} TPacketGDBoot;

typedef struct SPacketGDSetup {
    char szPublicIP[16];
    BYTE bChannel;
    WORD wListenPort;
    WORD wP2PPort;
    BYTE abMaps[32];
    std::uint32_t dwLoginCount;
    BYTE bAuthServer;
} TPacketGDSetup;

typedef struct SPacketLoginOnSetup {
    std::uint32_t dwID;
    char szLogin[31];
    char szSocialID[19];
    char szHost[16];
    std::uint32_t dwLoginKey;
    std::uint32_t adwClientKey[4];
} TPacketLoginOnSetup;
#pragma pack(pop)

#pragma pack(push, 1)
struct TGuildWarReserve {
    std::uint32_t dwID, dwGuildFrom, dwGuildTo, dwTime;
    BYTE bType;
    std::int32_t lWarPrice, lInitialScore;
    BYTE bStarted;
    std::uint32_t dwBetFrom, dwBetTo;
    std::int32_t lPowerFrom, lPowerTo, lHandicap;
};

struct TPacketGDGuildWarBet {
    std::uint32_t dwWarID;
    char szLogin[31];
    std::uint32_t dwGold, dwGuild;
};
#pragma pack(pop)
static_assert(sizeof(TGuildWarReserve) == 46, "Guild war reserve packet layout");
static_assert(sizeof(TPacketGDGuildWarBet) == 43, "Guild war bet packet layout");

#pragma pack(push, 1)
typedef struct SPacketMyshopPricelistHeader
{
    std::uint32_t dwOwnerID;
    BYTE byCount;
} TPacketMyshopPricelistHeader;
static_assert(sizeof(TPacketMyshopPricelistHeader) == 5, "2009 MyShop price list header");
#pragma pack(pop)

static_assert(sizeof(TPacketGDBoot) == 24, "DB boot request size");
static_assert(sizeof(TPacketGDSetup) == 58, "DB setup size");
static_assert(sizeof(TPacketLoginOnSetup) == 90, "DB setup login size");

#endif
