#ifndef MCNCC_BANWORD_LOCALE_API_CHECKS_H
#define MCNCC_BANWORD_LOCALE_API_CHECKS_H
static void BanwordLocaleApiChecks() {
    bool(CBanwordManager::*a)(TBanwordTable*,WORD)=&CBanwordManager::Initialize;
    bool(CBanwordManager::*b)(const char*)=&CBanwordManager::Find;
    bool(CBanwordManager::*c)(const char*)=&CBanwordManager::CheckString;
    void(CBanwordManager::*d)(char*)=&CBanwordManager::ConvertString;
    void(*e)(const char*,char*,int)=str_lower;
    void(*f)(const char*)=SendLog;
    LPCHARACTER(DESC::*g)()=&DESC::GetCharacter;
    int(*names[])(const char*)={check_name_independent,check_name_gb2312,check_name_big5,
        check_name_euckr,check_name_latin1,check_name_alphabet,check_name_sjis};
    bool(*h)(const char*)=sjis_is_disable_name_char;
    int(*i)(const char*,const char*)=is_char_sjis;
    (void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)names;(void)h;(void)i;
}
#endif
