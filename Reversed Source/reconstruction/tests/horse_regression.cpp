#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <map>
#include <sys/select.h>
#define private public
#include "../../game/src/horse_rider.h"
#undef private
#include "../../game/src/config.h"
#include "../../libthecore/include/main.h"
static HEART heart={0,{0,0},{0,0},{0,0},25,0};
LPHEART thecore_heart=&heart;
static time_t now=1000000;
time_t get_global_time() { return now; }
static int errors;
extern "C" void sys_log(int,const char*,...) {}
extern "C" void _sys_err(const char* function,int line,const char*,...) {
    assert(!std::strcmp(function,"event_process") && line==211); ++errors;
}
void ShutdownOnFatalError() { std::abort(); }
extern std::map<int,LPEVENTINFO> g_event_info;
class Probe : public CHorseRider {
public:
    int infos,clears,ride_time,starts,stops,deaths;
    Probe():infos(0),clears(0),ride_time(0),starts(0),stops(0),deaths(0) {}
    using CHorseRider::GetHorseData;
    using CHorseRider::SetHorseData;
    using CHorseRider::UpdateHorseDataByLogoff;
    virtual void SendHorseInfo() { ++infos; }
    virtual void ClearHorseInfo() { ++clears; }
    virtual void UpdateRideTime(int seconds) { ride_time+=seconds; }
    virtual bool StartRiding() { ++starts; return CHorseRider::StartRiding(); }
    virtual bool StopRiding() { ++stops; return CHorseRider::StopRiding(); }
    virtual void HorseDie() { ++deaths; CHorseRider::HorseDie(); }
    DWORD Vnum() { return GetMyHorseVnum(); }
};
static int Advance(int pulses) { heart.pulse+=pulses; return event_process(heart.pulse); }
static THorseInfo Info(BYTE level,BYTE riding,short stamina,short health,DWORD drop) {
    THorseInfo info; info.bLevel=level; info.bRiding=riding; info.sStamina=stamina; info.sHealth=health; info.dwHorseHealthDropTime=drop; return info;
}
static void Cancel(Probe& rider) {
    rider.Destroy(); assert(!rider.m_eventStaminaConsume && !rider.m_eventStaminaRegen);
    assert(Advance(1000000)==0 && event_count()==0 && g_event_info.empty());
}
int main() {
    assert(passes_per_sec==25);
    {
        Probe rider;
        assert(!rider.HasHorse() && !rider.GetHorseLevel() && !rider.GetHorseHealth() && !rider.GetHorseStamina() && !rider.IsHorseRiding());
        assert(rider.GetHorseMaxHealth()==1 && rider.GetHorseMaxStamina()==1 && rider.Vnum()==20030 && rider.GetHorseGrade()==0);
        assert(!rider.StartRiding() && !rider.StopRiding() && !rider.ReviveHorse());
        rider.SetHorseLevel(-1); assert(!rider.HasHorse() && rider.GetHorseHealth()==1 && rider.GetHorseStamina()==1);
        rider.SetHorseLevel(99); assert(rider.GetHorseLevel()==30 && rider.GetHorseHealth()==50 && rider.GetHorseStamina()==200);
        assert(rider.GetHorseST()==67 && rider.GetHorseDX()==89 && rider.GetHorseHT()==45 && rider.GetHorseIQ()==22 && rider.GetHorseArmor()==59);
        const BYTE levels[]={0,1,10,11,20,21,30,255},grades[]={0,1,1,2,2,3,3,26};
        for(unsigned i=0;i<sizeof levels;++i) { rider.SetHorseData(Info(levels[i],0,0,0,0)); assert(rider.GetHorseGrade()==grades[i]); }
        THorseInfo raw=Info(1,2,-7,-3,0x12345678UL); rider.SetHorseData(raw);
        assert(!std::memcmp(&raw,&rider.GetHorseData(),sizeof raw) && rider.IsHorseRiding());
        rider.SetHorseData(Info(1,0,1,3,static_cast<DWORD>(now)+259200)); rider.infos=0;
        rider.UpdateHorseDataByLogoff(719); assert(rider.GetHorseStamina()==1);
        rider.UpdateHorseDataByLogoff(720); assert(rider.GetHorseStamina()==2);
        rider.UpdateHorseDataByLogoff(0xffffffffUL); assert(rider.GetHorseStamina()==4 && !rider.infos);
        rider.SetHorseData(Info(1,0,0,3,static_cast<DWORD>(now)+259200)); assert(!rider.StartRiding());
        rider.SetHorseData(Info(1,0,4,0,static_cast<DWORD>(now)+259200)); assert(!rider.StartRiding());
        rider.SetHorseLevel(1); rider.infos=rider.ride_time=0;
        assert(rider.StartRiding() && !rider.StartRiding() && rider.IsHorseRiding());
        assert(rider.m_eventStaminaConsume && !rider.m_eventStaminaRegen && event_time(rider.m_eventStaminaConsume)==9000);
        assert(!std::strcmp(rider.m_eventStaminaConsume->file,"horse_rider.cpp") && rider.m_eventStaminaConsume->line==283);
        for(int i=0;i<4;++i) { assert(Advance(9000)==1); assert(rider.GetHorseStamina()==3-i); }
        assert(!rider.IsHorseRiding() && !rider.m_eventStaminaConsume && rider.m_eventStaminaRegen && rider.ride_time==1440);
        assert(event_time(rider.m_eventStaminaRegen)==18000 && rider.m_eventStaminaRegen->line==304);
        for(int i=1;i<=4;++i) { assert(Advance(18000)==1); assert(rider.GetHorseStamina()==i); }
        assert(!rider.m_eventStaminaRegen && !event_count() && g_event_info.empty());
        rider.UpdateHorseHealth(-3,false); assert(rider.GetHorseHealth()==0 && rider.GetHorseStamina()==0 && rider.deaths==1);
        int sent=rider.infos; assert(rider.ReviveHorse() && rider.GetHorseHealth()==3 && rider.GetHorseStamina()==4 && rider.infos==sent);
        assert(!rider.ReviveHorse() && Advance(18000)==1 && !rider.m_eventStaminaRegen);
        rider.FeedHorse(); assert(rider.GetHorseHealth()==3 && rider.GetHorseData().dwHorseHealthDropTime==static_cast<DWORD>(now)+259200);
        rider.SetHorseData(Info(1,1,4,3,static_cast<DWORD>(now)+259200)); int starts=rider.starts;
        rider.EnterHorse(); assert(rider.starts==starts+1 && rider.IsHorseRiding() && rider.m_eventStaminaConsume); Cancel(rider);
        rider.SetHorseData(Info(1,0,4,3,static_cast<DWORD>(now)));
        rider.EnterHorse(); assert(rider.GetHorseHealth()==3); Cancel(rider);
        rider.CheckHorseHealthDropTime(false); assert(rider.GetHorseHealth()==3); // Exact equality does not drop health.
        ++now; rider.CheckHorseHealthDropTime(false); assert(rider.GetHorseHealth()==2); --now;
        rider.SetHorseData(Info(1,0,1,1,static_cast<DWORD>(now))); rider.deaths=rider.infos=0;
        now+=2*259200+1; rider.CheckHorseHealthDropTime(false);
        assert(rider.GetHorseHealth()==0 && rider.GetHorseStamina()==0 && rider.deaths==3 && rider.infos==3); // Catch-up continues after death.
        now=1000000;
        rider.SetHorseData(Info(1,0,3,1,static_cast<DWORD>(now)-1)); rider.deaths=0;
        assert(rider.StartRiding()); int previous_errors=errors;
        assert(Advance(9000)==1 && rider.deaths==1 && !rider.IsHorseRiding() && !rider.GetHorseHealth() && !rider.GetHorseStamina());
        assert(errors==previous_errors+1 && !rider.m_eventStaminaConsume && !rider.m_eventStaminaRegen && !event_count());
        Cancel(rider);
        rider.SetHorseData(Info(0,1,1,1,99)); rider.SetHorseLevel(0);
        assert(rider.IsHorseRiding() && !rider.HasHorse()); // Level setter preserves the riding flag.
    }
    assert(!event_count() && g_event_info.empty()); event_destroy();
    std::puts("horse riding passed: exact stats, transitions, real timer queue, offline recovery, hunger and cancellation ordering");
}

// Unused OS boundaries from the real utility object; horse code calls only MINMAX.
extern "C" int select(int,fd_set*,fd_set*,fd_set*,struct timeval*) { std::abort(); }
extern "C" int fork() { std::abort(); }
extern "C" long random() { std::abort(); }
extern "C" { void* (*thecore_memcpy)(void*,const void*,size_t)=std::memcpy; }
