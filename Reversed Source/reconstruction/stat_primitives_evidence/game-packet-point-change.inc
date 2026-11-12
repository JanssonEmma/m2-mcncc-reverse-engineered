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
