#include "../../game/src/char.h"
#include "../../game/src/guild.h"
#include "../../game/src/packet.h"
#include <cstddef>
typedef DWORD(CHARACTER::*HorseVnumSignature)();
typedef void(CHARACTER::*HorseInfoSignature)();
typedef void(CHARACTER::*RiderSignature)(LPCHARACTER);
typedef void(CHARACTER::*RotationSignature)(float);
typedef void(CHARACTER::*EmpireSignature)(BYTE);
typedef LPCHARACTER(CHARACTER::*VictimSignature)();
typedef void(CHARACTER::*MoveSignature)(BYTE,BYTE,DWORD,DWORD,DWORD,DWORD,int);
typedef DWORD(CGuild::*MasterSignature)();
HorseVnumSignature exact_horse_vnum=&CHARACTER::GetMyHorseVnum;
HorseInfoSignature exact_send_info=&CHARACTER::SendHorseInfo;
HorseInfoSignature exact_clear_info=&CHARACTER::ClearHorseInfo;
RiderSignature exact_rider=&CHARACTER::SetRider;
RotationSignature exact_rotation=&CHARACTER::SetRotation;
EmpireSignature exact_empire=&CHARACTER::SetEmpire;
VictimSignature exact_victim=&CHARACTER::GetVictim;
MoveSignature exact_move=&CHARACTER::SendMovePacket;
MasterSignature exact_master=&CGuild::GetMasterPID;
typedef char TargetInt32[sizeof(int)==4?1:-1];
typedef char TargetFloat32[sizeof(float)==4?1:-1];
typedef char TargetDWORD32[sizeof(DWORD)==4?1:-1];
typedef char DeadBytes5[sizeof(TPacketGCDead)==5?1:-1];
typedef char DeadVidAt1[offsetof(TPacketGCDead,vid)==1?1:-1];
typedef char MoveBytes24[sizeof(TPacketGCMove)==24?1:-1];
typedef char MoveVidAt4[offsetof(TPacketGCMove,dwVID)==4?1:-1];
typedef char MoveXAt8[offsetof(TPacketGCMove,lX)==8?1:-1];
typedef char MoveYAt12[offsetof(TPacketGCMove,lY)==12?1:-1];
typedef char MoveTimeAt16[offsetof(TPacketGCMove,dwTime)==16?1:-1];
typedef char MoveDurationAt20[offsetof(TPacketGCMove,dwDuration)==20?1:-1];
// Full data records are retained; modern STL-containing class sizes are not fixed here.
