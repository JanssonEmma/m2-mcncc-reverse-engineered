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
