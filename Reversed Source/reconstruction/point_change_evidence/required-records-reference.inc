// Existing packed record, for provenance/reference only; DO NOT duplicate it.
#pragma pack(push,1)
struct packet_point_change {
    int header;       // offset0,size4, assigned17
    DWORD dwVID;      // offset4,size4
    BYTE type;        // offset8,size1
    long amount;      // offset9,size4
    long value;       // offset13,size4
};
#pragma pack(pop)
// Existing e_overtime enum, size4: OT_NONE0,OT_3HOUR1,OT_5HOUR2.
