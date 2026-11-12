// Integration regression: real CHARACTER ctor/GetName/GetRaceNum and stat helpers.
// Only external syscalls/logger and existing unrelated lifecycle boundaries are fixtures.
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
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/mob_manager.h"
#include "../../game/src/packet.h"
#include "../../common/tables.h"
#include <cassert>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int max_bytes_written,current_bytes_written,total_bytes_written;
extern bool RaceToJob(unsigned race,unsigned* ret_job);
extern "C" void desc_test_allow_clock(void);

static struct JobObserver {
    bool armed;
    unsigned expectedRace,calls;
    std::string expectedName;
    JobObserver():armed(false),expectedRace(0),calls(0) {}
} observer;

// Invoked only by stat_primitives_desc_output_syscalls.c at the external log boundary.
extern "C" void stat_primitives_observe_job_error(const char* function,int line,
        const char* format,const char* name,unsigned race) {
    assert(observer.armed);
    assert(std::strcmp(function,"GetJob")==0 && line==1569);
    assert(std::strcmp(format,"CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE")==0);
    assert(name && observer.expectedName==name && race==observer.expectedRace);
    observer.armed=false;
    ++observer.calls;
}

struct PointSnapshot {
    unsigned char real[sizeof(CHARACTER_POINT)];
    unsigned char instant[sizeof(CHARACTER_POINT_INSTANT)];
    explicit PointSnapshot(const CHARACTER& ch) {
        std::memcpy(real,&ch.m_points,sizeof(real));
        std::memcpy(instant,&ch.m_pointsInstant,sizeof(instant));
    }
    void Check(const CHARACTER& ch) const {
        assert(std::memcmp(real,&ch.m_points,sizeof(real))==0);
        assert(std::memcmp(instant,&ch.m_pointsInstant,sizeof(instant))==0);
    }
};

static void CheckJob(CHARACTER& ch,unsigned rawRace,const char* expectedName) {
    assert(rawRace<256);
    ch.m_points.job=static_cast<BYTE>(rawRace);
    PointSnapshot before(ch);
    const unsigned calls=observer.calls;
    observer.expectedRace=rawRace;
    observer.expectedName=expectedName;
    observer.armed=rawRace>=8;
    const BYTE result=ch.GetJob();
    assert(result==(rawRace<8?rawRace%4:0));
    assert(!observer.armed);
    assert(observer.calls==calls+(rawRace>=8?1U:0U));
    before.Check(ch);
}

static void CheckRaceToJob() {
    bool (*convert)(unsigned,unsigned*)=&RaceToJob;
    for(unsigned race=0;race<256;++race) {
        unsigned job=0xdeadbeefU;
        const bool result=convert(race,&job);
        assert(result==(race<8));
        assert(job==(race<8?race%4:0));
    }
    const unsigned invalid[]={256U,257U,65535U,0x7fffffffU,0x80000000U,UINT_MAX};
    for(unsigned i=0;i<sizeof(invalid)/sizeof(*invalid);++i) {
        unsigned job=12345;
        assert(!convert(invalid[i],&job) && job==0);
    }
    // ret_job!=0 is an original precondition: do not execute a null-output call.
}

static void CheckJobs(CHARACTER& ch,CMob& mob) {
    assert(!ch.GetDesc());
    ch.m_stName="raw-job-test";
    ch.m_pkMobData=0;
    ch.m_dwPolymorphRace=0;
    const unsigned first=observer.calls;
    for(unsigned race=0;race<256;++race) CheckJob(ch,race,"raw-job-test");
    assert(observer.calls-first==248);

    // Actual GetRaceNum disagrees with raw job: GetJob must ignore polymorph.
    ch.m_dwPolymorphRace=101;
    assert(ch.GetRaceNum()==101);
    CheckJob(ch,6,"raw-job-test"); // job2 despite displayed race101
    CheckJob(ch,255,"raw-job-test"); // diagnostic must report raw255, not101

    // Real CMob and GetRaceNum; no application method replacement.
    std::memset(&mob.m_table,0,sizeof(mob.m_table));
    mob.m_table.dwVnum=203;
    std::strcpy(mob.m_table.szLocaleName,"mob-job-name");
    ch.m_dwPolymorphRace=0;
    ch.m_pkMobData=&mob;
    assert(ch.GetRaceNum()==203);
    CheckJob(ch,5,"raw-job-test"); // raw5 maps1 despite mob203

    // GetName remains real, including named-player and mob-name fallbacks.
    ch.m_stName.clear();
    assert(std::strcmp(ch.GetName(),"mob-job-name")==0);
    CheckJob(ch,7,"mob-job-name");
    CheckJob(ch,254,"mob-job-name");
    ch.m_pkMobData=0;
    assert(std::strcmp(ch.GetName(),"")==0);
    CheckJob(ch,8,"");
    assert(observer.calls-first==251);
    ch.m_stName="state-test";
}

static long SignedBits(unsigned long bits) {
    assert(sizeof(bits)==sizeof(long));
    long result;
    std::memcpy(&result,&bits,sizeof(result));
    return result;
}
static void CheckState(CHARACTER& ch,long position,int hp,unsigned long bits) {
    ch.m_pointsInstant.position=position;
    ch.m_points.hp=hp;
    ch.m_pointsInstant.instant_flag=SignedBits(bits);
    PointSnapshot before(ch);
    const unsigned errors=observer.calls;
    assert(ch.IsDead()==(position==0));
    assert(ch.IsStun()==((bits&8UL)!=0));
    assert(observer.calls==errors && !observer.armed);
    before.Check(ch);
}
static void CheckStates(CHARACTER& ch) {
    const long positions[]={POS_DEAD,POS_SLEEPING,POS_RESTING,POS_SITTING,
        POS_FISHING,POS_FIGHTING,POS_MOUNTING,POS_STANDING,-1,-2,LONG_MIN,LONG_MAX};
    const int hitpoints[]={0,1,-1,INT_MIN,INT_MAX};
    const unsigned long flags[]={0UL,8UL,1UL,2UL,4UL,16UL,0x80000000UL,0xffffffffUL,0x7fffffffUL};
    for(unsigned p=0;p<sizeof(positions)/sizeof(*positions);++p)
        for(unsigned h=0;h<sizeof(hitpoints)/sizeof(*hitpoints);++h)
            for(unsigned f=0;f<sizeof(flags)/sizeof(*flags);++f)
                CheckState(ch,positions[p],hitpoints[h],flags[f]);
    for(unsigned bit=0;bit<32;++bit) {
        const unsigned long single=1UL<<bit;
        // Only bit3 matters, with either dead or living position and zero/positive HP.
        CheckState(ch,POS_DEAD,0,single);
        CheckState(ch,POS_STANDING,100,single);
        CheckState(ch,POS_FIGHTING,0,single|8UL);
        CheckState(ch,POS_DEAD,-1,single&~8UL);
    }
    ch.m_pointsInstant.position=POS_STANDING;
    ch.m_pointsInstant.instant_flag=0;
    ch.m_points.hp=0;
}

static void Put4(unsigned char* bytes,unsigned long value) {
    for(unsigned i=0;i<4;++i) bytes[i]=static_cast<unsigned char>(value>>(i*8));
}
static void CheckSkillData() {
    assert(sizeof(SPlayerSkill)==6 && sizeof(TPlayerSkill)==6);
    // Compile-time member pointer proves terminal old time_t storage is signed int.
    int TPlayerSkill::* timeField=&TPlayerSkill::tNextRead;
    (void)timeField;
    const int times[]={INT_MIN,-1,0,INT_MAX};
    for(unsigned i=0;i<sizeof(times)/sizeof(*times);++i) {
        TPlayerSkill skill;
        skill.bMasterType=3;skill.bLevel=40;skill.tNextRead=times[i];
        unsigned char expected[6]={3,40,0,0,0,0};
        Put4(expected+2,static_cast<unsigned>(times[i]));
        assert(std::memcmp(&skill,expected,6)==0);
        TPlayerSkill decoded;
        std::memcpy(&decoded,expected,6);
        assert(decoded.bMasterType==3 && decoded.bLevel==40 && decoded.tNextRead==times[i]);
        expected[0]=0;
        assert(decoded.bMasterType==3);
    }
    const unsigned char negativeTwo[6]={3,40,0xfe,0xff,0xff,0xff};
    TPlayerSkill decoded;
    std::memcpy(&decoded,negativeTwo,6);
    assert(decoded.bMasterType==3 && decoded.bLevel==40 && decoded.tNextRead==-2);
}
static void CheckPacketData() {
    assert(sizeof(packet_point_change)==17 && sizeof(TPacketGCPointChange)==17);
    int TPacketGCPointChange::* headerField=&TPacketGCPointChange::header;
    long TPacketGCPointChange::* amountField=&TPacketGCPointChange::amount;
    long TPacketGCPointChange::* valueField=&TPacketGCPointChange::value;
    (void)headerField;(void)amountField;(void)valueField;
    const unsigned char golden[17]={0x11,0,0,0,0xef,0xcd,0xab,0x89,0x0d,
        0xfc,0xfc,0xfd,0xfe,0x78,0x56,0x34,0x12};
    TPacketGCPointChange packet;
    packet.header=17;packet.dwVID=0x89abcdefUL;packet.type=13;
    packet.amount=-0x01020304L;packet.value=0x12345678L;
    assert(std::memcmp(&packet,golden,17)==0);
    TPacketGCPointChange decoded;
    std::memcpy(&decoded,golden,17);
    assert(decoded.header==17 && decoded.dwVID==0x89abcdefUL && decoded.type==13);
    assert(decoded.amount==-0x01020304L && decoded.value==0x12345678L);

    const int headers[]={INT_MIN,-1,0,INT_MAX};
    const long values[]={LONG_MIN,-1,0,LONG_MAX};
    for(unsigned h=0;h<4;++h) for(unsigned a=0;a<4;++a) for(unsigned v=0;v<4;++v) {
        packet.header=headers[h];packet.dwVID=0x89abcdefUL;packet.type=255;
        packet.amount=values[a];packet.value=values[v];
        unsigned char expected[17];
        Put4(expected,static_cast<unsigned>(headers[h]));
        Put4(expected+4,0x89abcdefUL);expected[8]=255;
        Put4(expected+9,static_cast<unsigned long>(values[a]));
        Put4(expected+13,static_cast<unsigned long>(values[v]));
        assert(std::memcmp(&packet,expected,17)==0);
        std::memcpy(&decoded,expected,17);
        assert(decoded.header==headers[h] && decoded.dwVID==0x89abcdefUL && decoded.type==255);
        assert(decoded.amount==values[a] && decoded.value==values[v]);
    }
}

int main() {
    assert(sizeof(int)==4 && sizeof(unsigned)==4 && sizeof(long)==4 && sizeof(DWORD)==4);
    // Check old non-const member signatures without replacing any method.
    BYTE (CHARACTER::*jobSignature)()=&CHARACTER::GetJob;
    bool (CHARACTER::*deadSignature)()=&CHARACTER::IsDead;
    bool (CHARACTER::*stunSignature)()=&CHARACTER::IsStun;
    (void)jobSignature;(void)deadSignature;(void)stunSignature;
    CheckSkillData();CheckPacketData();CheckRaceToJob();
    desc_test_allow_clock();
    quest::CQuestManager quests;
    CMob mob;
    CHARACTER character; // Actual constructor and Initialize, existing lifecycle fixture only.
    assert(!observer.calls);
    CheckJobs(character,mob);
    CheckStates(character);
    assert(observer.calls==251 && !observer.armed);
    character.m_pkMobData=0;character.m_dwPolymorphRace=0;
    std::puts("stat primitives passed: real raw-job conversion, exact GetJob1569 diagnostics, dead/stun predicates, skill6 and point-packet17 bytes");
}
