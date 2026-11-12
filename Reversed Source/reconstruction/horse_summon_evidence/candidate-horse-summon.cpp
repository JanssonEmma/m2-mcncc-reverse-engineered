//0816d940 complete old two-bool HorseSummon; no modern pet/name API.
#include "char.h"
#include "char_manager.h"
#include "packet.h"
#include "vector.h"
#include "locale.hpp"
#include "config.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include "horse-summon-detail.h"
struct char_event_info { CHARACTER* ch; }; // Full original record4 at target ABI.
long horse_dead_event(LPEVENT current) {
    //0816d890: original has no null guard/dynamic_cast.
    static_cast<char_event_info*>(current->info)->ch->HorseSummon(false,false);
    return 0;
}
void CHARACTER::HorseSummon(bool summon,bool fromFar) {
    using namespace horse_summon_detail;
    if(summon) {
        if(!m_Horse.bLevel || m_Horse.bRiding || m_chHorse) return;
        long startX=GetX(),startY=GetY();
        if(m_Horse.sHealth<1) fromFar=false;
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
        if(m_Horse.sHealth<1) {
            m_chHorse->SetPosition(0);
            void* raw=0;DWORD id=__AllocEventInfo(sizeof(char_event_info),&raw);
            int rate=passes_per_sec;
            static_cast<char_event_info*>(raw)->ch=this;
            CHARACTER* horse=m_chHorse;
            horse->m_pkDeadEvent=event_create_ex(horse_dead_event,id,Mul(rate,60),
                                               const_cast<char*>("char_horse.cpp"),0x9f);
        }
        m_chHorse->SetLevel(m_Horse.bLevel);
        CHARACTER* named=m_chHorse;
        const char* name=GetName();
        named->m_stName.assign(name);
        named=m_chHorse;
        const char* suffix=LC_TEXT("\264\324\300\307\040\270\273");
        named->m_stName.append(suffix);
        if(m_chHorse->Show(GetMapIndex(),x,y,GetZ(),false)) {
            if(m_Horse.sHealth<1) {
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
