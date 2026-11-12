#include "../../game/src/locale_service.h"
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <string>
#include "../../game/src/locale.hpp"
#include "../../game/src/config.h"
extern "C" void sys_log(int,const char*,...) { assert(!"unused locale diagnostic path"); }
static int errors,last_line;
static std::string error_format,error_text;
extern "C" void _sys_err(const char* function,int line,const char* format,...) {
    assert(std::strcmp(function,"locale_find")==0); ++errors; last_line=line; error_format=format;
    va_list args; va_start(args,format); error_text=va_arg(args,const char*); va_end(args);
}

int main() {
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
