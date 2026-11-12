
#include "char.h"
#include "guild.h"
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

