// Append in original game/src/locale_service.cpp. Existing g_stLocale,
// g_eLocalType and PK_PROTECT_LEVEL definitions remain single definitions.
// Required includes: config.h, locale.hpp, ../../libthecore/include/log.h, <cstdio>.
std::string g_stServiceName;
std::string g_stServiceBasePath=".";
std::string g_stServiceMapPath="data/map";
std::string g_stLocaleFilename;
std::string g_stLocal="";
int (*check_name)(const char*)=0;
int (*is_twobyte)(const char*)=0;

int is_twobyte_euckr(const char* str)
{
    // Original requires nonnull input and inspects only the first byte.
    return (static_cast<unsigned char>(*str)&0xe0U)>0x90U;
}
int is_twobyte_gb2312(const char* str)
{
    if(!str || !*str) return 0;
    const unsigned char b1=static_cast<unsigned char>(str[0]);
    const unsigned char b2=static_cast<unsigned char>(str[1]);
    return b1>=0xb0U && b1<=0xf7U && b2>=0xa1U && b2<=0xfeU;
}
int is_twobyte_big5(const char* str)
{
    if(!str || !*str) return 0;
    const unsigned int b1=static_cast<unsigned char>(str[0]);
    const unsigned int b2=static_cast<unsigned char>(str[1]);
    if(!(b1&0x80U)) return 0;
    const unsigned int code=(b1<<8)|b2;
    if((code>=0xa440U && code<=0xc67eU) || (code>=0xc940U && code<=0xf9d5U)) return 1;
    if(test_server) sys_log(0,"twobyte_big5 %x %x",b1,b2);
    return 0;
}
void LocaleService_LoadLocaleStringFile()
{
    if(!g_stLocaleFilename.empty() && !g_bAuthServer) {
        std::fprintf(stderr,"LocaleService %s\n",g_stLocaleFilename.c_str());
        locale_init(g_stLocaleFilename.c_str());
    }
}
const std::string& LocaleService_GetBasePath() { return g_stServiceBasePath; }
const std::string& LocaleService_GetMapPath() { return g_stServiceMapPath; }
