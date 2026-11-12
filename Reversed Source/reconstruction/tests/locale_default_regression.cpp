// Actual complete production locale TU: original static helper is called in same TU.
// No public setter, app mock or production test-ifdef is added.
#include "../../game/src/locale_service.cpp"
#include <cassert>
#include <cstdio>
#include <set>
#include <string>
int max_bytes_written,current_bytes_written,total_bytes_written;
static void OtherState() {
    assert(g_stLocal=="keep-local" && g_stLocale=="keep-encoding");
    assert(g_stDefaultQuestObjectDir=="keep-default-object");
    assert(g_eLocalType==LC_BRAZIL && g_iUseLocale==11 && PK_PROTECT_LEVEL==23);
    assert(g_bAuthServer && test_server==-1);
    assert(check_name==check_name_big5 && is_twobyte==is_twobyte_gb2312);
    assert(exp_table==exp_table_newcibn && aiPercentByDeltaLev==aiPercentByDeltaLev_sjis);
    assert(aiPercentByDeltaLevForBoss==aiPercentByDeltaLev_gb2312 &&
           aiChainLightningCountBySkillLevel==aiChainLightningCountBySkillLevel_gb2312);
}
static void SeedOtherState() {
    g_stLocal="keep-local";g_stLocale="keep-encoding";g_stDefaultQuestObjectDir="keep-default-object";
    g_eLocalType=LC_BRAZIL;g_iUseLocale=11;PK_PROTECT_LEVEL=23;g_bAuthServer=true;test_server=-1;
    check_name=check_name_big5;is_twobyte=is_twobyte_gb2312;
    exp_table=exp_table_newcibn;aiPercentByDeltaLev=aiPercentByDeltaLev_sjis;
    aiPercentByDeltaLevForBoss=aiPercentByDeltaLev_gb2312;
    aiChainLightningCountBySkillLevel=aiChainLightningCountBySkillLevel_gb2312;
}
int main() {
    void(*const original)()=&__LocaleService_Init_DEFAULT;
    const std::string&(*const getter)()=&LocaleService_GetQuestPath;
    (void)original;(void)getter;
    assert(g_stQuestDir=="./quest" && g_stDefaultQuestObjectDir=="./quest/object");
    assert(g_setQuestObjectDir.empty());
    assert(g_stServiceName.empty() && g_stServiceBasePath=="." && g_stServiceMapPath=="data/map");
    const std::string& persistent=LocaleService_GetQuestPath();
    assert(&persistent==&g_stQuestDir && &LocaleService_GetQuestPath()==&persistent);
    SeedOtherState();
    const std::string names[]={std::string(),std::string("ymir"),std::string("custom-service"),
        std::string("nested/path"),std::string(".."),std::string("trailing/"),
        std::string("path with spaces"),std::string("a//b"),std::string("ab\0cd",5)};
    for(unsigned i=0;i<sizeof names/sizeof names[0];++i)
        for(unsigned initial=0;initial<4;++initial) {
            g_stServiceName=names[i];g_stServiceBasePath="obsolete-base";
            g_stServiceMapPath="obsolete-map";g_stQuestDir="obsolete-quest";
            g_stLocaleFilename="obsolete-translation-file";
            g_setQuestObjectDir.clear();
            if(initial>=1) g_setQuestObjectDir.insert("obsolete");
            if(initial>=2) {g_setQuestObjectDir.insert("more-obsolete");g_setQuestObjectDir.insert(std::string("x\0y",3));}
            if(initial==3) g_setQuestObjectDir.insert("locale/"+names[i]+"/quest/object");
            const std::string expectedBase="locale/"+names[i],expectedQuest=expectedBase+"/quest";
            for(unsigned repeat=0;repeat<2;++repeat) {
                if(repeat) {
                    g_stLocaleFilename="reintroduced-file";
                    g_setQuestObjectDir.insert("reintroduced-object");
                }
                __LocaleService_Init_DEFAULT();
                assert(g_stServiceName==names[i] && g_stServiceBasePath==expectedBase);
                assert(g_stServiceMapPath==expectedBase+"/map" && g_stLocaleFilename.empty());
                assert(g_stQuestDir==expectedQuest && persistent==expectedQuest);
                assert(&LocaleService_GetQuestPath()==&persistent);
                assert(g_setQuestObjectDir.size()==1 && *g_setQuestObjectDir.begin()==expectedQuest+"/object");
                OtherState();
            }
        }
    // Getter always aliases current complete string, preserving embeddedNUL.
    const std::string direct("direct\0quest",12);g_stQuestDir=direct;
    assert(persistent==direct && &LocaleService_GetQuestPath()==&g_stQuestDir);
    std::puts("locale DEFAULT passed: actual static complete body, real config globals,72 path/set resets, binary service names, persistent QuestPath and all unrelated callback/table/flag state");
}
