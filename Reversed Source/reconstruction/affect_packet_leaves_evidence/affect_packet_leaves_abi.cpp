#include "../../game/src/char.h"
#include "../../game/src/desc.h"
#include "../../game/src/packet.h"
#include "../../common/tables.h"
#include <stddef.h>
extern void SendAffectRemovePacket(LPDESC,DWORD,DWORD,BYTE);
typedef char require_long32[sizeof(long)==4?1:-1];
typedef char gc_remove6[sizeof(TPacketGCAffectRemove)==6?1:-1];
typedef char gd_remove9[sizeof(TPacketGDRemoveAffect)==9?1:-1];
typedef char fly10[sizeof(TPacketGCCreateFly)==10?1:-1];
typedef char gc_type1[offsetof(TPacketGCAffectRemove,dwType)==1?1:-1];
typedef char gc_apply5[offsetof(TPacketGCAffectRemove,bApplyOn)==5?1:-1];
typedef char gd_pid0[offsetof(TPacketGDRemoveAffect,dwPID)==0?1:-1];
typedef char gd_type4[offsetof(TPacketGDRemoveAffect,dwType)==4?1:-1];
typedef char gd_apply8[offsetof(TPacketGDRemoveAffect,bApplyOn)==8?1:-1];
typedef char fly_type1[offsetof(TPacketGCCreateFly,bType)==1?1:-1];
typedef char fly_start2[offsetof(TPacketGCCreateFly,dwStartVID)==2?1:-1];
typedef char fly_end6[offsetof(TPacketGCCreateFly,dwEndVID)==6?1:-1];
void Gates(){bool(CHARACTER::*flag)(DWORD)=&CHARACTER::IsAffectFlag;void(CHARACTER::*fly)(BYTE,LPCHARACTER)=&CHARACTER::CreateFly;void(*remove)(LPDESC,DWORD,DWORD,BYTE)=&SendAffectRemovePacket;(void)flag;(void)fly;(void)remove;}
