// Add to original game/src/locale_service.h.
int is_twobyte_euckr(const char*);
int is_twobyte_gb2312(const char*);
int is_twobyte_big5(const char*);
void LocaleService_LoadLocaleStringFile();
const std::string& LocaleService_GetBasePath();
const std::string& LocaleService_GetMapPath();
bool LC_InitLocalization(const std::string& szLocal);
