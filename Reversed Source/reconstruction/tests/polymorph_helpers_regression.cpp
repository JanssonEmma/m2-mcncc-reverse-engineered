#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/mob_manager.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/constants.h"
#include "../../game/src/config.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
struct QuestAssertion {};
static bool expectQuestAssertion;
static unsigned assertions,skillErrors,skillLogs,jobErrors;
static DWORD expectedVnum;
static std::vector<std::string> diagnostics;
extern "C" void helper_test_assert(const char* condition,const char* file,int) {
    if(expectQuestAssertion&&!std::strcmp(condition,"ms_singleton")&&std::strstr(file,"singleton.h")) {++assertions;throw QuestAssertion();}
}
extern "C" void helper_test_error(const char* function,int line,const char* format,const char* name,unsigned long value) {
    assert(!std::strcmp(name,"poly-helper"));
    if(!std::strcmp(function,"GetSkillLevel")){assert(line==2614&&!std::strcmp(format,"%s skill vnum overflow %lu")&&value==expectedVnum);++skillErrors;diagnostics.push_back("error");}
    else{assert(!std::strcmp(function,"GetJob")&&line==1569&&!std::strcmp(format,"CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE")&&value==255);++jobErrors;}
}
extern "C" void helper_test_log(int level,const char* format,const char* name,unsigned long value) {
    assert(level==0&&!std::strcmp(format,"%s skill vnum overflow %lu")&&!std::strcmp(name,"poly-helper")&&value==expectedVnum);++skillLogs;diagnostics.push_back("log");
}
static const int goldenPower[41]={10,11,11,12,13,13,14,15,16,17,18,19,20,22,23,24,26,27,29,31,33,35,37,39,41,44,46,49,52,55,59,62,66,70,74,79,84,89,94,100,0};
// Independent wide-integer oracle: emulate signed32 product/add at boundaries,
// rather than calling or copying the production unsigned-int helper.
static long long Signed32(unsigned long long bits){bits&=0xffffffffULL;return bits>=0x80000000ULL?static_cast<long long>(bits)-4294967296LL:static_cast<long long>(bits);}
static long Oracle(int base,unsigned stat,int power){
    long long product=static_cast<long long>(stat)*static_cast<long long>(power);
    long long quotient=Signed32(static_cast<unsigned long long>(product))/100;
    return static_cast<long>(Signed32(static_cast<unsigned long long>(static_cast<long long>(base)+quotient)));
}
static void ExpectQuestFailure(CHARACTER& ch,BYTE point){
    assert(CMobManager::instance_ptr()&&quest::CQuestManager::instance_ptr());
    quest::CQuestManager* saved=quest::CQuestManager::ms_singleton;quest::CQuestManager::ms_singleton=0;
    expectQuestAssertion=true;unsigned before=assertions;bool caught=false;
    try{(void)ch.GetPolymorphPoint(point);}catch(const QuestAssertion&){caught=true;}
    expectQuestAssertion=false;quest::CQuestManager::ms_singleton=saved;assert(caught&&assertions==before+1);
}
int main(){
    assert(sizeof(int)==4&&sizeof(long)==4&&sizeof(DWORD)==4&&sizeof(TPlayerSkill)==6);
    desc_test_allow_clock();quest::CQuestManager quests;CMobManager manager;CMob mob;CHARACTER ch;
    ch.m_stName="poly-helper";std::memset(&mob.m_table,0,sizeof(mob.m_table));mob.m_table.dwVnum=1001;
    mob.m_table.bStr=80;mob.m_table.bCon=60;mob.m_table.bDex=40;mob.m_table.bInt=100;mob.m_table.wDef=7;
    manager.m_map_pkMobByVnum[1001]=&mob;manager.m_map_pkMobByName["fixture"]=&mob;
    ch.m_pkMobData=&mob;assert(&ch.GetMobTable()==&mob.m_table);mob.m_table.wDef=123;assert(ch.GetMobTable().wDef==123);
    TPlayerSkill skills[255];std::memset(skills,0,sizeof(skills));
    for(unsigned i=0;i<255;++i){skills[i].bMasterType=static_cast<BYTE>(i);skills[i].tNextRead=static_cast<int>(i*3+7);}
    ch.m_pSkillLevels=skills;test_server=0;
    for(int i=0;i<41;++i)assert(aiPolymorphPowerByLevel[i]==goldenPower[i]);
    for(unsigned level=0;level<256;++level){
        skills[129].bLevel=static_cast<BYTE>(level);assert(ch.GetSkillLevel(129)==static_cast<int>(level>40?40:level));
        assert(ch.GetPolymorphPower()==goldenPower[level>40?40:level]);
        assert(skills[129].bMasterType==129&&skills[129].tNextRead==394);
    }
    skills[0].bLevel=17;skills[254].bLevel=255;assert(ch.GetSkillLevel(0)==17&&ch.GetSkillLevel(254)==40);
    const DWORD invalid[]={255UL,256UL,0xffffffffUL};
    for(unsigned i=0;i<3;++i){expectedVnum=invalid[i];diagnostics.clear();unsigned e=skillErrors,l=skillLogs;assert(ch.GetSkillLevel(invalid[i])==0);assert(skillErrors==e+1&&skillLogs==l+1&&diagnostics.size()==2&&diagnostics[0]=="error"&&diagnostics[1]=="log");}
    ch.m_pSkillLevels=0;assert(ch.GetSkillLevel(129)==0&&ch.GetPolymorphPower()==10);ch.m_pSkillLevels=skills;skills[129].bLevel=39;
    const int servers[]={0,1,-1,INT_MIN},flags[]={0,50,-50,INT_MIN,INT_MAX};
    for(unsigned s=0;s<4;++s)for(unsigned f=0;f<5;++f){test_server=servers[s];quests.m_mapEventFlag["poly"]=flags[f];assert(ch.GetPolymorphPower()==(servers[s]&&flags[f]?flags[f]:100));assert(quests.m_mapEventFlag["poly"]==flags[f]);}
    quests.m_mapEventFlag.clear();test_server=1;assert(ch.GetPolymorphPower()==100&&quests.m_mapEventFlag.empty());
    ch.m_pointsInstant.points[12]=20;ch.m_pointsInstant.points[13]=25;ch.m_pointsInstant.points[14]=30;ch.m_pointsInstant.points[15]=40;ch.m_pointsInstant.points[95]=333;
    ch.m_dwPolymorphRace=1001;ch.m_bPolyMaintainStat=false;ch.m_points.job=0;ch.m_points.skill_group=1;test_server=0;
    assert(ch.GetPolymorphPoint(12)==100&&ch.GetPolymorphPoint(13)==85&&ch.GetPolymorphPoint(14)==70&&ch.GetPolymorphPoint(15)==140&&ch.GetPolymorphPoint(95)==333);
    for(unsigned race=0;race<8;++race)for(unsigned group=0;group<4;++group){ch.m_points.job=static_cast<BYTE>(race);ch.m_points.skill_group=static_cast<BYTE>(group);bool iq=(race%4==3)||(race%4==2&&group==2);assert(ch.GetPolymorphPoint(12)==(iq?120:100));}
    ch.m_points.job=255;unsigned errors=jobErrors;assert(ch.GetPolymorphPoint(12)==100&&jobErrors==errors+2);
    errors=jobErrors;assert(ch.GetPolymorphPoint(13)==85&&ch.GetPolymorphPoint(14)==70&&ch.GetPolymorphPoint(15)==140&&jobErrors==errors);
    ch.m_points.job=3;skills[129].bLevel=40;assert(ch.GetPolymorphPoint(12)==40&&ch.GetPolymorphPoint(13)==25);
    skills[129].bLevel=39;ch.m_points.job=0;test_server=1;
    const int bases[]={INT_MIN,-1,0,20,INT_MAX};const unsigned stats[]={0,1,81,255};
    for(unsigned point=12;point<=15;++point)for(unsigned b=0;b<5;++b)for(unsigned t=0;t<4;++t)for(unsigned f=1;f<5;++f){
        ch.m_pointsInstant.points[point]=bases[b];
        switch(point){case 12:mob.m_table.bStr=static_cast<BYTE>(stats[t]);break;case 13:mob.m_table.bCon=static_cast<BYTE>(stats[t]);break;case 14:mob.m_table.bDex=static_cast<BYTE>(stats[t]);break;default:mob.m_table.bInt=static_cast<BYTE>(stats[t]);break;}
        quests.m_mapEventFlag["poly"]=flags[f];assert(ch.GetPolymorphPoint(static_cast<BYTE>(point))==Oracle(bases[b],stats[t],flags[f]));
    }
    // Missing mob and unsupported type MUST still enter Power's real singleton
    // guard. A narrow external assertion observer throws, with full stack unwind.
    ch.m_dwPolymorphRace=999999;ch.m_bPolyMaintainStat=false;ExpectQuestFailure(ch,12);
    ch.m_dwPolymorphRace=1001;ExpectQuestFailure(ch,95);
    quest::CQuestManager* saved=quest::CQuestManager::ms_singleton;quest::CQuestManager::ms_singleton=0;
    ch.m_dwPolymorphRace=0;assert(ch.GetPolymorphPoint(95)==333&&ch.GetPolymorphPoint(12)==ch.GetPoint(12));ch.m_dwPolymorphRace=1001;ch.m_bPolyMaintainStat=true;assert(ch.GetPolymorphPoint(95)==333&&ch.GetPolymorphPoint(12)==ch.GetPoint(12));quest::CQuestManager::ms_singleton=saved;
    ch.m_bPolyMaintainStat=false;ch.m_dwPolymorphRace=999999;quests.m_mapEventFlag["poly"]=-50;std::size_t count=quests.m_mapEventFlag.size();assert(ch.GetPolymorphPoint(95)==333&&quests.m_mapEventFlag.size()==count&&quests.m_mapEventFlag["poly"]==-50&&manager.m_map_pkMobByVnum.size()==1);
    ch.m_pSkillLevels=0;ch.m_pkMobData=0;ch.m_dwPolymorphRace=0;test_server=0;quests.m_mapEventFlag.clear();manager.m_map_pkMobByVnum.clear();manager.m_map_pkMobByName.clear();
    assert(jobErrors==2&&skillErrors==3&&skillLogs==3&&assertions==2);
    std::puts("polymorph helpers passed: real lookup/flags/points/job/MIN, all skill bytes, exact table, diagnostics and signed32 arithmetic");
}
