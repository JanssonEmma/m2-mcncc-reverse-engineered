
// Complete old packed packet records, target dead5/move24.
typedef struct packet_dead { BYTE header;DWORD vid; } TPacketGCDead;
typedef struct packet_move {
    BYTE bHeader,bFunc,bArg,bRot;
    DWORD dwVID;
    long lX,lY;
    DWORD dwTime,dwDuration;
} TPacketGCMove;
