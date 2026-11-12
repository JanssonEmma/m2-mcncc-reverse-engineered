#include "../../game/src/locale_service.h"
#include "../../game/src/config.h"
#include "../../game/src/constants.h"
typedef char lt_int4[sizeof(int)==4?1:-1];
typedef char lt_ptr4[sizeof(void*)==4?1:-1];
typedef char lt_eight[JOB_MAX_NUM*2==8?1:-1];
void(*transfer_signature)()=LocaleService_TransferDefaultSetting;
bool(CTableBySkill::*real_check_signature)()=&CTableBySkill::Check;
int(**name_callback)(const char*)=&check_name;
int(**two_byte_callback)(const char*)=&is_twobyte;
const DWORD** mutable_exp_pointer=&exp_table;
const int** mutable_boss_pointer=&aiPercentByDeltaLevForBoss;
const int** mutable_normal_pointer=&aiPercentByDeltaLev;
const int** mutable_chain_pointer=&aiChainLightningCountBySkillLevel;
