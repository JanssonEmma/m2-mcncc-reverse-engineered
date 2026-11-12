#include "../../game/src/locale_service.h"
#include "../../game/src/config.h"
#include "../../game/src/constants.h"
typedef char int4[sizeof(int)==4?1:-1];
typedef char pointer4[sizeof(void*)==4?1:-1];
int (*a)(const char*)=is_twobyte_euckr;
int (*b)(const char*)=is_twobyte_gb2312;
int (*c)(const char*)=is_twobyte_big5;
int (**d)(const char*)=&check_name;
int (**e)(const char*)=&is_twobyte;
void (*f)()=LocaleService_LoadLocaleStringFile;
const std::string& (*g)()=LocaleService_GetBasePath;
const std::string& (*h)()=LocaleService_GetMapPath;
bool (*i)(const std::string&)=LC_InitLocalization;
const int (*j)[31]=&aiPercentByDeltaLev_euckr;
const int (*k)[31]=&aiPercentByDeltaLevForBoss_euckr;
const int (*l)[31]=&aiPercentByDeltaLev_gb2312;
const int (*m)[31]=&aiPercentByDeltaLev_sjis;
const int (*n)[41]=&aiChainLightningCountBySkillLevel_euckr;
const int (*o)[41]=&aiChainLightningCountBySkillLevel_gb2312;
const int** p=&aiPercentByDeltaLev;
const int** q=&aiPercentByDeltaLevForBoss;
const int** r=&aiChainLightningCountBySkillLevel;
std::string* s=&g_stLocaleFilename;
