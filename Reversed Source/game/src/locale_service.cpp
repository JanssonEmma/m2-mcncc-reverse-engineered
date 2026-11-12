#include "empire_text_convert.h"
#include <cstdlib>
#include "constants.h"
#include "banword.h"
#include "mob_manager.h"
#include "utils.h"
#include <cstring>
#include <cctype>
#include "config.h"
#include "locale.hpp"
#include "../../libthecore/include/log.h"
#include <cstdio>
#include "../../libthecore/include/typedef.h"
// Partial old locale service: target enum and predicates only.
#include "locale_service.h"
eLocalization g_eLocalType=LC_NOSET;
eLocalization LC_GetLocalType() { return g_eLocalType; }
bool LC_IsLocale(eLocalization type) { return LC_GetLocalType()==type; }
bool LC_IsYMIR() { return LC_GetLocalType()==LC_YMIR; }
bool LC_IsLetsGame() { return LC_GetLocalType()==LC_LETSGAME; }
bool LC_IsCIBN() { return LC_GetLocalType()==LC_CIBN; }
bool LC_IsJapan() { return LC_GetLocalType()==LC_JAPAN; }
bool LC_IsEnglish() { return LC_GetLocalType()==LC_ENGLISH; }
bool LC_IsHongKong() { return LC_GetLocalType()==LC_HONGKONG; }
bool LC_IsNewCIBN() { return LC_GetLocalType()==LC_NEWCIBN; }
bool LC_IsGermany() { return LC_GetLocalType()==LC_GERMANY; }
bool LC_IsKorea() { return LC_GetLocalType()==LC_KOREA; }
bool LC_IsCanada() { return LC_GetLocalType()==LC_CANADA; }
bool LC_IsBrazil() { return LC_GetLocalType()==LC_BRAZIL; }
bool LC_IsEurope()
{
    const eLocalization type=LC_GetLocalType();
    return type==LC_FRANCE || type==LC_ITALY || type==LC_TURKEY || type==LC_POLAND ||
           type==LC_UK || type==LC_SPAIN || type==LC_PORTUGAL;
}

// Locale static initializer 08068b40 uses this original encoding.
std::string g_stLocale="euckr";

// Original mutable byte at0823b488; reference storage location retained.
BYTE PK_PROTECT_LEVEL=30;

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

// Complete original locale_service.cpp1017, 080686c0.
bool LC_InitLocalization(const std::string& szLocal)
{
    g_stLocal=szLocal;
    if(g_stLocal=="ymir") g_eLocalType=LC_YMIR;
    else if(g_stLocal=="letsgame") g_eLocalType=LC_LETSGAME;
    else if(g_stLocal=="cibn") g_eLocalType=LC_CIBN;
    else if(g_stLocal=="japan") g_eLocalType=LC_JAPAN;
    else if(g_stLocal=="english") g_eLocalType=LC_ENGLISH;
    else if(g_stLocal=="hongkong") g_eLocalType=LC_HONGKONG;
    else if(g_stLocal=="newcibn") g_eLocalType=LC_NEWCIBN;
    else if(g_stLocal=="germany") g_eLocalType=LC_GERMANY;
    else if(g_stLocal=="korea") g_eLocalType=LC_KOREA;
    else if(g_stLocal=="france") g_eLocalType=LC_FRANCE;
    else if(g_stLocal=="italy") g_eLocalType=LC_ITALY;
    else if(g_stLocal=="spain") g_eLocalType=LC_SPAIN;
    else if(g_stLocal=="uk") g_eLocalType=LC_UK;
    else if(g_stLocal=="turkey") g_eLocalType=LC_TURKEY;
    else if(g_stLocal=="poland") g_eLocalType=LC_POLAND;
    else if(g_stLocal=="portugal") g_eLocalType=LC_PORTUGAL;
    else if(g_stLocal=="canada") g_eLocalType=LC_CANADA;
    else if(g_stLocal=="brazil") g_eLocalType=LC_BRAZIL;
    else return false;
    return true;
}

// Append to original locale_service.cpp after full banword/str_lower/CMob dependencies.
// Includes: banword.h, mob_manager.h, utils.h, <cstring>, <cctype>.
int check_name_independent(const char* str)
{
    if(CBanwordManager::instance().CheckString(str)) return 0;
    char szTmp[257];szTmp[256]=0;
    str_lower(str,szTmp,256);
    return CMobManager::instance().Get(szTmp,false)==0;
}
int check_name_gb2312(const char* str)
{
    static const BYTE exceptions[5][2]={{0xd7,0xfa},{0xd7,0xfb},{0xd7,0xfc},{0xd7,0xfd},{0xd7,0xfe}};
    if(!str || !*str) return 0;
    for(int i=0;str[i];) {
        const unsigned char b1=static_cast<unsigned char>(str[i]);
        if(b1&0x80U) {
            if(!str[i+1]) return 0;
            const unsigned char b2=static_cast<unsigned char>(str[i+1]);i+=2;
            if(b1<0xb0U || b1>0xf7U || b2<0xa1U || b2>0xfeU) return 0;
            for(int j=0;j<5;++j) if(b1==exceptions[j][0] && b2==exceptions[j][1]) return 0;
        } else {
            if(!std::isdigit(static_cast<signed char>(b1)) && !std::isalpha(static_cast<signed char>(b1))) return 0;
            ++i;
        }
    }
    return check_name_independent(str);
}
int check_name_big5(const char* str)
{
    if(!str || !*str) return 0;
    for(int i=0;str[i];) {
        const unsigned int b1=static_cast<unsigned char>(str[i]);
        if(b1&0x80U) {
            if(!str[i+1]) return 0;
            const unsigned int b2=static_cast<unsigned char>(str[i+1]),code=(b1<<8)|b2;
            if(!((code>=0xa440U && code<=0xc67eU)||(code>=0xc940U && code<=0xf9d5U))) {
                if(test_server) sys_log(0,"check_name_big5[%d][%s] %x %x %x",i,str,b1,b2,code);
                return 0;
            }
            i+=2;
        } else {
            if(!std::isdigit(static_cast<signed char>(b1)) && !std::isalpha(static_cast<signed char>(b1))) return 0;
            ++i;
        }
    }
    return check_name_independent(str);
}
int check_name_euckr(const char* str)
{
    if(!str || !*str || std::strlen(str)<2) return 0;
    for(const char* tmp=str;*tmp;++tmp) {
        const unsigned char byte=static_cast<unsigned char>(*tmp);
        const int c=static_cast<signed char>(byte);
        const bool han=(byte&0xe0U)>0x90U;
        if(!han && std::isspace(c)) return 0;
        if(!han && std::isdigit(c)) continue;
        if(!han && std::isalpha(c)) continue;
        const int code=c+256;
        if(code<176 || code>200) return 0;
        ++tmp;if(!*tmp) break;
    }
    return check_name_independent(str);
}
int check_name_latin1(const char* str)
{
    if(!str || !*str || std::strlen(str)<2) return 0;
    for(const char* tmp=str;*tmp;++tmp) {
        const unsigned char byte=static_cast<unsigned char>(*tmp);
        const int c=static_cast<signed char>(byte);
        const bool han=(byte&0xe0U)>0x90U;
        if(!han && std::isspace(c)) return 0;
        if(!han && std::isdigit(c)) continue;
        if(!han && std::isalpha(c)) continue;
        if(byte==145 || byte==146 || byte==196 || byte==214 || byte==220 || byte==223 ||
           byte==228 || byte==246 || byte==252) continue;
        const int code=c+256;
        if(code<176 || code>200) return 0;
        ++tmp;if(!*tmp) break;
    }
    return check_name_independent(str);
}
int check_name_alphabet(const char* str)
{
    if(!str || !*str || std::strlen(str)<2) return 0;
    for(const char* tmp=str;*tmp;++tmp)
        if(!std::isdigit(static_cast<signed char>(*tmp)) && !std::isalpha(static_cast<signed char>(*tmp))) return 0;
    return check_name_independent(str);
}
bool sjis_is_disable_name_char(const char* src)
{
    // Original local mutable pointer .data0823b48c -> readonly814000 at081f7e0f.
    static const char* sjis_symbols="\x81\x40";
    return std::strncmp(src,sjis_symbols,2)==0;
}
int is_char_sjis(const char* p,const char* e)
{
    const unsigned char first=static_cast<unsigned char>(*p);
    if(((first>=0x81U && first<=0x9fU)||(first>=0xe0U && first<=0xfcU)) &&
       static_cast<int>(e-p)>1) {
        const unsigned char second=static_cast<unsigned char>(p[1]);
        if((second>=0x40U && second<=0x7eU)||(second>=0x80U && second<=0xfcU)) return 1;
    }
    return 0;
}
int check_name_sjis(const char* str)
{
    const char* p=str;const char* e=str+std::strlen(str);
    while(*p) {
        if(is_char_sjis(p,e)) {
            if(sjis_is_disable_name_char(p) ||
               (static_cast<unsigned char>(p[0])==0x81U && p[1]=='@')) return 0;
            p+=2;
        } else {
            if(!std::isalpha(static_cast<signed char>(*p)) && !std::isdigit(static_cast<signed char>(*p))) return 0;
            ++p;
        }
    }
    return check_name_independent(str);
}

// Complete old locale_service.cpp970, original08068580.
// Includes constants.h, config.h, <cstdio>, <cstdlib>; name helpers already declared.
void LocaleService_TransferDefaultSetting()
{
    if(!check_name) check_name=check_name_euckr;
    if(!is_twobyte) is_twobyte=is_twobyte_euckr;
    if(!exp_table) exp_table=exp_table_euckr;
    for(int job=0;job<JOB_MAX_NUM*2;++job) {
        if(!CTableBySkill::instance().Check()) {
            std::fprintf(stderr,"[NO SETTING SKILL] aiSkillPowerByLevelFromType[%d]",job);
            std::exit(1);
        }
    }
    if(!aiPercentByDeltaLevForBoss) aiPercentByDeltaLevForBoss=aiPercentByDeltaLevForBoss_euckr;
    if(!aiPercentByDeltaLev) aiPercentByDeltaLev=aiPercentByDeltaLev_euckr;
    if(!aiChainLightningCountBySkillLevel) aiChainLightningCountBySkillLevel=aiChainLightningCountBySkillLevel_euckr;
}

// Original locale_service.cpp550; ELF STB_LOCAL, preserve file-static linkage.
static void __LocaleService_Init_DEFAULT()
{
    g_stLocaleFilename="";
    g_stServiceBasePath="locale/"+g_stServiceName;
    g_stServiceMapPath=g_stServiceBasePath+"/map";
    g_stQuestDir=g_stServiceBasePath+"/quest";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert(g_stQuestDir+"/object");
}
// Exact original locale_service.cpp1012 const-reference API,080686b0.
const std::string& LocaleService_GetQuestPath()
{
    return g_stQuestDir;
}

// Original 08066210; full static locale_service.cpp body.
static void __LocaleService_Init_CIBN()
{
    check_name = check_name_gb2312;
    is_twobyte = is_twobyte_gb2312;
    exp_table = exp_table_gb2312;
    aiPercentByDeltaLev = aiPercentByDeltaLev_gb2312;
    aiPercentByDeltaLevForBoss = aiPercentByDeltaLev_gb2312;
    aiChainLightningCountBySkillLevel = aiChainLightningCountBySkillLevel_gb2312;
    g_iUseLocale = 1;
    PK_PROTECT_LEVEL = 10;
    g_stLocale="gb2312";
    g_stServiceBasePath="locale/cibn";
    g_stServiceMapPath="locale/cibn/map";
    g_stLocaleFilename="locale/cibn/gb2312_string.txt";
    g_stQuestDir="locale/cibn/quest";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/cibn/quest/object");
}

// Original 080663e0; full static locale_service.cpp body.
static void __LocaleService_Init_CIBN_OLD()
{
    __LocaleService_Init_CIBN();
    g_stServiceBasePath=".";
    g_stServiceMapPath="data/map";
    g_stLocaleFilename="gb2312_string.txt";
    g_stQuestDir="quest";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("quest/object");
}

// Original 08066780; full static locale_service.cpp body.
static void __LocaleService_Init_JAPAN()
{
    g_stLocale="sjis";
    g_stServiceBasePath="locale/japan";
    g_stQuestDir="locale/japan/quest";
    g_stServiceMapPath="locale/japan/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/japan/quest/object");
    g_stLocaleFilename="locale/japan/sjis_string.txt";
    g_iUseLocale = 1;
    check_name = check_name_sjis;
    exp_table = exp_table_sjis;
    aiPercentByDeltaLev = aiPercentByDeltaLev_sjis;
}

// Original 08066930; full static locale_service.cpp body.
static void __LocaleService_Init_English()
{
    g_stLocale="";
    g_stServiceBasePath="locale/english";
    g_stQuestDir="locale/english/quest";
    g_stServiceMapPath="locale/english/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/english/quest/object");
    g_stLocaleFilename="locale/english/eng_string.txt";
    g_iUseLocale = 1;
}

// Original 08066ac0; full static locale_service.cpp body.
static void __LocaleService_Init_HongKong()
{
    g_stLocale="big5";
    g_stServiceBasePath="locale/hongkong";
    g_stQuestDir="locale/hongkong/quest";
    g_stServiceMapPath="locale/hongkong/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/hongkong/quest/object");
    g_stLocaleFilename="locale/hongkong/big5_string.txt";
    check_name = check_name_big5;
    g_iUseLocale = 1;
    is_twobyte = is_twobyte_big5;
    exp_table = exp_table_hk;
}

// Original 08066c70; full static locale_service.cpp body.
static void __LocaleService_Init_NewCIBN()
{
    g_stLocale="gb2312";
    g_stServiceBasePath="locale/newcibn";
    g_stQuestDir="locale/newcibn/quest";
    g_stServiceMapPath="locale/newcibn/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/newcibn/quest/object");
    g_stLocaleFilename="locale/newcibn/gb2312_string.txt";
    g_iUseLocale = 1;
    check_name = check_name_gb2312;
    is_twobyte = is_twobyte_gb2312;
    exp_table = exp_table_newcibn;
}

// Original 08066e20; full static locale_service.cpp body.
static void __LocaleService_Init_Germany()
{
    g_stLocale="latin1";
    g_stServiceBasePath="locale/germany";
    g_stQuestDir="locale/germany/quest";
    g_stServiceMapPath="locale/germany/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/germany/quest/object");
    g_stLocaleFilename="locale/germany/locale_string.txt";
    PK_PROTECT_LEVEL = 0xf;
    g_iUseLocale = 1;
    check_name = check_name_latin1;
}

// Original 08066fc0; full static locale_service.cpp body.
static void __LocaleService_Init_Korea()
{
    g_stLocale="euckr";
    g_stServiceBasePath="locale/korea";
    g_stQuestDir="locale/korea/quest";
    g_stServiceMapPath="locale/korea/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/korea/quest/object");
    g_iUseLocale = 1;
}

// Original 08067120; full static locale_service.cpp body.
static void __LocaleService_Init_France()
{
    g_stLocale="latin1";
    g_stServiceBasePath="locale/france";
    g_stQuestDir="locale/france/quest";
    g_stServiceMapPath="locale/france/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/france/quest/object");
    g_stLocaleFilename="locale/france/locale_string.txt";
    g_iUseLocale = 1;
    check_name = check_name_latin1;
    PK_PROTECT_LEVEL = 0xf;
}

// Original 080672d0; full static locale_service.cpp body.
static void __LocaleService_Init_Italy()
{
    g_stLocale="latin1";
    g_stServiceBasePath="locale/italy";
    g_stQuestDir="locale/italy/quest";
    g_stServiceMapPath="locale/italy/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/italy/quest/object");
    g_stLocaleFilename="locale/italy/locale_string.txt";
    PK_PROTECT_LEVEL = 0xf;
    g_iUseLocale = 1;
    check_name = check_name_latin1;
}

// Original 08067470; full static locale_service.cpp body.
static void __LocaleService_Init_spain()
{
    g_stLocale="latin1";
    g_stServiceBasePath="locale/spain";
    g_stQuestDir="locale/spain/quest";
    g_stServiceMapPath="locale/spain/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/spain/quest/object");
    g_stLocaleFilename="locale/spain/locale_string.txt";
    g_iUseLocale = 1;
    check_name = check_name_latin1;
    PK_PROTECT_LEVEL = 0xf;
}

// Original 08067620; full static locale_service.cpp body.
static void __LocaleService_Init_UK()
{
    g_stLocale="latin1";
    g_stServiceBasePath="locale/uk";
    g_stQuestDir="locale/uk/quest";
    g_stServiceMapPath="locale/uk/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/uk/quest/object");
    g_stLocaleFilename="locale/uk/locale_string.txt";
    PK_PROTECT_LEVEL = 0xf;
    g_iUseLocale = 1;
    check_name = check_name_latin1;
}

// Original 080677c0; full static locale_service.cpp body.
static void __LocaleService_Init_Turkey()
{
    g_stLocale="latin5";
    g_stServiceBasePath="locale/turkey";
    g_stQuestDir="locale/turkey/quest";
    g_stServiceMapPath="locale/turkey/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/turkey/quest/object");
    g_stLocaleFilename="locale/turkey/locale_string.txt";
    g_iUseLocale = 1;
    check_name = check_name_latin1;
    PK_PROTECT_LEVEL = 0xf;
}

// Original 08067970; full static locale_service.cpp body.
static void __LocaleService_Init_Poland()
{
    g_stLocale="latin2";
    g_stServiceBasePath="locale/poland";
    g_stQuestDir="locale/poland/quest";
    g_stServiceMapPath="locale/poland/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/poland/quest/object");
    g_stLocaleFilename="locale/poland/locale_string.txt";
    PK_PROTECT_LEVEL = 0xf;
    g_iUseLocale = 1;
    check_name = check_name_latin1;
}

// Original 08067b10; full static locale_service.cpp body.
static void __LocaleService_Init_Portugal()
{
    g_stLocale="latin1";
    g_stServiceBasePath="locale/portugal";
    g_stQuestDir="locale/portugal/quest";
    g_stServiceMapPath="locale/portugal/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/portugal/quest/object");
    g_stLocaleFilename="locale/portugal/locale_string.txt";
    g_iUseLocale = 1;
    check_name = check_name_latin1;
    PK_PROTECT_LEVEL = 0xf;
}

// Original 08067cc0; full static locale_service.cpp body.
static void __LocaleService_Init_Canada()
{
    g_stLocale="latin1";
    g_stServiceBasePath="locale/canada";
    g_stQuestDir="locale/canada/quest";
    g_stServiceMapPath="locale/canada/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/canada/quest/object");
    g_stLocaleFilename="locale/canada/locale_string.txt";
    check_name = check_name_alphabet;
    g_iUseLocale = 1;
}

// Original 08067e50; full static locale_service.cpp body.
static void __LocaleService_Init_Brazil()
{
    g_stLocale="latin1";
    g_stServiceBasePath="locale/brazil";
    g_stQuestDir="locale/brazil/quest";
    g_stServiceMapPath="locale/brazil/map";
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert("locale/brazil/quest/object");
    g_stLocaleFilename="locale/brazil/locale_string.txt";
    check_name = check_name_alphabet;
    g_iUseLocale = 1;
}

// Original08067ff0 locale_service.cpp809, file-static const-reference API.
// Target PLAYER_PER_ACCOUNT is4. The emitted allowlist returns for these16;
// every other string prints the original diagnostic and exits0.
static void __CheckPlayerSlot(const std::string& service_name)
{
    if(service_name=="ymir" || service_name=="korea" || service_name=="japan" ||
       service_name=="newcibn" || service_name=="brazil" || service_name=="hongkong" ||
       service_name=="canada" || service_name=="english" || service_name=="germany" ||
       service_name=="italy" || service_name=="poland" || service_name=="portugal" ||
       service_name=="turkey" || service_name=="uk" || service_name=="france" || service_name=="spain")
        return;
    std::printf("<ERROR> PLAYER_PER_ACCOUNT = %d\n",PLAYER_PER_ACCOUNT);
    std::exit(0);
}

// Original080681a0 locale_service.cpp853. No LC_InitLocalization call in old body.
bool LocaleService_Init(const std::string& c_rstServiceName)
{
    if(!g_stServiceName.empty()) {
        _sys_err("LocaleService_Init",856,"ALREADY exist service");
        return false;
    }
    g_stServiceName=c_rstServiceName;
    if(g_stServiceName=="cibn") {std::puts("Init cinb");__LocaleService_Init_CIBN();}
    else if(g_stServiceName=="cibn_old") {std::puts("Init cibn_old");__LocaleService_Init_CIBN_OLD();}
    else if(g_stServiceName=="japan") {std::puts("Init japan");__LocaleService_Init_JAPAN();}
    else if(g_stServiceName=="english") {std::puts("Init english");__LocaleService_Init_English();}
    else if(g_stServiceName=="hongkong") {std::puts("Init Hongkong");__LocaleService_Init_HongKong();}
    else if(g_stServiceName=="newcibn") {std::puts("Init NewCIBN");__LocaleService_Init_NewCIBN();}
    else if(g_stServiceName=="germany") {std::puts("Init getmany");__LocaleService_Init_Germany();}
    else if(g_stServiceName=="korea") {std::puts("Init korea");__LocaleService_Init_Korea();}
    else if(g_stServiceName=="france") {std::puts("Init france");__LocaleService_Init_France();}
    else if(g_stServiceName=="italy") {std::puts("Init italy");__LocaleService_Init_Italy();}
    else if(g_stServiceName=="spain") {std::puts("Init spain");__LocaleService_Init_spain();}
    else if(g_stServiceName=="uk") {std::puts("Init uk");__LocaleService_Init_UK();}
    else if(g_stServiceName=="turkey") {std::puts("Init turkey");__LocaleService_Init_Turkey();}
    else if(g_stServiceName=="poland") {std::puts("Init poland");__LocaleService_Init_Poland();}
    else if(g_stServiceName=="portugal") {std::puts("Init portugal");__LocaleService_Init_Portugal();}
    else if(g_stServiceName=="canada") {std::printf("Init canada");__LocaleService_Init_Canada();}
    else if(g_stServiceName=="brazil") {std::printf("Init brazil");__LocaleService_Init_Brazil();}
    else {std::puts("Init else");__LocaleService_Init_DEFAULT();}
    std::printf("Setting Locale =\"%s\"\n",g_stServiceName.c_str());
    std::puts(g_stServiceBasePath.c_str());
    __CheckPlayerSlot(g_stServiceName);
    return true;
}

void LocaleService_LoadEmpireTextConvertTables()
{
    for (DWORD empire=1;empire<=3;++empire) {
        char filename[256];
        std::snprintf(filename,255,"%s/lang%d.cvt",LocaleService_GetBasePath().c_str(),static_cast<int>(empire));
        sys_log(0,"Load %s",filename);
        LoadEmpireTextConvertTable(empire,filename);
    }
}
