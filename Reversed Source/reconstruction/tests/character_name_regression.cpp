#include <map>
#include <set>
#include <string>
#include <ext/hash_map>
#define private public
#include "../../game/src/char_manager.h"
#include "../../game/src/questmanager.h"
#undef private
#include "character_fixture.h"
#include "../../game/src/crc32.h"
#include "../../libthecore/include/main.h"
#include <string>
#include <cassert>
#include <cstring>
#include <cstdio>
extern unsigned isolated_race_registration_logs;
extern unsigned isolated_monarch_logs,isolated_pulse_reads;
extern DWORD isolated_dword_clock;
extern int passes_per_sec,test_server;
static unsigned deleted_characters=0;
class CleanupProbe : public CharacterProbe {
public:
    ~CleanupProbe() {
        CHARACTER_MANAGER& manager=CHARACTER_MANAGER::instance();
        assert(!manager.Find(static_cast<DWORD>(GetVID())));
        assert(!manager.FindPC(GetName()));
        if(GetPlayerID()) assert(!manager.FindByPID(GetPlayerID()));
        assert(!manager.m_set_pkChrState.count(this));
        assert(!manager.m_map_pkChrByRaceNum[GetRaceNum()].count(this));
        ++deleted_characters;
    }
};
int main() {
    CHARACTER_MANAGER manager;
    quest::CQuestManager quest_manager;
    CharacterProbe character;
    assert(!std::strcmp(character.GetName(),""));
    CMob mob;
    std::strcpy(mob.m_table.szLocaleName,"Localized mob");
    character.Mob(&mob); assert(!std::strcmp(character.GetName(),"Localized mob"));
    character.Name("Player name"); assert(!std::strcmp(character.GetName(),"Player name"));
    character.Name(""); assert(!std::strcmp(character.GetName(),"Localized mob"));
    character.Mob(0); assert(!std::strcmp(character.GetName(),""));
    character.Race(7,0); assert(character.GetRaceNum()==7);
    mob.m_table.dwVnum=0x12345678UL; character.Mob(&mob);
    assert(character.GetRaceNum()==0x5678);
    character.Race(9,0xabcdef01UL); assert(character.GetRaceNum()==0xef01);
    character.Mob(0); assert(character.GetRaceNum()==0xef01);
    character.Race(255,0); assert(character.GetRaceNum()==255);
    character.Race(3,0x10000UL); assert(character.GetRaceNum()==0);
    CharacterProbe peer; peer.Race(1,0); peer.Name("peer");
    manager.RegisterRaceNumMap(&peer);
    assert(manager.m_map_pkChrByRaceNum.empty() && isolated_race_registration_logs==0);
    peer.Race(1,9004); manager.RegisterRaceNumMap(&peer); manager.RegisterRaceNumMap(&peer);
    assert(isolated_race_registration_logs==2 && manager.m_map_pkChrByRaceNum[9004].size()==1);
    peer.Race(1,20031); manager.UnregisterRaceNumMap(&peer);
    assert(manager.m_map_pkChrByRaceNum.size()==1 && manager.m_map_pkChrByRaceNum[9004].count(&peer));
    peer.Race(1,9004); manager.UnregisterRaceNumMap(&peer); manager.UnregisterRaceNumMap(&peer);
    assert(manager.m_map_pkChrByRaceNum.count(9004) && manager.m_map_pkChrByRaceNum[9004].empty());
    peer.Race(1,0x10000UL+9004); manager.RegisterRaceNumMap(&peer);
    assert(manager.m_map_pkChrByRaceNum[9004].count(&peer)); manager.UnregisterRaceNumMap(&peer);

    CleanupProbe* doomed=new CleanupProbe;
    doomed->Identity(0x80000001UL,17); doomed->PlayerID(0xffffffffUL);
    doomed->Name("ABCDEFGHIJKLMNOPQRSTUVWXignored"); doomed->Race(2,9004);
    //Even mismatched pointers are removed by matching keys in the original routine.
    manager.m_map_pkChrByVID[0x80000001UL]=&peer;
    manager.m_map_pkChrByPID[0xffffffffUL]=&peer;
    manager.m_map_pkPCChr["abcdefghijklmnopqrstuvwx"]=&peer;
    manager.AddToStateList(doomed); manager.RegisterRaceNumMap(doomed);
    manager.DelayedSave(doomed); manager.RegisterForMonsterLog(doomed);
    manager.SelectStone(doomed);
    manager.DestroyCharacter(doomed);
    assert(deleted_characters==1 && manager.m_set_pkChrForDelayedSave.count(doomed));
    assert(manager.m_set_pkChrMonsterLog.count(doomed) && manager.m_pkChrSelectedStone==doomed);
    manager.m_set_pkChrForDelayedSave.clear(); manager.m_set_pkChrMonsterLog.clear(); manager.SelectStone(0);
    //PID zero is deliberately exempt from PID registry erasure.
    CleanupProbe* zero=new CleanupProbe; zero->Identity(0,0); zero->PlayerID(0); zero->Name(""); zero->Race(0,0);
    manager.m_map_pkChrByPID[0]=&peer; manager.DestroyCharacter(zero);
    assert(deleted_characters==2 && manager.FindByPID(0)==&peer);
    char expected_identity[129];
    character.Name("retained");
    character.Create("identity",123,false);
    std::snprintf(expected_identity,128,"identity%p172815",static_cast<void*>(&character));
    assert(character.GetVID()==VID(123,get_crc32(expected_identity,std::strlen(expected_identity))));
    assert(!std::strcmp(character.GetName(),"retained"));
    VID old_identity=character.GetVID();
    character.Create("identity",123,true);
    std::snprintf(expected_identity,128,"identity%p172816",static_cast<void*>(&character));
    assert(character.GetVID()==VID(123,get_crc32(expected_identity,std::strlen(expected_identity))));
    assert(character.GetVID()!=old_identity && !std::strcmp(character.GetName(),"identity"));
    std::string long_name(200,'Q'); character.Create(long_name.c_str(),0xffffffffUL,true);
    std::string truncated_name(127,'Q');
    assert(character.GetVID()==VID(0xffffffffUL,get_crc32(truncated_name.c_str(),127)));
    assert(!std::strcmp(character.GetName(),long_name.c_str()));
    //The counter is shared between characters, including NPC-name calls.
    peer.Create("other",99,false);
    std::snprintf(expected_identity,128,"other%p172818",static_cast<void*>(&peer));
    assert(peer.GetVID()==VID(99,get_crc32(expected_identity,std::strlen(expected_identity))));
    assert(!std::strcmp(peer.GetName(),"peer"));
    isolated_dword_clock=0xffffffffUL; character.ResetStopTime();
    isolated_dword_clock=17; assert(character.GetStopTime()==0xffffffffUL);
    character.ResetStopTime(); assert(character.GetStopTime()==17);
    passes_per_sec=25; thecore_heart->pulse=1000; isolated_pulse_reads=0;
    character.InitMC(); assert(isolated_pulse_reads==5);
    const DWORD limits[]={250,1500,1500,15120000UL,90000};
    for(int i=0;i<MI_MAX;++i) {
        MONARCH_INDEX index=static_cast<MONARCH_INDEX>(i);
        assert(character.GetMCL(index)==limits[i]);
        assert(character.GetMC(index)==static_cast<DWORD>(1000)-limits[i]*25UL);
        assert(character.IsMCOK(index));
    }
    assert(character.GetMCLTime(MI_HEAL)==static_cast<DWORD>(10UL-250UL));
    character.SetMC(MI_WARP); assert(character.GetMC(MI_WARP)==1000);
    thecore_heart->pulse=2499; assert(!character.IsMCOK(MI_WARP) && character.GetMCLTime(MI_WARP)==1);
    thecore_heart->pulse=2500; assert(character.IsMCOK(MI_WARP) && character.GetMCLTime(MI_WARP)==0);
    thecore_heart->pulse=2525; assert(character.IsMCOK(MI_WARP) && character.GetMCLTime(MI_WARP)==0xffffffffUL);
    character.m_dwMonarchCooltime[MI_HEAL]=0xfffffff0UL;
    thecore_heart->pulse=32; assert(!character.IsMCOK(MI_HEAL));
    character.m_dwMonarchCooltimelimit[MI_HEAL]=48; assert(character.IsMCOK(MI_HEAL));
    test_server=1; unsigned before_logs=isolated_monarch_logs;
    character.IsMCOK(MI_HEAL); character.GetMCLTime(MI_HEAL);
    assert(isolated_monarch_logs==before_logs+2); test_server=0;
    passes_per_sec=1; thecore_heart->pulse=7; character.InitMC();
    assert(character.GetMC(MI_TAX)==static_cast<DWORD>(7UL-604800UL));
    assert(character.GetMCL(MI_TAX)==604800UL && character.IsMCOK(MI_TAX));
    passes_per_sec=25;
    quest_manager.m_mapEventFlag["threeway_war_dead_count"]=8;
    character.ClearKillCount(); assert(character.m_iKillCount==8);
    quest_manager.m_mapEventFlag["threeway_war_dead_count"]=-19;
    character.ClearKillCount(); assert(character.m_iKillCount==-19);
    quest_manager.m_mapEventFlag.clear(); character.ClearKillCount();
    assert(character.m_iKillCount==0 && quest_manager.m_mapEventFlag.empty());
    character.EndStateEmpty();
    std::puts("character name passed: constructed full declaration, explicit name and localized mob fallback");
}
