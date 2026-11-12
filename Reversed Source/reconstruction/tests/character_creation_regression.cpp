#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/char_manager.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/crc32.h"
#include "../../libthecore/include/main.h"
#include <cassert>
#include <cstdio>
#include <cstring>
extern DWORD isolated_dword_clock,isolated_dword_step;
extern float isolated_float_clock;
extern int test_server;
class DispatchProbe : public CHARACTER {
public:
    unsigned idle,moving,battle;
    DispatchProbe():idle(0),moving(0),battle(0) {}
    void StateIdle() { ++idle; }
    void StateMove() { ++moving; }
    void StateBattle() { ++battle; }
};
static void CheckBindings(CHARACTER* character) {
    assert(character->m_stateIdle.m_pInstance==character && character->m_stateMove.m_pInstance==character && character->m_stateBattle.m_pInstance==character);
    assert(character->m_stateIdle.m_pfnBeginState==&CHARACTER::BeginStateEmpty);
    assert(character->m_stateIdle.m_pfnState==&CHARACTER::StateIdle && character->m_stateMove.m_pfnState==&CHARACTER::StateMove);
    assert(character->m_stateBattle.m_pfnState==&CHARACTER::StateBattle && character->m_stateBattle.m_pfnEndState==&CHARACTER::EndStateEmpty);
    assert(character->m_stateMove.m_pfnBeginState==&CHARACTER::BeginStateEmpty && character->m_stateIdle.m_pfnEndState==&CHARACTER::EndStateEmpty);
    assert(!character->m_triggerOnClick.bType && !character->m_triggerOnClick.pFunc && character->m_kVIDVictim==VID());
    assert(character->m_pNewState==&character->m_stateIdle && character->m_dwStateDuration==1);
    assert(!character->GetDesc() && !character->GetSectree() && character->m_pointsInstant.position==7);
}
int main() {
    quest::CQuestManager quests;
    quests.m_mapEventFlag["arena_potion_limit_count"]=14;
    quests.m_mapEventFlag["threeway_war_dead_count"]=6;
    CHARACTER_MANAGER manager;
    isolated_dword_clock=100; isolated_dword_step=0; isolated_float_clock=10;
    thecore_heart->pulse=50; test_server=0;
    {
        DispatchProbe probe; CheckBindings(&probe);
        assert(probe.m_nPotionLimit==14 && probe.m_iKillCount==6 && probe.m_dwLastSkillTime==100);
        probe.Update(); assert(probe.idle==1 && !probe.m_pNewState);
        probe.GotoState(probe.m_stateMove); probe.Update(); assert(probe.moving==1);
        probe.SetPosition(5); probe.Update(); assert(probe.battle==1);
        probe.SetPosition(7); probe.Update(); assert(probe.idle==2);
    }
    std::vector<CHARACTER*> created;
    const char* first_name="ABCDEFGHIJKLMNOPQRSTUVWX first";
    CHARACTER* first=manager.CreateCharacter(first_name,101); created.push_back(first);
    CheckBindings(first);
    assert(static_cast<DWORD>(first->GetVID())==1 && first->GetPlayerID()==0);
    assert(!std::strcmp(first->GetName(),first_name));
    assert(manager.Find(1)==first && manager.Find(first->GetVID())==first && manager.FindByPID(101)==first);
    assert(manager.FindPC("abcdefghijklmnopqrstuvwx ignored")==first);
    char identity[129]; std::snprintf(identity,128,"%s%p172815",first_name,static_cast<void*>(first));
    assert(first->GetVID()==VID(1,get_crc32(identity,std::strlen(identity))));
    CHARACTER* npc=manager.CreateCharacter("not stored"); created.push_back(npc);
    assert(static_cast<DWORD>(npc->GetVID())==2 && !std::strcmp(npc->GetName(),"") && !manager.FindByPID(0));
    assert(manager.m_map_pkPCChr.size()==1 && manager.m_map_pkChrByPID.size()==1);
    CHARACTER* same_name=manager.CreateCharacter("abcdefghijklmnopqrstuvwx second",102); created.push_back(same_name);
    assert(same_name!=first && static_cast<DWORD>(same_name->GetVID())==3);
    assert(manager.FindPC(same_name->GetName())==first && manager.FindByPID(102)==same_name && manager.Find(3)==same_name);
    CHARACTER* same_pid=manager.CreateCharacter("another",101); created.push_back(same_pid);
    assert(manager.FindPC("ANOTHER")==same_pid && manager.FindByPID(101)==first && manager.Find(4)==same_pid);
    manager.m_iVIDCount=0;
    CHARACTER* same_vid=manager.CreateCharacter("collision",104); created.push_back(same_vid);
    assert(static_cast<DWORD>(same_vid->GetVID())==1 && manager.Find(1)==first);
    assert(manager.FindPC("COLLISION")==same_vid && manager.FindByPID(104)==same_vid);
    CHARACTER* empty=manager.CreateCharacter("",0xffffffffUL); created.push_back(empty);
    assert(manager.FindPC("")==empty && manager.FindByPID(0xffffffffUL)==empty);
    manager.m_iVIDCount=-1; CHARACTER* zero=manager.CreateCharacter("zero id npc"); created.push_back(zero);
    assert(static_cast<DWORD>(zero->GetVID())==0 && manager.Find(0)==zero && manager.FindPC("")==empty);
    assert(first->m_nPotionLimit==14 && same_vid->m_iKillCount==6);
    manager.m_map_pkChrByVID.clear(); manager.m_map_pkChrByPID.clear(); manager.m_map_pkPCChr.clear();
    for(unsigned i=0;i<created.size();++i) delete created[i];
    //Normal registry teardown after proto loading supplies the player ID field.
    manager.m_iVIDCount=70;
    CHARACTER* clean=manager.CreateCharacter("clean",8080); clean->m_dwPlayerID=8080;
    manager.AddToStateList(clean); manager.DestroyCharacter(clean);
    assert(!manager.Find(71) && !manager.FindByPID(8080) && !manager.FindPC("clean") && manager.m_set_pkChrState.empty());
    std::puts("character creation passed: real constructor/FSM, seeded identities, PC/NPC maps, duplicate keys and VID wrap");
}
