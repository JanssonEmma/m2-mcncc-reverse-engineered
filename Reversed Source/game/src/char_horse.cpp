#include "char_manager.h"
#include "packet.h"
#include "vector.h"
#include "locale.hpp"
#include "config.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include "horse-summon-detail.h"
#include "questmanager.h"

#include "char.h"
#include "guild.h"
#include "item.h"
#include "skill.h"
bool CHARACTER::ReviveHorse()
{
    if (!CHorseRider::ReviveHorse())
        return false;

    HorseSummon(false, false);
    HorseSummon(true, false);
    return true;
}

DWORD CHARACTER::GetMyHorseVnum() {
    int delta=0;
    if(m_pGuild) delta=m_pGuild->GetMasterPID()==m_dwPlayerID?2:1;
    return static_cast<DWORD>(static_cast<unsigned int>(c_aHorseStat[GetHorseLevel()].iNPCRace)+
                              static_cast<unsigned int>(delta));
}

void CHARACTER::ClearHorseInfo() {
    if(!IsHorseRiding()) {
        ChatPacket(5,"hide_horse_state");
        m_bSendHorseLevel=0;m_bSendHorseHealthGrade=0;m_bSendHorseStaminaGrade=0;
    }
    m_chHorse=0;
}

void CHARACTER::SendHorseInfo() {
    if(!m_chHorse && !IsHorseRiding()) return;
    short health=GetHorseHealth();
    int healthGrade=0;
    if(health) {
        short maximum=GetHorseMaxHealth();
        healthGrade=1;
        if(maximum*3<health*10) {
            health=GetHorseHealth();maximum=GetHorseMaxHealth();
            healthGrade=2+(maximum*7<health*10);
        }
    }
    short stamina=GetHorseStamina();
    unsigned int staminaGrade=0;
    short maximum=GetHorseMaxStamina();
    if(maximum<stamina*10) {
        stamina=GetHorseStamina();staminaGrade=1;maximum=GetHorseMaxStamina();
        if(maximum*3<stamina*10) {
            stamina=GetHorseStamina();maximum=GetHorseMaxStamina();
            staminaGrade=2+(maximum*7<stamina*10);
        }
    }
    if(GetHorseLevel()!=m_bSendHorseLevel || m_bSendHorseHealthGrade!=healthGrade ||
       m_bSendHorseStaminaGrade!=staminaGrade) {
        ChatPacket(5,"horse_state %d %d %d",static_cast<unsigned int>(GetHorseLevel()),
                   healthGrade,staminaGrade);
        m_bSendHorseLevel=GetHorseLevel();
        m_bSendHorseHealthGrade=static_cast<BYTE>(healthGrade);
        m_bSendHorseStaminaGrade=static_cast<BYTE>(staminaGrade);
    }
}

void CHARACTER::SetRider(LPCHARACTER rider) {
    CHARACTER* previous=m_chRider;
    if(previous) previous->ClearHorseInfo();
    m_chRider=rider;
    if(rider) rider->SendHorseInfo();
}

//0816d720: genuine unresolved quest/horse runtime dependencies retained.
bool CHARACTER::IsHorseVisible()
{
    return m_chHorse != nullptr;
}

bool CHARACTER::StartRiding()
{
    const char* notice;
    if (IsPolymorphed()) {
        notice = "\272\257\275\305\040\273\363\305\302\277\241\274\255\264\302\040\270\273\277\241\040\305\273\040\274\366\040\276\370\275\300\264\317\264\331\056";
    } else {
        CItem* armor = GetWear(0);
        const DWORD vnum = armor ? armor->GetVnum() : 0;
        if (vnum >= 11901 && vnum <= 11904) {
            notice = "\277\271\272\271\300\273\040\300\324\300\272\040\273\363\305\302\277\241\274\255\040\270\273\300\273\040\305\273\040\274\366\040\276\370\275\300\264\317\264\331\056";
        } else if (CHorseRider::StartRiding()) {
            HorseSummon(false, false);
            MountVnum(GetMyHorseVnum());
            return true;
        } else if (!GetHorseLevel()) {
            notice = "\270\273\300\273\040\274\322\300\257\307\317\260\355\040\300\326\301\366\040\276\312\275\300\264\317\264\331\056";
        } else if (GetHorseHealth() < 1) {
            notice = "\270\273\300\314\040\301\327\276\356\300\326\264\302\040\273\363\305\302\040\300\324\264\317\264\331\056";
        } else if (GetHorseStamina() <= 0) {
            notice = "\270\273\300\307\040\275\272\305\327\271\314\263\312\260\241\040\272\316\301\267\307\317\277\251\040\270\273\300\273\040\305\273\040\274\366\040\276\370\275\300\264\317\264\331\056";
        } else {
            return false;
        }
    }
    ChatPacket(1, "%s", locale_find(notice, g_iUseLocale));
    return false;
}

bool CHARACTER::StopRiding() {
    if(!CHorseRider::StopRiding()) return false;
    quest::CQuestManager::instance().Unmount(static_cast<unsigned int>(m_dwPlayerID));
    if(IsDead() || IsStun()) {
        m_dwMountVnum=0;
        ComputePoints();
        UpdatePacket();
    } else {
        MountVnum(0);
        HorseSummon(true,false);
    }
    PointChange(12,0,false,false);
    PointChange(14,0,false,false);
    PointChange(13,0,false,false);
    PointChange(15,0,false,false);
    return true;
}

// 0816d940
struct char_event_info { CHARACTER* ch; }; // Full original record4 at target ABI.
long horse_dead_event(LPEVENT current) {
    //0816d890: original has no null guard/dynamic_cast.
    static_cast<char_event_info*>(current->info)->ch->HorseSummon(false,false);
    return 0;
}
void CHARACTER::HorseSummon(bool summon,bool fromFar) {
    using namespace horse_summon_detail;
    if(summon) {
        if(!GetHorseLevel() || IsHorseRiding() || m_chHorse) return;
        long startX=GetX(),startY=GetY();
        if(GetHorseHealth()<1) fromFar=false;
        int dx,dy;
        if(!fromFar) {
            dx=number_ex(-100,100,"char_horse.cpp",0x87);
            dy=number_ex(-100,100,"char_horse.cpp",0x88);
        } else {
            int direction=number_ex(0,1,"char_horse.cpp",0x82);
            int distance=number_ex(2000,2500,"char_horse.cpp",0x82);
            dx=Mul(direction*2-1,distance);
            direction=number_ex(0,1,"char_horse.cpp",0x83);
            distance=number_ex(2000,2500,"char_horse.cpp",0x83);
            dy=Mul(direction*2-1,distance);
        }
        long x=Add(startX,dx),y=Add(startY,dy);
        int rotation=Trunc32(180.0L+static_cast<long double>(m_pointsInstant.fRot));
        long z=GetZ(),map=GetMapIndex(); // Captured before virtual race getter.
        DWORD race=GetMyHorseVnum(); // Primary vptr slot14,0816df10.
        m_chHorse=CHARACTER_MANAGER::instance().SpawnMob(race,map,x,y,z,false,rotation,false);
        if(!m_chHorse) {
            ChatPacket(1,LC_TEXT("\270\273\040\274\322\310\257\277\241\040\275\307\306\320\307\317\277\264\275\300\264\317\264\331\056"));
            return;
        }
        if(GetHorseHealth()<1) {
            m_chHorse->SetPosition(0);
            void* raw=0;DWORD id=__AllocEventInfo(sizeof(char_event_info),&raw);
            int rate=passes_per_sec;
            static_cast<char_event_info*>(raw)->ch=this;
            CHARACTER* horse=m_chHorse;
            horse->m_pkDeadEvent=event_create_ex(horse_dead_event,id,Mul(rate,60),
                                               const_cast<char*>("char_horse.cpp"),0x9f);
        }
        m_chHorse->SetLevel(GetHorseLevel());
        CHARACTER* named=m_chHorse;
        const char* name=GetName();
        named->m_stName.assign(name);
        named=m_chHorse;
        const char* suffix=LC_TEXT("\264\324\300\307\040\270\273");
        named->m_stName.append(suffix);
        if(m_chHorse->Show(GetMapIndex(),x,y,GetZ(),false)) {
            if(GetHorseHealth()<1) {
                TPacketGCDead packet;
                packet.header=14;
                packet.vid=m_chHorse->GetVID();
                PacketAround(&packet,5,0); // Owner's view, not horse's view.
            }
            m_chHorse->SetRider(this);
        } else {
            CHARACTER_MANAGER::instance().DestroyCharacter(m_chHorse);
            // Assembly stores only function,line,format: decompiler's y is stale.
            _sys_err("HorseSummon",0xa9,"cannot show monster");
            m_chHorse=0;
        }
    } else {
        CHARACTER* horse=m_chHorse;
        if(!horse) return;
        horse->SetRider(0); // Real ClearHorseInfo normally clears owner's m_chHorse.
        if(fromFar) {
            horse->SetNowWalking(false);
            float degree=GetDegreeFromPositionXY(horse->GetX(),horse->GetY(),GetX(),GetY());
            horse->SetRotation(degree+180.0f);
            float dx,dy;
            GetDeltaByDegree(horse->m_pointsInstant.fRot,3500.0f,&dx,&dy);
            int x=Trunc32(static_cast<long double>(horse->GetX())+dx);
            int y=Trunc32(static_cast<long double>(horse->GetY())+dy);
            horse->Goto(x,y);
            horse->SendMovePacket(0,0,0,0,0,0,-1); // Sent regardless of Goto result.
        } else CHARACTER_MANAGER::instance().DestroyCharacter(horse);
    }
}

LPCHARACTER CHARACTER::GetRider()
{
    return m_chRider;
}

// 0816df60
void CHARACTER::HorseDie()
{
    CHorseRider::HorseDie();
    HorseSummon(false, false);
}

// 0816e1f0
void CHARACTER::SetHorseLevel(int level)
{
    CHorseRider::SetHorseLevel(level);
    SetSkillLevel(SKILL_HORSE, GetHorseLevel());
}
