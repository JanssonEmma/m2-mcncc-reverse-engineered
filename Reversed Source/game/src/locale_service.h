// Partial old locale declarations recovered from target DWARF.
#ifndef MCNCC_GAME_LOCALE_SERVICE_H
#define MCNCC_GAME_LOCALE_SERVICE_H
#include <clocale>
#include <string>
extern std::string g_stLocale;
// Host CRTs can define LC_MAX as a macro; preserve the recovered enum spelling.
#ifdef LC_MAX
#undef LC_MAX
#endif
enum eLocalization {
    LC_NOSET = 0,
    LC_YMIR = 1,
    LC_LETSGAME = 2,
    LC_CIBN = 3,
    LC_JAPAN = 4,
    LC_ENGLISH = 5,
    LC_HONGKONG = 6,
    LC_NEWCIBN = 7,
    LC_GERMANY = 8,
    LC_KOREA = 9,
    LC_FRANCE = 10,
    LC_ITALY = 11,
    LC_SPAIN = 12,
    LC_UK = 13,
    LC_TURKEY = 14,
    LC_POLAND = 15,
    LC_PORTUGAL = 16,
    LC_CANADA = 17,
    LC_BRAZIL = 18,
    LC_MAX = 19
};
extern eLocalization g_eLocalType;
#include "locale.hpp"
eLocalization LC_GetLocalType();
bool LC_IsLocale(eLocalization locale);
bool LC_IsYMIR();
bool LC_IsLetsGame();
bool LC_IsCIBN();
bool LC_IsJapan();
bool LC_IsEnglish();
bool LC_IsHongKong();
bool LC_IsNewCIBN();
bool LC_IsGermany();
bool LC_IsKorea();
bool LC_IsEurope();
bool LC_IsCanada();
bool LC_IsBrazil();
// Add to original game/src/locale_service.h.
int is_twobyte_euckr(const char*);
int is_twobyte_gb2312(const char*);
int is_twobyte_big5(const char*);
void LocaleService_LoadLocaleStringFile();
const std::string& LocaleService_GetBasePath();
void LocaleService_LoadEmpireTextConvertTables();
const std::string& LocaleService_GetMapPath();
bool LC_InitLocalization(const std::string& szLocal);

// Exact old locale_service.cpp function signatures from metadata.
int check_name_independent(const char*);
int check_name_gb2312(const char*);
int check_name_big5(const char*);
int check_name_euckr(const char*);
int check_name_latin1(const char*);
int check_name_alphabet(const char*);
bool sjis_is_disable_name_char(const char*);
int is_char_sjis(const char*,const char*);
int check_name_sjis(const char*);

void LocaleService_TransferDefaultSetting();

const std::string& LocaleService_GetQuestPath();

bool LocaleService_Init(const std::string& c_rstServiceName);

#endif
