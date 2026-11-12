// Complete real locale functions and parser, shared full server test graph.
// Only sys_log/_sys_err and C stdio boundaries are observed externally.
#include "../../game/src/locale_service.h"
#include "../../game/src/locale.hpp"
#include "../../game/src/config.h"
#include "../../game/src/constants.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#ifdef _WIN32
#include <io.h>
#define LF_DUP _dup
#define LF_DUP2 _dup2
#define LF_FILENO _fileno
#define LF_CLOSE _close
#else
#include <unistd.h>
#define LF_DUP dup
#define LF_DUP2 dup2
#define LF_FILENO fileno
#define LF_CLOSE close
#endif
extern std::string g_stServiceName,g_stServiceBasePath,g_stServiceMapPath,g_stLocal;
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" {
void desc_test_allow_clock(void);
void locale_test_stdio_begin(const char*);
void locale_test_stdio_finish(void);
int locale_test_stdio_active(void);
}
static bool expect_big5_log;
static unsigned expected_first,expected_second,big5_calls;
extern "C" void locale_foundation_observe_big5(int level,unsigned first,unsigned second) {
    assert(expect_big5_log && level==0);
    assert(first==expected_first && second==expected_second);
    ++big5_calls;assert(big5_calls==1);
}
class Capture {
    FILE* file;int saved;
public:
    Capture():file(std::tmpfile()),saved(-1) {
        assert(file);std::fflush(stderr);saved=LF_DUP(LF_FILENO(stderr));assert(saved>=0);
        assert(LF_DUP2(LF_FILENO(file),LF_FILENO(stderr))>=0);
    }
    ~Capture() {
        std::fflush(stderr);assert(LF_DUP2(saved,LF_FILENO(stderr))>=0);
        LF_CLOSE(saved);std::fclose(file);
    }
    long Mark() {
        std::fflush(stderr);assert(std::fseek(file,0,SEEK_END)==0);
        long n=std::ftell(file);assert(n>=0);return n;
    }
    void Expect(long start,const std::string& expected) {
        long end=Mark();char text[2048];
        assert(end>=start && end-start<static_cast<long>(sizeof text));
        assert(std::fseek(file,start,SEEK_SET)==0);
        std::size_t n=std::fread(text,1,static_cast<std::size_t>(end-start),file);
        assert(n==static_cast<std::size_t>(end-start));
        assert(std::string(text,n)==expected);
        assert(std::fseek(file,0,SEEK_END)==0);
    }
};
static void Predicates() {
    assert(is_twobyte_gb2312(0)==0 && is_twobyte_big5(0)==0);
    char empty[2]={0,static_cast<char>(0xff)};
    assert(is_twobyte_gb2312(empty)==0 && is_twobyte_big5(empty)==0);
    for(unsigned first=0;first<256;++first) {
        char text[3]={static_cast<char>(first),0,0};
        const bool euckr=first>=0xa0U;
        assert(is_twobyte_euckr(text)==static_cast<int>(euckr));
    }
    for(int debug=0;debug<2;++debug) {
        test_server=debug;
        for(unsigned first=0;first<256;++first)
            for(unsigned second=0;second<256;++second) {
                char text[3]={static_cast<char>(first),static_cast<char>(second),0};
                const unsigned code=(first<<8)|second;
                const bool gb=first>=0xb0U && first<=0xf7U && second>=0xa1U && second<=0xfeU;
                const bool big=(code>=0xa440U && code<=0xc67eU) ||
                               (code>=0xc940U && code<=0xf9d5U);
                expected_first=first;expected_second=second;big5_calls=0;
                expect_big5_log=debug!=0 && first>=0x80U && !big;
                assert(is_twobyte_gb2312(text)==static_cast<int>(gb));
                assert(big5_calls==0);
                assert(is_twobyte_big5(text)==static_cast<int>(big));
                assert(big5_calls==static_cast<unsigned>(expect_big5_log));
                expect_big5_log=false;
                assert(static_cast<unsigned char>(text[0])==first &&
                       static_cast<unsigned char>(text[1])==second && text[2]==0);
            }
    }
    test_server=0;
}
static void SetIndependent() {
    g_stServiceName="untouched-service";g_stServiceBasePath="untouched/base";
    g_stServiceMapPath="untouched/map";g_stLocaleFilename="untouched-file.dat";
    g_stLocale="untouched-encoding";g_iUseLocale=1;PK_PROTECT_LEVEL=17;
    check_name=is_twobyte_euckr;is_twobyte=is_twobyte_big5;
    exp_table=exp_table_gb2312;aiPercentByDeltaLev=aiPercentByDeltaLev_sjis;
    aiPercentByDeltaLevForBoss=aiPercentByDeltaLev_gb2312;
    aiChainLightningCountBySkillLevel=aiChainLightningCountBySkillLevel_gb2312;
}
static void Independent(const std::string& filename="untouched-file.dat") {
    assert(g_stServiceName=="untouched-service" && g_stServiceBasePath=="untouched/base");
    assert(g_stServiceMapPath=="untouched/map" && g_stLocaleFilename==filename);
    assert(g_stLocale=="untouched-encoding" && g_iUseLocale==1 && PK_PROTECT_LEVEL==17);
    assert(check_name==is_twobyte_euckr && is_twobyte==is_twobyte_big5);
    assert(exp_table==exp_table_gb2312 && aiPercentByDeltaLev==aiPercentByDeltaLev_sjis);
    assert(aiPercentByDeltaLevForBoss==aiPercentByDeltaLev_gb2312 &&
           aiChainLightningCountBySkillLevel==aiChainLightningCountBySkillLevel_gb2312);
}
static void Localization(Capture& capture) {
    const char* names[]={"ymir","letsgame","cibn","japan","english","hongkong","newcibn",
        "germany","korea","france","italy","spain","uk","turkey","poland","portugal","canada","brazil"};
    SetIndependent();test_server=1;g_bAuthServer=true;
    long start=capture.Mark();
    for(unsigned i=0;i<18;++i) {
        g_eLocalType=LC_NOSET;
        assert(LC_InitLocalization(names[i]));
        assert(static_cast<int>(g_eLocalType)==static_cast<int>(i)+1 && g_stLocal==names[i]);
        Independent();assert(test_server==1 && g_bAuthServer);
        assert(LC_InitLocalization(names[i])); // no one-time guard
        Independent();assert(test_server==1 && g_bAuthServer);
        std::string upper=names[i];
        for(std::size_t c=0;c<upper.size();++c) upper[c]=static_cast<char>(upper[c]-'a'+'A');
        const eLocalization before=g_eLocalType;
        assert(!LC_InitLocalization(upper) && g_eLocalType==before && g_stLocal==upper);
        Independent();assert(test_server==1 && g_bAuthServer);
    }
    const std::string unknowns[]={std::string(),std::string(" japan"),std::string("japan "),
        std::string("unknown"),std::string("japan\0extra",11),std::string("japan\0",6)};
    for(int previous=0;previous<=18;++previous)
        for(unsigned i=0;i<sizeof unknowns/sizeof unknowns[0];++i) {
            g_eLocalType=static_cast<eLocalization>(previous);
            assert(!LC_InitLocalization(unknowns[i]));
            assert(static_cast<int>(g_eLocalType)==previous && g_stLocal==unknowns[i]);
            Independent();assert(test_server==1 && g_bAuthServer);
        }
    assert(LC_InitLocalization("japan"));
    assert(LC_InitLocalization(g_stLocal) && g_eLocalType==LC_JAPAN && g_stLocal=="japan");
    Independent();assert(test_server==1 && g_bAuthServer);capture.Expect(start,"");
    test_server=0;g_bAuthServer=false;
}
static void Getters() {
    SetIndependent();
    const std::string& base=LocaleService_GetBasePath();
    const std::string& map=LocaleService_GetMapPath();
    assert(&base==&g_stServiceBasePath && &map==&g_stServiceMapPath);
    assert(&LocaleService_GetBasePath()==&base && &LocaleService_GetMapPath()==&map);
    const std::string newBase("base\0suffix",11),newMap("map\0suffix",10);
    g_stServiceBasePath=newBase;g_stServiceMapPath=newMap;
    assert(base==newBase && map==newMap);
    assert(&LocaleService_GetBasePath()==&base && &LocaleService_GetMapPath()==&map);
    SetIndependent();Independent();
}
static void Loader(Capture& capture) {
    SetIndependent();g_eLocalType=LC_GERMANY;test_server=0;g_bAuthServer=false;
    const std::string guardLocal=g_stLocal;const eLocalization guardEnum=g_eLocalType;
    long start=capture.Mark();
    // If either skip guard regresses, the external fopen boundary rejects the call
    // before the actual parser could attempt an absent file or process exit.
    g_stLocaleFilename.clear();LocaleService_LoadLocaleStringFile();
    assert(!locale_test_stdio_active());Independent("");
    g_bAuthServer=true;g_stLocaleFilename="locale foundation deliberately absent.tmp";
    LocaleService_LoadLocaleStringFile();assert(!locale_test_stdio_active());
    Independent("locale foundation deliberately absent.tmp");
    g_stLocaleFilename.clear();LocaleService_LoadLocaleStringFile();
    Independent("");assert(g_stLocal==guardLocal && g_eLocalType==guardEnum);
    capture.Expect(start,"" );
    g_bAuthServer=false;
    const char* filename="locale foundation real input.tmp";
    const char fixture[]="\"foundation-key-one\";\"first translated value\";\n"
                         "\"foundation-key-two\";\"second translated value\";\n";
    FILE* output=std::fopen(filename,"wb");assert(output);
    assert(std::fwrite(fixture,1,sizeof(fixture)-1,output)==sizeof(fixture)-1);
    assert(std::fclose(output)==0);
    g_stLocaleFilename=filename;
    const std::string localBefore=g_stLocal;
    const eLocalization enumBefore=g_eLocalType;
    start=capture.Mark();locale_test_stdio_begin(g_stLocaleFilename.c_str());
    LocaleService_LoadLocaleStringFile();
    // Verifies original open/seek/tell/read order and parser's unclosed stream,
    // then performs external test cleanup so Windows can remove the fixture.
    locale_test_stdio_finish();
    capture.Expect(start,std::string("LocaleService ")+filename+"\n");
    assert(std::remove(filename)==0);
    Independent(filename);assert(g_stLocal==localBefore && g_eLocalType==enumBefore);
    const char* first=locale_find("foundation-key-one",1);
    const char* second=locale_find("foundation-key-two",1);
    assert(std::strcmp(first,"first translated value")==0);
    assert(std::strcmp(second,"second translated value")==0 && first!=second);
    // Old map owns no strings in its destructor; cleanup only after real lookups.
    delete[] first;delete[] second;
}
int main() {
    assert(sizeof(int)==4);
    assert(g_stServiceName.empty() && g_stServiceBasePath=="." && g_stServiceMapPath=="data/map");
    assert(g_stLocaleFilename.empty() && g_stLocal.empty() && g_stLocale=="euckr");
    assert(!check_name && !is_twobyte && !exp_table && !aiPercentByDeltaLev &&
           !aiPercentByDeltaLevForBoss && !aiChainLightningCountBySkillLevel);
    assert(g_eLocalType==LC_NOSET && g_iUseLocale==0 && PK_PROTECT_LEVEL==30);
    desc_test_allow_clock();
    { Capture capture;Predicates();Localization(capture);Getters();Loader(capture); }
    g_stServiceName.clear();g_stServiceBasePath=".";g_stServiceMapPath="data/map";
    g_stLocaleFilename.clear();g_stLocal.clear();g_stLocale="euckr";
    check_name=0;is_twobyte=0;exp_table=0;aiPercentByDeltaLev=0;
    aiPercentByDeltaLevForBoss=0;aiChainLightningCountBySkillLevel=0;
    g_eLocalType=LC_NOSET;g_iUseLocale=0;PK_PROTECT_LEVEL=30;test_server=0;g_bAuthServer=false;
    std::puts("locale foundation: all65536 bytepairs/both debug modes,18 locale names/state isolation, getter identity and real file/parser/translation passed");
}
