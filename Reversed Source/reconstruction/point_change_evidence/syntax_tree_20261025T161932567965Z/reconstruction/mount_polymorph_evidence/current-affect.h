// Complete old24-byte affect data; newer pooled allocation APIs are absent in this target.
#ifndef MCNCC_GAME_AFFECT_H
#define MCNCC_GAME_AFFECT_H
#include "../../libthecore/include/typedef.h"
class CAffect {
public:
    DWORD dwType;
    BYTE bApplyOn;
    long int lApplyValue;
    DWORD dwFlag;
    long int lDuration;
    long int lSPCost;
};
#endif
