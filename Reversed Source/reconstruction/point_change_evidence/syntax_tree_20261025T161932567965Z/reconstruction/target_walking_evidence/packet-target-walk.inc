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
