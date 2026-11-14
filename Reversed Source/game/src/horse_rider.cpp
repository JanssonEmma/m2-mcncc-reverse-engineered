// Recovered old horse lifecycle, stats and event callbacks. Native target: x86 FreeBSD.
#include "horse_rider.h"
#include "utils.h"
#include "config.h"
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"
#include <cstdint>
#include <cstring>
const int HORSE_HEALTH_DROP_INTERVAL=259200;
const int HORSE_STAMINA_CONSUME_INTERVAL=360;
const int HORSE_STAMINA_REGEN_INTERVAL=720;
THorseStat c_aHorseStat[HORSE_MAX_LEVEL+1]={
    {0,0,1,1,0,0,0,0,0,0,0,0},
    {25,20101,3,4,26,35,18,9,54,43,64,32},
    {25,20101,4,4,27,36,18,9,55,44,66,33},
    {25,20101,5,5,28,38,19,9,56,44,67,33},
    {25,20101,7,5,29,39,19,10,57,45,68,34},
    {25,20101,8,6,30,40,20,10,58,46,69,34},
    {25,20101,9,6,31,41,21,10,59,47,70,35},
    {25,20101,11,7,32,42,21,11,60,48,72,36},
    {25,20101,12,7,33,44,22,11,61,48,73,36},
    {25,20101,13,8,34,45,22,11,62,49,74,37},
    {25,20101,15,10,35,46,23,12,63,50,75,37},
    {35,20104,18,30,40,53,27,13,69,55,82,41},
    {35,20104,19,35,41,54,27,14,70,56,84,42},
    {35,20104,21,40,42,56,28,14,71,56,85,42},
    {35,20104,22,50,43,57,28,14,72,57,86,43},
    {35,20104,24,55,44,58,29,15,73,58,87,43},
    {35,20104,25,60,44,59,30,15,74,59,88,44},
    {35,20104,27,65,45,60,30,15,75,60,90,45},
    {35,20104,28,70,46,62,31,15,76,60,91,45},
    {35,20104,30,80,47,63,31,16,77,61,92,46},
    {35,20104,32,100,48,64,32,16,78,62,93,46},
    {50,20107,35,120,53,71,36,18,84,67,100,50},
    {50,20107,36,125,55,74,37,18,86,68,103,51},
    {50,20107,37,130,57,76,38,19,88,70,105,52},
    {50,20107,38,135,59,78,39,20,90,72,108,54},
    {50,20107,40,140,60,80,40,20,91,72,109,54},
    {50,20107,42,145,61,81,40,20,92,73,110,55},
    {50,20107,44,150,62,83,42,21,94,75,112,56},
    {50,20107,46,160,63,84,42,21,95,76,114,57},
    {50,20107,48,170,65,87,43,22,97,77,116,58},
    {50,20107,50,200,67,89,45,22,99,79,118,59},
};
struct horserider_info { CHorseRider* hr; };
CHorseRider::CHorseRider() { Initialize(); }
CHorseRider::~CHorseRider() { Destroy(); }
void CHorseRider::Initialize() {
    m_eventStaminaRegen=0; m_eventStaminaConsume=0;
    std::memset(&m_Horse,0,sizeof m_Horse);
}
void CHorseRider::Destroy() { event_cancel(&m_eventStaminaRegen); event_cancel(&m_eventStaminaConsume); }
short CHorseRider::GetHorseMaxHealth() { return static_cast<short>(c_aHorseStat[m_Horse.bLevel].iMaxHealth); }
short CHorseRider::GetHorseMaxStamina() { return static_cast<short>(c_aHorseStat[m_Horse.bLevel].iMaxStamina); }
BYTE CHorseRider::GetHorseGrade()
{
    const BYTE level = m_Horse.bLevel;
    return level ? static_cast<BYTE>((level - 1) / 10 + 1) : 0;
}
void CHorseRider::SetHorseData(const THorseInfo& info) { m_Horse=info; }
void CHorseRider::UpdateHorseDataByLogoff(DWORD seconds) {
    const auto elapsed = static_cast<std::uint32_t>(seconds);
    if(m_Horse.bLevel && elapsed>=720) UpdateHorseStamina(static_cast<int>(elapsed/720),false);
}
void CHorseRider::UpdateHorseStamina(int amount,bool send) {
    const int maximum = c_aHorseStat[m_Horse.bLevel].iMaxStamina;
    int total=static_cast<int>(static_cast<unsigned int>(m_Horse.sStamina)+static_cast<unsigned int>(amount));
    m_Horse.sStamina = static_cast<short>(MINMAX(0, total, maximum));
    if(!m_Horse.sStamina && m_Horse.bRiding) StopRiding();
    if(send) SendHorseInfo();
}
void CHorseRider::UpdateHorseHealth(int amount,bool send) {
    BYTE level=m_Horse.bLevel;
    const int maximum = c_aHorseStat[level].iMaxHealth;
    int total=static_cast<int>(static_cast<unsigned int>(m_Horse.sHealth)+static_cast<unsigned int>(amount));
    m_Horse.sHealth=static_cast<short>(MINMAX(0,total,maximum));
    if(level && !m_Horse.sHealth) HorseDie();
    if(send) SendHorseInfo();
}
void CHorseRider::ResetHorseHealthDropTime() {
    m_Horse.dwHorseHealthDropTime = static_cast<std::uint32_t>(get_global_time()) + HORSE_HEALTH_DROP_INTERVAL;
}
void CHorseRider::CheckHorseHealthDropTime(bool send) {
    const auto now = static_cast<std::uint32_t>(get_global_time());
    while(m_Horse.dwHorseHealthDropTime<now) { m_Horse.dwHorseHealthDropTime+=259200UL; UpdateHorseHealth(-1,send); }
}
void CHorseRider::SetHorseLevel(int level) {
    level = MINMAX(0, level, 30);
    m_Horse.bLevel = static_cast<BYTE>(level);
    const short stamina = static_cast<short>(c_aHorseStat[level].iMaxStamina);
    const short health = static_cast<short>(c_aHorseStat[level].iMaxHealth);
    m_Horse.dwHorseHealthDropTime = 0;
    m_Horse.sStamina = stamina;
    m_Horse.sHealth = health;
    ResetHorseHealthDropTime();
    SendHorseInfo();
}
bool CHorseRider::ReviveHorse() {
    const BYTE level = m_Horse.bLevel;
    if (!level || m_Horse.sHealth > 0)
        return false;
    const short health = static_cast<short>(c_aHorseStat[level].iMaxHealth);
    const short stamina = static_cast<short>(c_aHorseStat[level].iMaxStamina);
    m_Horse.sHealth = health;
    m_Horse.sStamina = stamina;
    ResetHorseHealthDropTime(); StartStaminaRegenEvent(); return true;
}
void CHorseRider::FeedHorse() {
    if(m_Horse.bLevel && m_Horse.sHealth>0) { UpdateHorseHealth(1,true); ResetHorseHealthDropTime(); }
}
void CHorseRider::EnterHorse() {
    if(m_Horse.bLevel && m_Horse.sHealth>0) {
        if(m_Horse.bRiding) { m_Horse.bRiding=0; StartRiding(); }
        else StartStaminaRegenEvent();
        CheckHorseHealthDropTime(false);
    }
}
bool CHorseRider::StartRiding() {
    if(m_Horse.bRiding || !m_Horse.bLevel || m_Horse.sHealth<=0 || m_Horse.sStamina<=0) return false;
    m_Horse.bRiding=1; StartStaminaConsumeEvent(); SendHorseInfo(); return true;
}
bool CHorseRider::StopRiding() {
    if(!m_Horse.bRiding) return false;
    m_Horse.bRiding=0; StartStaminaRegenEvent(); return true;
}
void CHorseRider::HorseDie() {
    const auto consume = m_eventStaminaConsume;
    const auto regen = m_eventStaminaRegen;
    sys_log(0, "HORSE DIE %p %p", static_cast<void*>(regen), static_cast<void*>(consume));
    UpdateHorseStamina(-static_cast<int>(m_Horse.sStamina),true);
    event_cancel(&m_eventStaminaRegen); event_cancel(&m_eventStaminaConsume);
}
// These empty virtual hooks and default race are actual target bodies.
void CHorseRider::SendHorseInfo() {}
void CHorseRider::ClearHorseInfo() {}
void CHorseRider::UpdateRideTime(int) {}
DWORD CHorseRider::GetMyHorseVnum() { return 20030; }
void CHorseRider::StartStaminaConsumeEvent() {
    if(!m_Horse.bLevel || m_Horse.sHealth<=0) return;
    sys_log(0,"HORSE STAMINA REGEN EVENT CANCEL %p",m_eventStaminaRegen);
    event_cancel(&m_eventStaminaRegen);
    if(!m_eventStaminaConsume) {
        void* data=0; DWORD id=__AllocEventInfo(sizeof(horserider_info),&data);
        static_cast<horserider_info*>(data)->hr=this;
        int delay=static_cast<int>(static_cast<unsigned int>(passes_per_sec)*360U);
        m_eventStaminaConsume=event_create_ex(horse_stamina_consume_event,id,delay,const_cast<char*>("horse_rider.cpp"),283);
        sys_log(0,"HORSE STAMINA CONSUME EVENT CREATE %p",m_eventStaminaConsume);
    }
}
void CHorseRider::StartStaminaRegenEvent() {
    if(!m_Horse.bLevel || m_Horse.sHealth<=0) return;
    sys_log(0,"HORSE STAMINA CONSUME EVENT CANCEL %p",m_eventStaminaConsume);
    event_cancel(&m_eventStaminaConsume);
    if(!m_eventStaminaRegen) {
        void* data=0; DWORD id=__AllocEventInfo(sizeof(horserider_info),&data);
        static_cast<horserider_info*>(data)->hr=this;
        int delay=static_cast<int>(static_cast<unsigned int>(passes_per_sec)*720U);
        m_eventStaminaRegen=event_create_ex(horse_stamina_regen_event,id,delay,const_cast<char*>("horse_rider.cpp"),304);
        sys_log(0,"HORSE STAMINA REGEN EVENT CREATE %p",m_eventStaminaRegen);
    }
}
long horse_stamina_consume_event(LPEVENT event) {
    CHorseRider* rider=static_cast<horserider_info*>(event->info)->hr;
    if(rider->m_Horse.sHealth<=0) { rider->m_eventStaminaConsume=0; return 0; }
    rider->UpdateHorseStamina(-1,true); rider->UpdateRideTime(360);
    const short stamina = rider->m_Horse.sStamina;
    long next=static_cast<int>(static_cast<unsigned int>(passes_per_sec)*360U);
    if(!stamina) { rider->m_eventStaminaConsume=0; next=0; }
    rider->CheckHorseHealthDropTime(true); sys_log(0,"HORSE STAMINA - %p",event); return next;
}
long horse_stamina_regen_event(LPEVENT event) {
    CHorseRider* rider=static_cast<horserider_info*>(event->info)->hr;
    if(rider->m_Horse.sHealth<=0) { rider->m_eventStaminaRegen=0; return 0; }
    rider->UpdateHorseStamina(1,true);
    const short stamina = rider->m_Horse.sStamina;
    const auto pulseRate = static_cast<std::uint32_t>(passes_per_sec);
    const short maximum = rider->GetHorseMaxStamina();
    long next = static_cast<std::int32_t>(pulseRate * 720u);
    if(stamina==maximum) { rider->m_eventStaminaRegen=0; next=0; }
    rider->CheckHorseHealthDropTime(true); sys_log(0,"HORSE STAMINA + %p",event); return next;
}
