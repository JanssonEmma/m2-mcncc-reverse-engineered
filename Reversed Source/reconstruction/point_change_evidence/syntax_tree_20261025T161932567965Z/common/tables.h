// Partial old account tables, recovered from desc.cpp DWARF.
#ifndef MCNCC_COMMON_TABLES_H
#define MCNCC_COMMON_TABLES_H
#include "../libthecore/include/typedef.h"
#pragma pack(push, 1)
typedef struct SSimplePlayer {
    DWORD dwID;
    char szName[25];
    BYTE byJob;
    BYTE byLevel;
    DWORD dwPlayMinutes;
    BYTE byST;
    BYTE byHT;
    BYTE byDX;
    BYTE byIQ;
    WORD wMainPart;
    BYTE bChangeName;
    WORD wHairPart;
    BYTE bDummy[4];
    long x;
    long y;
    long lAddr;
    WORD wPort;
    BYTE skill_group;
} TSimplePlayer;
typedef struct SAccountTable {
    DWORD id;
    char login[31];
    char passwd[17];
    char social_id[19];
    char status[9];
    BYTE bEmpire;
    TSimplePlayer players[4];
} TAccountTable;
#pragma pack(pop)

// Recovered old mob, horse and character support records.
// Old packed mob records, recovered from char.cpp DWARF.
#ifndef MCNCC_COMMON_MOB_TABLES_FRAGMENT
#define MCNCC_COMMON_MOB_TABLES_FRAGMENT
#pragma pack(push, 1)
typedef struct SMobSkillLevel {
    DWORD dwVnum;
    BYTE bLevel;
} TMobSkillLevel;
typedef struct SMobTable {
    DWORD dwVnum;
    char szName[25];
    char szLocaleName[25];
    BYTE bType;
    BYTE bRank;
    BYTE bBattleType;
    BYTE bLevel;
    BYTE bSize;
    DWORD dwGoldMin;
    DWORD dwGoldMax;
    DWORD dwExp;
    DWORD dwMaxHP;
    BYTE bRegenCycle;
    BYTE bRegenPercent;
    WORD wDef;
    DWORD dwAIFlag;
    DWORD dwRaceFlag;
    DWORD dwImmuneFlag;
    BYTE bStr;
    BYTE bDex;
    BYTE bCon;
    BYTE bInt;
    DWORD dwDamageRange[2];
    short int sAttackSpeed;
    short int sMovingSpeed;
    BYTE bAggresiveHPPct;
    WORD wAggressiveSight;
    WORD wAttackRange;
    char cEnchants[6];
    char cResists[11];
    DWORD dwResurrectionVnum;
    DWORD dwDropItemVnum;
    BYTE bMountCapacity;
    BYTE bOnClickType;
    BYTE bEmpire;
    char szFolder[65];
    float fDamMultiply;
    DWORD dwSummonVnum;
    DWORD dwDrainSP;
    DWORD dwMobColor;
    DWORD dwPolymorphItemVnum;
    TMobSkillLevel Skills[5];
    BYTE bBerserkPoint;
    BYTE bStoneSkinPoint;
    BYTE bGodSpeedPoint;
    BYTE bDeathBlowPoint;
    BYTE bRevivePoint;
} TMobTable;
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
    DWORD dwHorseHealthDropTime;
};
#pragma pack(pop)


typedef struct SLogoutPacket { char login[31]; char passwd[17]; } TLogoutPacket;

// Old item records: item.cpp DWARF, packed layout.
#pragma pack(push, 1)
struct TPlayerItemAttribute {
    BYTE bType;
    short int sValue;
};
struct SItemLimit {
    BYTE bType;
    long int lValue;
};
typedef SItemLimit TItemLimit;
struct SItemApply {
    BYTE bType;
    long int lValue;
};
typedef SItemApply TItemApply;
struct SItemTable {
    DWORD dwVnum;
    char szName[25];
    char szLocaleName[25];
    BYTE bType;
    BYTE bSubType;
    BYTE bWeight;
    BYTE bSize;
    DWORD dwAntiFlags;
    DWORD dwFlags;
    DWORD dwWearFlags;
    DWORD dwImmuneFlag;
    DWORD dwGold;
    DWORD dwShopBuyPrice;
    TItemLimit aLimits[2];
    TItemApply aApplies[3];
    long int alValues[6];
    long int alSockets[3];
    DWORD dwRefinedVnum;
    WORD wRefineSet;
    BYTE bAlterToMagicItemPct;
    BYTE bSpecular;
    BYTE bGainSocketPct;
    short int sAddonType;
};
typedef SItemTable TItemTable;
struct SPlayerItem {
    DWORD id;
    BYTE window;
    WORD pos;
    DWORD count;
    DWORD vnum;
    long int alSockets[3];
    TPlayerItemAttribute aAttr[7];
    DWORD owner;
};
typedef SPlayerItem TPlayerItem;
struct TPacketAffectElement {
    DWORD dwType;
    BYTE bApplyOn;
    long int lApplyValue;
    DWORD dwFlag;
    long int lDuration;
    long int lSPCost;
};
// Old DB party-member-level payload (party.cpp DWARF; common reference placement).
struct SPacketPartySetMemberLevel { DWORD dwLeaderPID; DWORD dwPID; BYTE bLevel; };
typedef SPacketPartySetMemberLevel TPacketPartySetMemberLevel;
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
    DWORD guild_id;
    DWORD pid;
    DWORD offer;
    BYTE level;
    BYTE grade;
} TPacketGuildChangeMemberData;
#pragma pack(pop)

#endif
