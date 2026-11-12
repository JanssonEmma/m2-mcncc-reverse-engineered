// Partial packet definitions recovered from desc.cpp DWARF.
#ifndef MCNCC_GAME_PACKET_H
#define MCNCC_GAME_PACKET_H
#include "../../common/tables.h"
#include "../../common/length.h"
#include <cstdint>
#include "../../libthecore/include/typedef.h"
#pragma pack(push, 1)
struct TNPCPosition {
    BYTE bType;
    char name[CHARACTER_NAME_MAX_LEN + 1];
    std::int32_t x, y;
};

typedef struct SPacketGCNPCPosition {
    BYTE header;
    WORD size, count;
} TPacketGCNPCPosition;
static_assert(sizeof(TNPCPosition) == 34, "NPC position wire entry");
static_assert(sizeof(TPacketGCNPCPosition) == 5, "NPC position wire header");

struct packet_item_use {
    BYTE header;
    BYTE pos;
    std::uint32_t ch_vid;
    std::uint32_t victim_vid;
    std::uint32_t vnum;
};
static_assert(sizeof(packet_item_use) == 14);

struct packet_motion {
    BYTE header;
    std::uint32_t vid;
    std::uint32_t victim_vid;
    std::uint16_t motion;
};
static_assert(sizeof(packet_motion) == 11);

struct TPacketGCRefineInformation {
    BYTE header;
    BYTE type;
    BYTE pos;
    std::uint32_t src_vnum;
    std::uint32_t result_vnum;
    BYTE material_count;
    std::int32_t cost;
    std::int32_t prob;
    struct {
        std::uint32_t vnum;
        std::int32_t count;
    } materials[5];
};
static_assert(sizeof(TPacketGCRefineInformation) == 60);

typedef struct SPacketGGRelay {
    BYTE bHeader;
    char szName[25];
    std::int32_t lSize;
} TPacketGGRelay;
static_assert(sizeof(TPacketGGRelay) == 30, "2009 peer relay header");
typedef struct packet_handshake {
    BYTE bHeader;
    std::uint32_t dwHandshake;
    std::uint32_t dwTime;
    std::int32_t lDelta;
} TPacketGCHandshake;
static_assert(sizeof(TPacketGCHandshake) == 13, "2009 handshake wire size");
typedef struct SPacketGCChangeName {
    BYTE header;
    std::uint32_t pid;
    char name[25];
} TPacketGCChangeName;
static_assert(sizeof(TPacketGCChangeName) == 30, "2009 client rename packet");
typedef struct command_player_create_success {
    BYTE header;
    BYTE bAccountCharacterIndex;
    TSimplePlayer player;
} TPacketGCPlayerCreateSuccess;
static_assert(sizeof(TPacketGCPlayerCreateSuccess) == 65, "2009 client create success packet");
typedef struct packet_create_failure {
    BYTE header;
    BYTE bType;
} TPacketGCCreateFailure;
typedef struct packet_main_character {
    BYTE header;
    std::uint32_t dwVID;
    WORD wRaceNum;
    char szName[25];
    std::int32_t lx, ly, lz;
    BYTE empire, skill_group;
} TPacketGCMainCharacter;
typedef struct packet_main_character3_bgm {
    BYTE header;
    std::uint32_t dwVID;
    WORD wRaceNum;
    char szChrName[25];
    char szBGMName[25];
    std::int32_t lx, ly, lz;
    BYTE empire, skill_group;
} TPacketGCMainCharacter3_BGM;
typedef struct packet_main_character4_bgm_vol {
    BYTE header;
    std::uint32_t dwVID;
    WORD wRaceNum;
    char szChrName[25];
    char szBGMName[25];
    float fBGMVol;
    std::int32_t lx, ly, lz;
    BYTE empire, skill_group;
} TPacketGCMainCharacter4_BGM_VOL;
typedef struct packet_points {
    BYTE header;
    std::int32_t points[POINT_MAX_NUM];
} TPacketGCPoints;
typedef struct packet_skill_level {
    BYTE bHeader;
    TPlayerSkill skills[SKILL_MAX_NUM];
} TPacketGCSkillLevel;
typedef struct packet_login_success {
    BYTE bHeader;
    TSimplePlayer players[4];
    std::uint32_t guild_id[4];
    char guild_name[4][13];
    std::uint32_t handle;
    std::uint32_t random_key;
} TPacketGCLoginSuccess;
static_assert(sizeof(TPacketGCLoginSuccess) == 329, "2009 client login success packet");
typedef struct command_handshake {
    BYTE bHeader;
    std::uint32_t dwHandshake;
    std::uint32_t dwTime;
    std::int32_t lDelta;
} TPacketCGHandshake;
static_assert(sizeof(TPacketCGHandshake) == 13, "2009 handshake wire size");
typedef struct packet_bindudp {
    BYTE header;
    std::uint32_t addr;
    WORD port;
} TPacketGCBindUDP;
static_assert(sizeof(TPacketGCBindUDP) == 7, "2009 handshake wire size");
typedef struct packet_phase { BYTE header; BYTE phase; } TPacketGCPhase;
typedef struct packet_chat {
    BYTE header;
    WORD size;
    BYTE type;
    std::uint32_t id;
    BYTE bEmpire;
} TPacketGCChat;
static_assert(sizeof(TPacketGCChat) == 9, "2009 chat header");
struct packet_item_ground_add {
    BYTE bHeader;
    std::int32_t x;
    std::int32_t y;
    std::int32_t z;
    std::uint32_t dwVID;
    std::uint32_t dwVnum;
};
typedef packet_item_ground_add TPacketGCItemGroundAdd;
static_assert(sizeof(TPacketGCItemGroundAdd) == 21, "2009 ground item packet");
struct packet_item_ground_del {
    BYTE bHeader;
    std::uint32_t dwVID;
};
typedef packet_item_ground_del TPacketGCItemGroundDel;
static_assert(sizeof(TPacketGCItemGroundDel) == 5, "2009 ground item packet");
struct packet_item_update {
    BYTE header;
    BYTE pos;
    BYTE count;
    std::int32_t alSockets[3];
    TPlayerItemAttribute aAttr[7];
};
typedef packet_item_update TPacketGCItemUpdate;
static_assert(sizeof(TPacketGCItemUpdate) == 36, "2009 item packet");
struct packet_item_set {
    BYTE header;
    BYTE pos;
    std::uint32_t vnum;
    BYTE count;
    std::int32_t alSockets[3];
    TPlayerItemAttribute aAttr[7];
};
typedef packet_item_set TPacketGCItemSet;
static_assert(sizeof(TPacketGCItemSet) == 40, "2009 item packet");
struct packet_item_set2 {
    BYTE header;
    BYTE pos;
    std::uint32_t vnum;
    BYTE count;
    std::uint32_t flags;
    std::int32_t alSockets[3];
    TPlayerItemAttribute aAttr[7];
};
typedef packet_item_set2 TPacketGCItemSet2;
static_assert(sizeof(TPacketGCItemSet2) == 44, "2009 item packet");
// Old30-byte ground ownership packet; name is24 characters plus terminator.
struct packet_item_ownership {
    BYTE bHeader;
    std::uint32_t dwVID;
    char szName[25];
};
typedef packet_item_ownership TPacketGCItemOwnership;
static_assert(sizeof(TPacketGCItemOwnership) == 30, "2009 ground item packet");
typedef struct packet_update_char {
    BYTE header;
    std::uint32_t dwVID;
    WORD awPart[4];
    BYTE bMovingSpeed,bAttackSpeed,bStateFlag;
    std::uint32_t dwAffectFlag[2];
    std::uint32_t dwGuildID;
    std::int16_t sAlignment;
    BYTE bPKMode;
    std::uint32_t dwMountVnum;
} TPacketGCCharacterUpdate;
static_assert(sizeof(TPacketGCCharacterUpdate) == 35, "2009 character update packet");
#pragma pack(push,1)
struct packet_party_update {
    BYTE header;
    std::uint32_t pid;
    BYTE role;
    BYTE percent_hp;
    short affects[7];
};
typedef packet_party_update TPacketGCPartyUpdate;
static_assert(sizeof(TPacketGCPartyUpdate) == 21, "2009 party update packet");

// Complete old packed packet records, target dead5/move24.
typedef struct packet_dead { BYTE header; std::uint32_t vid; } TPacketGCDead;
static_assert(sizeof(TPacketGCDead) == 5, "death packet wire size");
struct TPacketGCCharacterAdd {
    BYTE header;
    std::uint32_t dwVID;
    float angle;
    std::int32_t x, y, z;
    BYTE bType;
    WORD wRaceNum;
    BYTE bMovingSpeed, bAttackSpeed, bStateFlag;
    std::uint32_t dwAffectFlag[2];
};

struct TPacketGCCharacterAdditionalInfo {
    BYTE header;
    std::uint32_t dwVID;
    char name[25];
    WORD awPart[4];
    BYTE bEmpire;
    std::uint32_t dwGuildID, dwMarkCRC;
    std::int16_t sAlignment;
    BYTE bPKMode;
    std::uint32_t dwMountVnum;
};

struct TPacketGCCharacterDelete {
    BYTE header;
    std::uint32_t id;
};

static_assert(sizeof(TPacketGCCharacterAdd) == 35, "character insert packet size");
static_assert(sizeof(TPacketGCCharacterAdditionalInfo) == 54, "character additional packet size");
static_assert(sizeof(TPacketGCCharacterDelete) == 5, "character remove packet size");

typedef struct packet_shop_sign { BYTE bHeader; std::uint32_t dwVID; char szSign[33]; } TPacketGCShopSign;
static_assert(sizeof(TPacketGCShopSign) == 38, "shop sign packet wire size");
typedef struct packet_stun { BYTE header; std::uint32_t vid; } TPacketGCStun;
typedef struct packet_fishing { BYTE header; BYTE subheader; std::uint32_t info; BYTE dir; } TPacketGCFishing;
static_assert(sizeof(TPacketGCFishing) == 7, "fishing packet wire size");
typedef struct packet_dig_motion
{
    BYTE header;
    std::uint32_t vid;
    std::uint32_t target_vid;
    BYTE count;
} TPacketGCDigMotion;
static_assert(sizeof(TPacketGCDigMotion) == 10, "Dig motion packet wire size");
static_assert(sizeof(TPacketGCStun) == 5, "stun packet wire size");
typedef struct packet_move {
    BYTE bHeader,bFunc,bArg,bRot;
    std::uint32_t dwVID;
    std::int32_t lX,lY;
    std::uint32_t dwTime,dwDuration;
} TPacketGCMove;
static_assert(sizeof(TPacketGCMove) == 24, "2009 movement packet");

#pragma pack(pop)

// Insert into game/src/packet.h; complete old record and original typedef.
// The int header occupies FOUR bytes. The whole record is packed17 bytes.
#pragma pack(push, 1)
typedef struct packet_point_change {
    std::int32_t header;
    std::uint32_t dwVID;
    BYTE type;
    std::int32_t amount;
    std::int32_t value;
} TPacketGCPointChange;
static_assert(sizeof(TPacketGCPointChange) == 17, "2009 combat packet");

#pragma pack(pop)

// Insert in game/src/packet.h. Old DWARF records, both packed6 bytes.
// Original anonymous walk-mode names/placement follow reference packet.h;
// old SETNZ byte instruction independently fixes run0/walk1.
enum { WALKMODE_RUN = 0, WALKMODE_WALK = 1 };
#pragma pack(push, 1)
typedef struct packet_target {
    BYTE header;
    std::uint32_t dwVID;
    BYTE bHPPercent;
} TPacketGCTarget;
static_assert(sizeof(TPacketGCTarget) == 6, "2009 target health packet");

#pragma pack(push,1)
typedef struct {
    BYTE bHeader;
    std::int32_t lID;
    char szName[33];
    std::uint32_t dwVID;
    BYTE bType;
} TPacketGCTargetCreate;
typedef struct {
    BYTE bHeader;
    std::int32_t lID, lX, lY;
} TPacketGCTargetUpdate;
typedef struct {
    BYTE bHeader;
    std::int32_t lID;
} TPacketGCTargetDelete;
static_assert(sizeof(TPacketGCTargetCreate) == 43, "2009 quest target create packet");
static_assert(sizeof(TPacketGCTargetUpdate) == 13, "2009 quest target update packet");
static_assert(sizeof(TPacketGCTargetDelete) == 5, "2009 quest target delete packet");
#pragma pack(pop)
typedef struct SPacketGCWalkMode {
    BYTE header;
    std::uint32_t vid;
    BYTE mode;
} TPacketGCWalkMode;
static_assert(sizeof(TPacketGCWalkMode) == 6, "walk mode packet size");

#pragma pack(pop)


#pragma pack(pop)
// Insert in game/src/packet.h; complete old packet header (4 packed bytes).
#pragma pack(push, 1)
typedef struct packet_guild {
    BYTE header;
    WORD size;
    BYTE subheader;
} TPacketGCGuild;
static_assert(sizeof(TPacketGCGuild) == 4, "2009 guild packet header");

#pragma pack(pop)

#pragma pack(push,1)
struct packet_quickslot_add { BYTE header; BYTE pos; TQuickslot slot; };
struct packet_quickslot_del { BYTE header; BYTE pos; };

#pragma pack(pop)


#pragma pack(push,1)
typedef struct packet_fly { BYTE bHeader; BYTE bType; std::uint32_t dwStartVID; std::uint32_t dwEndVID; } TPacketGCCreateFly;
static_assert(sizeof(TPacketGCCreateFly) == 10, "2009 fly packet");
struct TPacketGCFlyTargeting {
    BYTE bHeader;
    std::uint32_t dwShooterVID, dwTargetVID;
    std::int32_t x, y;
};
static_assert(sizeof(TPacketGCFlyTargeting) == 17, "fly target wire size");
typedef struct { BYTE bHeader; std::uint32_t dwType; BYTE bApplyOn; } TPacketGCAffectRemove;
static_assert(sizeof(TPacketGCAffectRemove) == 6, "2009 affect wire size");

typedef struct packet_damage_info {
    BYTE header;
    std::uint32_t dwVID;
    BYTE flag;
    std::int32_t damage;
} TPacketGCDamageInfo;
static_assert(sizeof(TPacketGCDamageInfo) == 10, "2009 combat packet");
typedef struct packet_affect_add {
    BYTE bHeader;
    TPacketAffectElement elem;
} TPacketGCAffectAdd;
static_assert(sizeof(TPacketGCAffectAdd) == 22, "2009 affect wire size");

typedef struct packet_sync_position {
    BYTE bHeader;
    WORD wSize;
} TPacketGCSyncPosition;

typedef struct packet_ownership {
    BYTE bHeader;
    std::uint32_t dwOwnerVID;
    std::uint32_t dwVictimVID;
} TPacketGCOwnership;
static_assert(sizeof(TPacketGCOwnership) == 9, "ownership packet wire size");

typedef struct command_sync_position_element {
    std::uint32_t dwVID;
    std::int32_t lX;
    std::int32_t lY;
} TPacketCGSyncPositionElement;
static_assert(sizeof(TPacketGCSyncPosition) == 3, "2009 sync header");
static_assert(sizeof(TPacketCGSyncPositionElement) == 12, "2009 sync element");
#pragma pack(pop)


// Complete original6-byte record. DWARF offsets0/1/3/5, old WORDindex notmodernDWORD.
#pragma pack(push,1)
struct packet_quest_info { BYTE header; WORD size; WORD index; BYTE flag; };
#pragma pack(pop)


// Original packet.h1279, complete6byte record offsets0/1/3/4.
#pragma pack(push,1)
struct packet_script { BYTE header; WORD size; BYTE skin; WORD src_size; };
#pragma pack(pop)


// Original packet.h812, complete74-byte packet (long/DWORD target32).
#pragma pack(push,1)
typedef struct packet_quest_confirm { BYTE header; char msg[65]; std::int32_t timeout; std::uint32_t requestPID; } TPacketGCQuestConfirm;
static_assert(sizeof(TPacketGCQuestConfirm) == 74, "2009 quest confirm packet");
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct packet_shop {
    BYTE header;
    WORD size;
    BYTE subheader;
} TPacketGCShop;

typedef struct packet_shop_item {
    std::uint32_t vnum;
    std::uint32_t price;
    BYTE count;
    std::int32_t alSockets[3];
    TPlayerItemAttribute aAttr[7];
} TPacketGCShopItem;

typedef struct packet_shop_start {
    std::uint32_t owner_vid;
    TPacketGCShopItem items[40];
} TPacketGCShopStart;

typedef struct packet_shop_update_item {
    BYTE pos;
    TPacketGCShopItem item;
} TPacketGCShopUpdateItem;

static_assert(sizeof(TPacketGCShop) == 4, "2009 shop packet header");
static_assert(sizeof(TPacketGCShopItem) == 42, "2009 shop item");
static_assert(sizeof(TPacketGCShopStart) == 1684, "2009 shop start payload");
static_assert(sizeof(TPacketGCShopUpdateItem) == 43, "2009 shop update payload");

typedef struct packet_pvp {
    BYTE bHeader;
    std::uint32_t dwVIDSrc;
    std::uint32_t dwVIDDst;
    BYTE bMode;
} TPacketGCPVP;
static_assert(sizeof(TPacketGCPVP) == 10, "2009 PvP packet");

typedef struct packet_whisper {
    BYTE bHeader;
    WORD wSize;
    BYTE bType;
    char szNameFrom[25];
} TPacketGCWhisper;

typedef struct packet_party_add {
    BYTE header;
    std::uint32_t pid;
    char name[25];
} TPacketGCPartyAdd;

typedef struct packet_party_link {
    BYTE header;
    std::uint32_t pid;
    std::uint32_t vid;
} TPacketGCPartyLink;

typedef struct packet_party_remove {
    BYTE header;
    std::uint32_t pid;
} TPacketGCPartyRemove;

typedef struct packet_party_parameter {
    BYTE bHeader;
    BYTE bDistributeMode;
} TPacketGCPartyParameter;
static_assert(sizeof(TPacketGCPartyAdd) == 30, "2009 party add packet");
static_assert(sizeof(TPacketGCPartyLink) == 9, "2009 party link and unlink packet");
static_assert(sizeof(TPacketGCPartyRemove) == 5, "2009 party remove packet");
static_assert(sizeof(TPacketGCPartyParameter) == 2, "2009 party parameter packet");

typedef struct packet_warp {
    BYTE bHeader;
    std::int32_t lX, lY;
    std::uint32_t lAddr;
    WORD wPort;
} TPacketGCWarp;
static_assert(sizeof(TPacketGCWarp) == 15, "2009 client warp packet");

typedef struct packet_exchange {
    BYTE header;
    BYTE sub_header;
    bool is_me;
    std::uint32_t arg1, arg2, arg3;
    std::int32_t alSockets[3];
    TPlayerItemAttribute aAttr[7];
} TPacketGCExchange;
static_assert(sizeof(TPacketGCExchange) == 48, "2009 exchange packet");

typedef struct SPacketGGLogin {
    BYTE bHeader;
    char szName[25];
    std::uint32_t dwPID;
    BYTE bEmpire;
    BYTE bMapIndex;
    BYTE bChannel;
} TPacketGGLogin;
static_assert(sizeof(TPacketGGLogin) == 33, "2009 peer login packet");

typedef struct SPacketGGLogout {
    BYTE bHeader;
    char szName[25];
} TPacketGGLogout;

typedef struct SPacketGGSetup {
    BYTE bHeader;
    WORD wPort;
    BYTE bChannel;
} TPacketGGSetup;

typedef struct packet_symbol_data {
    BYTE header;
    WORD size;
    std::uint32_t guild_id;
} TPacketGCGuildSymbolData;
static_assert(sizeof(TPacketGCGuildSymbolData) == 7, "2009 guild symbol packet header");
#pragma pack(pop)

// Original messenger packet records: GC header (4), list entry (2).
#pragma pack(push, 1)
typedef struct packet_messenger {
    BYTE header;
    WORD size;
    BYTE subheader;
} TPacketGCMessenger;
typedef struct packet_messenger_list_offline {
    BYTE connected;
    BYTE length;
} TPacketGCMessengerListOffline;
typedef struct packet_messenger_list_online {
    BYTE connected;
    BYTE length;
} TPacketGCMessengerListOnline;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct packet_gg_login_ping {
    BYTE bHeader;
    char szLogin[31];
} TPacketGGLoginPing;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct packet_matrix_card {
    BYTE bHeader;
    std::uint32_t dwRows;
    std::uint32_t dwCols;
} TPacketGCMatrixCard;
static_assert(sizeof(TPacketGCMatrixCard) == 9, "2009 matrix challenge packet");

typedef struct packet_runup_matrix_quiz {
    BYTE bHeader;
    char szQuiz[9];
} TPacketGCRunupMatrixQuiz;

struct TPacketCGLogin3 {
    BYTE header;
    char login[31];
    char passwd[17];
    std::uint32_t adwClientKey[4];
};
static_assert(sizeof(TPacketCGLogin3) == 65, "old client auth login");

typedef struct packet_login_failure {
    BYTE header;
    char szStatus[9];
} TPacketGCLoginFailure;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct packet_safebox_size {
    BYTE bHeader;
    BYTE bSize;
} TPacketCGSafeboxSize;

typedef struct packet_dungeon {
    BYTE bHeader;
    WORD size;
    BYTE subheader;
} TPacketGCDungeon;

typedef struct packet_dungeon_dest_position {
    std::int32_t x, y;
} TPacketGCDungeonDestPosition;
static_assert(sizeof(TPacketGCDungeon) == 4);
static_assert(sizeof(TPacketGCDungeonDestPosition) == 8);
#pragma pack(pop)

#endif
