#include "../../game/src/locale_service.h"
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <string>
#include <vector>
#include "../../game/src/locale.hpp"
#include "../../game/src/config.h"
static std::vector<int> logged_map_allow;
extern "C" void sys_log(int level,const char* format,...)
{
    assert(level == 0 && std::strcmp(format, "MAP_ALLOW: %d") == 0);
    va_list args;
    va_start(args, format);
    logged_map_allow.push_back(va_arg(args, int));
    va_end(args);
}
static int errors,last_line;
static std::string error_format,error_text;
extern "C" void _sys_err(const char* function,int line,const char* format,...) {
    assert(std::strcmp(function,"locale_find")==0); ++errors; last_line=line; error_format=format;
    va_list args; va_start(args,format); error_text=va_arg(args,const char*); va_end(args);
}

static void CheckMapAllow()
{
    const BYTE guard = 0xA5;
    BYTE output[] = { guard, guard, guard, guard, guard, guard };
    assert(!g_bAuthServer);
    assert(!map_allow_find(0));
    map_allow_copy(output, 1);
    map_allow_log();
    assert(logged_map_allow.empty());
    assert(output[0] == guard);

    map_allow_add(513);
    map_allow_add(-2);
    map_allow_add(300);
    assert(map_allow_find(-2) && map_allow_find(300) && map_allow_find(513));
    assert(!map_allow_find(0) && !map_allow_find(44));

    // Nonempty copies store before testing size, with signed sorting and BYTE narrowing.
    map_allow_copy(output + 1, -1);
    assert(output[0] == guard && output[1] == 254 && output[2] == guard);
    output[1] = guard;
    map_allow_copy(output + 1, 0);
    assert(output[0] == guard && output[1] == 254 && output[2] == guard);
    output[1] = guard;
    map_allow_copy(output + 1, 1);
    assert(output[0] == guard && output[1] == 254 && output[2] == 44 && output[3] == guard);

    g_bAuthServer = true;
    assert(!map_allow_find(-2) && !map_allow_find(513));
    map_allow_add(513); // Auth mode never detects duplicates in find; set insertion stays unique.
    map_allow_add(9);
    assert(!map_allow_find(9));
    map_allow_copy(output + 1, 20);
    assert(output[0] == guard && output[1] == 254 && output[2] == 9 &&
           output[3] == 44 && output[4] == 1 && output[5] == guard);
    map_allow_log();
    assert(logged_map_allow.size() == 4 && logged_map_allow[0] == -2 &&
           logged_map_allow[1] == 9 && logged_map_allow[2] == 300 && logged_map_allow[3] == 513);
    g_bAuthServer = false;
    assert(map_allow_find(9) && map_allow_find(513));
}

int main() {
    CheckMapAllow();
    assert(g_eLocalType==LC_NOSET && g_iUseLocale==0);
    bool (*predicates[])()={LC_IsYMIR,LC_IsLetsGame,LC_IsCIBN,LC_IsJapan,LC_IsEnglish,
        LC_IsHongKong,LC_IsNewCIBN,LC_IsGermany,LC_IsKorea,LC_IsCanada,LC_IsBrazil};
    const int expected[]={1,2,3,4,5,6,7,8,9,17,18};
    for(int i=0;i<=19;++i) {
        g_eLocalType=static_cast<eLocalization>(i);
        assert(LC_GetLocalType()==g_eLocalType && LC_IsLocale(g_eLocalType));
        for(size_t j=0;j<sizeof predicates/sizeof predicates[0];++j) assert(predicates[j]()==(i==expected[j]));
        assert(LC_IsEurope()==(i>=10 && i<=16));
    }
    g_eLocalType=LC_GERMANY; assert(!LC_IsEurope()); // Target predicate excludes Germany.
    g_eLocalType=LC_KOREA; g_iUseLocale=0; assert(LC_IsKorea());
    g_iUseLocale=1; assert(LC_IsKorea());
    g_eLocalType=LC_NOSET; g_iUseLocale=0;
    g_eLocalType=LC_GERMANY;
    const char key[]="source"; const char* pair[]={key,"translated"};
    locale_add(pair); const char* first=locale_find(key,1); assert(std::strcmp(first,"translated")==0 && first!=pair[1]);
    const char* replacement[]={key,"replacement"}; locale_add(replacement);
    const char* second=locale_find(key,1); assert(std::strcmp(second,"replacement")==0 && second!=first && std::strcmp(first,"translated")==0);
    const char* retain[]={key,0}; locale_add(retain); assert(locale_find(key,1)==second);
    const char* ignored[]={0,"ignored"}; locale_add(ignored);
    assert(locale_find(key,0)==key && locale_find(key,2)==key && !errors);
    test_server=1; assert(locale_find(key,0)==key && last_line==51 && error_format=="LOCALE_ERROR: locae == 0  \"%s\";" && error_text==key);
    g_eLocalType=LC_KOREA; assert(locale_find(key,1)==key && last_line==51);
    test_server=0; g_eLocalType=LC_GERMANY;
    const char empty[]="untranslated"; const char* nullpair[]={empty,0}; locale_add(nullpair);
    int before=errors; assert(locale_find(empty,1)==empty && errors==before);
    test_server=1; assert(locale_find(empty,1)==empty && last_line==70 && error_format=="LOCALE_ERROR: text is NULL  \"%s\";"); test_server=0;
    const char* missing=locale_find("missing",1); assert(std::strcmp(missing,"@0949missing")==0 && last_line==61);
    const char* missing_again=locale_find("other",1); assert(missing==missing_again && std::strcmp(missing,"@0949other")==0);
    std::string longkey(1500,'x'); const char* cropped=locale_find(longkey.c_str(),1); assert(cropped==missing && std::strlen(cropped)==1023 && std::strncmp(cropped,"@0949",5)==0 && cropped[1022]=='x');
    assert(locale_find("z",1)==missing && std::strcmp(missing,"@0949z")==0);
    delete[] first; delete[] second; // Explicit test cleanup; the old map has a non-owning destructor.
    g_eLocalType=LC_NOSET; g_iUseLocale=0;
    std::puts("locale translation passed: replacement pointer lifetime, source/null fallback, Korean bypass, exact diagnostics and shared cropped missing buffer");
    std::puts("locale predicates passed: exact old enum, independent flag and Europe membership");
}
