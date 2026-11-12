#include <cstddef>
#include "../../game/src/affect.h"
#include "../../common/tables.h"
#define SIZE(T,N) typedef char size_##T[(sizeof(T)==N)?1:-1]
#define OFFSET(T,M,N) typedef char off_##T##_##M[(offsetof(T,M)==N)?1:-1]
SIZE(CAffect,24);
OFFSET(CAffect,dwType,0);
OFFSET(CAffect,bApplyOn,4);
OFFSET(CAffect,lApplyValue,8);
OFFSET(CAffect,dwFlag,12);
OFFSET(CAffect,lDuration,16);
OFFSET(CAffect,lSPCost,20);
SIZE(TPacketAffectElement,21);
OFFSET(TPacketAffectElement,dwType,0);
OFFSET(TPacketAffectElement,bApplyOn,4);
OFFSET(TPacketAffectElement,lApplyValue,5);
OFFSET(TPacketAffectElement,dwFlag,9);
OFFSET(TPacketAffectElement,lDuration,13);
OFFSET(TPacketAffectElement,lSPCost,17);
