// Actual complete application graph. No lookup/application callbacks or VM substitutes.
#include <map>
#include <set>
#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/questmanager.h"
#include "../../game/src/char.h"
#include "../../game/src/char_manager.h"
#undef protected
#undef private
#include "../../game/src/mob_manager.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
int main(){
 desc_test_allow_clock();CHARACTER_MANAGER cm;quest::CQuestManager qm;CHARACTER player,npc,other;DescOutputProbe desc;CMob mob;
 assert(sizeof(DWORD)==4&&sizeof(WORD)==2);qm.m_pCurrentCharacter=0;
 assert(!qm.GetCurrentNPCCharacterPtr()&&!qm.GetCurrentNPCRace());
 const DWORD vids[]={0,1,0x7fffffffUL,0x80000000UL,0xffffffffUL};
 npc.m_bCharType=0;other.m_bCharType=0;player.m_dwPlayerID=77;
 npc.SetQuestNPCID(123);other.SetQuestNPCID(456);
 qm.m_pCurrentCharacter=&player;qm.m_iCurrentSkin=4;qm.m_strScript="preserve";qm.m_bError=true;
 for(unsigned i=0;i<5;++i){
  cm.m_map_pkChrByVID.clear();player.SetQuestNPCID(vids[i]);
  assert(player.m_dwQuestNPCVID==vids[i]&&npc.m_dwQuestNPCVID==123&&other.m_dwQuestNPCVID==456);
  assert(cm.m_map_pkChrByVID.empty()&&player.m_dwPlayerID==77);
  assert(!player.GetQuestNPC()&&!qm.GetCurrentNPCCharacterPtr()&&!qm.GetCurrentNPCRace());
  cm.m_map_pkChrByPID[vids[i]]=&other; // PID map is deliberately DIFFERENT; never substituted.
  cm.m_map_pkChrByVID[vids[i]]=0;assert(!player.GetQuestNPC()&&!qm.GetCurrentNPCCharacterPtr());
  cm.m_map_pkChrByVID[vids[i]]=&npc;assert(player.GetQuestNPC()==&npc&&qm.GetCurrentNPCCharacterPtr()==&npc);
  npc.BindDesc(0);npc.m_dwPolymorphRace=0;npc.m_pkMobData=0;npc.m_points.job=3;
  assert(qm.GetCurrentNPCRace()==3);
  npc.BindDesc(&desc);assert(player.GetQuestNPC()==&npc&&qm.GetCurrentNPCRace()==3);npc.BindDesc(0);
  mob.m_table.dwVnum=0x12345678UL;npc.m_pkMobData=&mob;assert(qm.GetCurrentNPCRace()==0x5678U);
  npc.m_dwPolymorphRace=0x8765ffffUL;assert(qm.GetCurrentNPCRace()==0xffffU);
  npc.m_dwPolymorphRace=0x10000UL;assert(qm.GetCurrentNPCRace()==0U); // WORD truncate, nonzero polymorph still takes precedence.
  npc.m_dwPolymorphRace=0;npc.m_pkMobData=0;
  other.m_dwPolymorphRace=0;other.m_pkMobData=0;other.m_points.job=1;
  cm.m_map_pkChrByVID[vids[i]]=&other;assert(player.GetQuestNPC()==&other&&qm.GetCurrentNPCRace()==1);
  assert(qm.m_pCurrentCharacter==&player&&qm.m_iCurrentSkin==4&&qm.m_strScript=="preserve"&&qm.m_bError);
  assert(cm.m_map_pkChrByVID.size()==1&&cm.m_map_pkChrByVID.find(vids[i])->second==&other);
 }
 // Fresh current CHARACTER and ID on separate calls; no invented mid-lookup observer.
 other.SetQuestNPCID(0x11223344UL);cm.m_map_pkChrByVID[0x11223344UL]=&npc;qm.m_pCurrentCharacter=&other;
 assert(qm.GetCurrentNPCCharacterPtr()==&npc);qm.m_pCurrentCharacter=0;
 assert(!qm.GetCurrentNPCCharacterPtr()&&!qm.GetCurrentNPCRace());
 cm.m_map_pkChrByVID.clear();cm.m_map_pkChrByPID.clear();npc.m_pkMobData=0;npc.BindDesc(0);qm.m_pCurrentPC=0;
 std::puts("whole quest NPC context lookup/set/race actual manager and CHAR graph passed; double lookup static-only");return 0;
}
