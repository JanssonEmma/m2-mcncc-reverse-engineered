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
