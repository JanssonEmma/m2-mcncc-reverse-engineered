//081282a0/0812dde0: actual packed move encoder and view dispatch.
#include "char.h"
#include "packet.h"
#include "../../libthecore/include/utils.h"
#include "horse-summon-detail.h"
void EncodeMovePacket(TPacketGCMove& packet,DWORD vid,BYTE function,BYTE argument,
                      DWORD x,DWORD y,DWORD duration,DWORD time,BYTE rotation) {
    packet.bHeader=3;packet.bFunc=function;packet.bArg=argument;packet.dwVID=vid;
    if(!time) time=get_dword_time();
    packet.dwTime=time;packet.bRot=rotation;
    packet.lX=horse_summon_detail::Signed(static_cast<unsigned int>(x));
    packet.lY=horse_summon_detail::Signed(static_cast<unsigned int>(y));
    packet.dwDuration=duration;
}
void CHARACTER::SendMovePacket(BYTE function,BYTE argument,DWORD x,DWORD y,
                                DWORD duration,DWORD time,int rotation) {
    if(!function) { x=m_posDest.x;y=m_posDest.y;duration=m_dwMoveDuration; }
    if(rotation==-1) rotation=horse_summon_detail::Trunc32(m_pointsInstant.fRot)/5;
    TPacketGCMove packet;
    EncodeMovePacket(packet,m_vid,function,argument,x,y,duration,time,static_cast<BYTE>(rotation));
    PacketView(&packet,24,this);
}
