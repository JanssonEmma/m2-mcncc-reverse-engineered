#ifndef MCNCC_DB_PROTOCOL_H
#define MCNCC_DB_PROTOCOL_H

#include <cstdint>

// DB-side declarations pending integration with the shared game tables.
enum
{
    ITEM_AWARD_WHY_MAX_LEN = 50,
    MONEY_LOG_TYPE_MAX_NUM = 9,
    PREMIUM_MAX_NUM = 9,
    ITEM_SOCKET_MAX_NUM = 3,
    ITEM_ATTRIBUTE_MAX_NUM = 7,
    PART_MAIN = 0,
    PART_HAIR = 3,
    MAX_PRIV_NUM = 5,
    ITEM_QUEST = 18,
    ATTRIBUTE_SET_WEAPON = 0,
    ATTRIBUTE_SET_BODY,
    ATTRIBUTE_SET_WRIST,
    ATTRIBUTE_SET_FOOTS,
    ATTRIBUTE_SET_NECK,
    ATTRIBUTE_SET_HEAD,
    ATTRIBUTE_SET_SHIELD,
    ATTRIBUTE_SET_EAR,
    HEADER_DG_LOGIN_SUCCESS = 30,
    HEADER_DG_LOGIN_NOT_EXIST = 31,
    HEADER_DG_LOGIN_WRONG_PASSWD = 33,
    HEADER_DG_LOGIN_ALREADY = 34,
    HEADER_DG_PLAYER_CREATE_ALREADY = 38,
    HEADER_DG_PLAYER_CREATE_FAILED = 39,
    HEADER_DG_PARTY_CREATE = 59,
    HEADER_DG_PARTY_DELETE = 60,
    HEADER_DG_PARTY_ADD = 61,
    HEADER_DG_PARTY_REMOVE = 62,
    HEADER_DG_PARTY_STATE_CHANGE = 63,
    HEADER_DG_PARTY_SET_MEMBER_LEVEL = 65,
    HEADER_DG_SET_EVENT_FLAG = 105,
    HEADER_GD_SET_EVENT_FLAG = 55,
    HEADER_DG_CHANGE_NAME = 121,
    HEADER_DG_CHANGE_EMPIRE_PRIV = 124,
    HEADER_DG_CHANGE_GUILD_PRIV = 125,
    HEADER_DG_MONEY_LOG = 126,
    HEADER_DG_CHANGE_CHARACTER_PRIV = 127
};

enum
{
    GUILD_WAR_SEND_DECLARE = 1,
    GUILD_WAR_REFUSE = 2,
    GUILD_WAR_WAIT_START = 4,
    GUILD_WAR_CANCEL = 5,
    GUILD_WAR_ON_WAR = 6,
    GUILD_WAR_END = 7,
    GUILD_WAR_OVER = 8,
    GUILD_WAR_RESERVE = 9,
    HEADER_DG_GUILD_SKILL_UPDATE = 56,
    HEADER_DG_GUILD_EXP_UPDATE = 58,
    HEADER_DG_GUILD_ADD_MEMBER = 92,
    HEADER_DG_GUILD_REMOVE_MEMBER = 93,
    HEADER_DG_GUILD_CHANGE_GRADE = 94,
    HEADER_DG_GUILD_CHANGE_MEMBER_DATA = 95,
    HEADER_DG_GUILD_DISBAND = 96,
    HEADER_DG_GUILD_WAR = 97,
    HEADER_DG_GUILD_LOAD = 100,
    HEADER_DG_GUILD_SKILL_USABLE_CHANGE = 102
};

#pragma pack(push, 1)
typedef struct SItemPriceInfo
{
    std::uint32_t dwVnum;
    std::uint32_t dwPrice;
} TItemPriceInfo;
static_assert(sizeof(TItemPriceInfo) == 8, "2009 MyShop price entry");

typedef struct SItemPriceListTable
{
    std::uint32_t dwOwnerID;
    BYTE byCount;
    TItemPriceInfo aPriceInfo[SHOP_PRICELIST_MAX_NUM];
} TItemPriceListTable;

struct tLocale
{
    char szValue[32];
    char szKey[32];
};

struct TPacketDGLoginAlready
{
    char szLogin[LOGIN_MAX_LEN + 1];
};

struct TPacketDGChangeEmpirePriv
{
    BYTE type;
    std::int32_t value;
    BYTE empire;
    BYTE bLog;
    std::int32_t end_time_sec;
};
static_assert(sizeof(TPacketDGChangeEmpirePriv) == 11, "2009 privilege packet");

struct TPacketDGChangeGuildPriv
{
    BYTE type;
    std::int32_t value;
    std::uint32_t guild_id;
    BYTE bLog;
    std::int32_t end_time_sec;
};
static_assert(sizeof(TPacketDGChangeGuildPriv) == 14, "2009 privilege packet");

struct TPacketDGChangeCharacterPriv
{
    BYTE type;
    std::int32_t value;
    std::uint32_t pid;
    BYTE bLog;
};
static_assert(sizeof(TPacketDGChangeCharacterPriv) == 10, "2009 privilege packet");

typedef struct SPacketGDHighscore
{
    std::uint32_t dwPID;
    std::int32_t lValue;
    std::int8_t cDir;
    char szBoard[21];
} TPacketGDHighscore;
static_assert(sizeof(TPacketGDHighscore) == 30, "2009 highscore request");








struct TPacketGiveGuildPriv
{
    BYTE type;
    std::int32_t value;
    std::uint32_t guild_id;
    std::int32_t duration_sec;
};
static_assert(sizeof(TPacketGiveGuildPriv) == 13, "2009 privilege packet");

struct TPacketGiveEmpirePriv
{
    BYTE type;
    std::int32_t value;
    BYTE empire;
    std::int32_t duration_sec;
};
static_assert(sizeof(TPacketGiveEmpirePriv) == 10, "2009 privilege packet");

struct TPacketGiveCharacterPriv
{
    BYTE type;
    std::int32_t value;
    std::uint32_t pid;
};
static_assert(sizeof(TPacketGiveCharacterPriv) == 9, "2009 privilege packet");

struct TPacketMarriageAdd
{
    std::uint32_t dwPID1;
    std::uint32_t dwPID2;
    std::int32_t tMarryTime;
    char szName1[CHARACTER_NAME_MAX_LEN + 1];
    char szName2[CHARACTER_NAME_MAX_LEN + 1];
};
static_assert(sizeof(TPacketMarriageAdd) == 62, "2009 marriage or wedding record");

struct TPacketMarriageRemove
{
    std::uint32_t dwPID1;
    std::uint32_t dwPID2;
};
static_assert(sizeof(TPacketMarriageRemove) == 8, "2009 marriage or wedding record");

struct TPacketWeddingRequest
{
    std::uint32_t dwPID1;
    std::uint32_t dwPID2;
};
static_assert(sizeof(TPacketWeddingRequest) == 8, "2009 marriage or wedding record");

struct TPacketWeddingEnd
{
    std::uint32_t dwPID1;
    std::uint32_t dwPID2;
};
static_assert(sizeof(TPacketWeddingEnd) == 8, "2009 marriage or wedding record");
struct TPacketGDSMS
{
    char szFrom[25];
    char szTo[25];
    char szMobile[33];
    char szMsg[81];
};

struct TPacketBlockChat
{
    char szName[25];
    std::int32_t lDuration;
};
static_assert(sizeof(TPacketBlockChat) == 29, "2009 DB block chat record");

struct TSafeboxChangeSizePacket
{
    std::uint32_t dwID;
    BYTE bSize;
};
static_assert(sizeof(TSafeboxChangeSizePacket) == 5, "2009 storage request");

struct TSafeboxChangePasswordPacket
{
    std::uint32_t dwID;
    char szOldPassword[7];
    char szNewPassword[7];
};
static_assert(sizeof(TSafeboxChangePasswordPacket) == 18, "2009 storage request");

struct TPacketReloadAdmin
{
    char szIP[16];
};
#pragma pack(pop)

struct TSafeboxChangeSizePacket;
struct TSafeboxChangePasswordPacket;
struct TPacketGDSMS;
struct TPacketGiveEmpirePriv;
struct TPacketGiveGuildPriv;
struct TPacketGiveCharacterPriv;
struct TPacketMarriageAdd;
struct TPacketMarriageRemove;
struct TPacketWeddingRequest;
struct TPacketWeddingEnd;
struct TPacketBlockChat;
struct TPacketReloadAdmin;

#endif
