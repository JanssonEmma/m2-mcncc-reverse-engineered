// Partial packet definitions recovered from desc.cpp DWARF.
#ifndef MCNCC_GAME_PACKET_H
#define MCNCC_GAME_PACKET_H
#include "../../common/tables.h"
#include "../../libthecore/include/typedef.h"
#pragma pack(push, 1)
typedef struct SPacketGGRelay {
    BYTE bHeader;
    char szName[25];
    long lSize;
} TPacketGGRelay;
typedef struct packet_handshake {
    BYTE bHeader;
    DWORD dwHandshake;
    DWORD dwTime;
    long lDelta;
} TPacketGCHandshake;
typedef struct command_handshake {
    BYTE bHeader;
    DWORD dwHandshake;
    DWORD dwTime;
    long lDelta;
} TPacketCGHandshake;
typedef struct packet_bindudp {
    BYTE header;
    DWORD addr;
    WORD port;
} TPacketGCBindUDP;
typedef struct packet_phase { BYTE header; BYTE phase; } TPacketGCPhase;
typedef struct packet_chat {
    BYTE header;
    WORD size;
    BYTE type;
    DWORD id;
    BYTE bEmpire;
} TPacketGCChat;
struct packet_item_ground_add {
    BYTE bHeader;
    long int x;
    long int y;
    long int z;
    DWORD dwVID;
    DWORD dwVnum;
};
typedef packet_item_ground_add TPacketGCItemGroundAdd;
struct packet_item_ground_del {
    BYTE bHeader;
    DWORD dwVID;
};
typedef packet_item_ground_del TPacketGCItemGroundDel;
struct packet_item_update {
    BYTE header;
    BYTE pos;
    BYTE count;
    long int alSockets[3];
    TPlayerItemAttribute aAttr[7];
};
typedef packet_item_update TPacketGCItemUpdate;
struct packet_item_set {
    BYTE header;
    BYTE pos;
    DWORD vnum;
    BYTE count;
    long int alSockets[3];
    TPlayerItemAttribute aAttr[7];
};
typedef packet_item_set TPacketGCItemSet;
struct packet_item_set2 {
    BYTE header;
    BYTE pos;
    DWORD vnum;
    BYTE count;
    DWORD flags;
    long int alSockets[3];
    TPlayerItemAttribute aAttr[7];
};
typedef packet_item_set2 TPacketGCItemSet2;
// Old30-byte ground ownership packet; name is24 characters plus terminator.
struct packet_item_ownership {
    BYTE bHeader;
    DWORD dwVID;
    char szName[25];
};
typedef packet_item_ownership TPacketGCItemOwnership;
typedef struct packet_update_char {
    BYTE header;
    DWORD dwVID;
    WORD awPart[4];
    BYTE bMovingSpeed,bAttackSpeed,bStateFlag;
    DWORD dwAffectFlag[2];
    DWORD dwGuildID;
    short sAlignment;
    BYTE bPKMode;
    DWORD dwMountVnum;
} TPacketGCCharacterUpdate;
#pragma pack(push,1)
struct packet_party_update {
    BYTE header;
    DWORD pid;
    BYTE role;
    BYTE percent_hp;
    short affects[7];
};
typedef packet_party_update TPacketGCPartyUpdate;
#pragma pack(pop)

// Insert into game/src/packet.h; complete old record and original typedef.
// The int header occupies FOUR bytes. The whole record is packed17 bytes.
#pragma pack(push, 1)
typedef struct packet_point_change {
    int header;
    DWORD dwVID;
    BYTE type;
    long amount;
    long value;
} TPacketGCPointChange;
#pragma pack(pop)

// Insert in game/src/packet.h. Old DWARF records, both packed6 bytes.
// Original anonymous walk-mode names/placement follow reference packet.h;
// old SETNZ byte instruction independently fixes run0/walk1.
enum { WALKMODE_RUN = 0, WALKMODE_WALK = 1 };
#pragma pack(push, 1)
typedef struct packet_target {
    BYTE header;
    DWORD dwVID;
    BYTE bHPPercent;
} TPacketGCTarget;
typedef struct SPacketGCWalkMode {
    BYTE header;
    DWORD vid;
    BYTE mode;
} TPacketGCWalkMode;
#pragma pack(pop)

#pragma pack(pop)
// Insert in game/src/packet.h; complete old packet header (4 packed bytes).
#pragma pack(push, 1)
typedef struct packet_guild {
    BYTE header;
    WORD size;
    BYTE subheader;
} TPacketGCGuild;
#pragma pack(pop)

#endif
