// Partial old constant table declarations: target symbols/data with reference-guided int spelling.
#ifndef MCNCC_GAME_CONSTANTS_H
#define MCNCC_GAME_CONSTANTS_H
enum { ITEM_ACCESSORY_SOCKET_MAX_NUM = 3 };
extern const int aiAccessorySocketAddPct[ITEM_ACCESSORY_SOCKET_MAX_NUM];
extern const int aiAccessorySocketEffectivePct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
extern const int aiAccessorySocketDegradeTime[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
extern const int aiAccessorySocketPutPct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
// Insert into game/src/constants.h; retain existing accessory declarations.
#include "../../common/length.h"
#include "../../libthecore/include/typedef.h"
typedef struct SJobInitialPoints {
    int st;
    int ht;
    int dx;
    int iq;
    int max_hp;
    int max_sp;
    int hp_per_ht;
    int sp_per_iq;
    int hp_per_lv_begin;
    int hp_per_lv_end;
    int sp_per_lv_begin;
    int sp_per_lv_end;
    int max_stamina;
    int stamina_per_con;
    int stamina_per_lv_begin;
    int stamina_per_lv_end;
} TJobInitialPoints;
typedef struct SApplyInfo { BYTE bPointType; } TApplyInfo;
extern TJobInitialPoints JobInitialPoints[JOB_MAX_NUM];
extern const TApplyInfo aApplyInfo[MAX_APPLY_NUM];

// .rodata symbol0821b540,164 bytes; reference-guided const int spelling.
extern const int aiPolymorphPowerByLevel[41];

#endif
