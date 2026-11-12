// Insert in game/src/packet.h; complete old packet header (4 packed bytes).
#pragma pack(push, 1)
typedef struct packet_guild {
    BYTE header;
    WORD size;
    BYTE subheader;
} TPacketGCGuild;
#pragma pack(pop)
