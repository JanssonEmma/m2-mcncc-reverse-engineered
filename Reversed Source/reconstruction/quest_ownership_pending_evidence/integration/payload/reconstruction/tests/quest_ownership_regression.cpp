// Whole actual CQM/NPC lifecycle. Real SDK GCfinalizers/eventcleanup; no fake Lua/NPC/PC methods.
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
#include "../../game/src/questevent.h"
#undef private
#undef protected
#include "../../game/src/config.h"
#include "../../libthecore/include/main.h"
extern "C" {
#include "../../liblua/include/lua.h"
}
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <new>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static quest::CQuestManager* observed;
static lua_State* replacement;
static int phase,finalizers,timerLogs,pulse;
static std::vector<DWORD> pcIDs;
struct FinalizerPayload { int marker;int replace; };
extern "C" void quest_ownership_clear_timer(int count){
 if(phase==2&&count>0){
  assert(observed&&finalizers==1&&observed->L==0&&quest::CQuestManager::instance_ptr()==observed);
  // These members occur BEFORE m_mapPC in declaration order and are STILLalive during its teardown.
  assert(observed->m_mapNPC.find(19)!=observed->m_mapNPC.end()&&observed->m_mapNPC.find(19)->second.m_vnum==919);
  assert(observed->m_mapEventName.find("phase")!=observed->m_mapEventName.end()&&observed->m_mapEventName.find("phase")->second==7);
  ++timerLogs;
 }
}
static int Finalizer(lua_State* L){
 FinalizerPayload* data=static_cast<FinalizerPayload*>(lua_touserdata(L,1));assert(data&&data->marker==0x13579&&observed&&phase==1&&observed->L==L&&quest::CQuestManager::instance_ptr()==observed);
 assert(observed->m_mapPC.size()==2&&observed->m_mapNPC.size()==2&&observed->m_strScript=="owned-context");
 for(unsigned i=0;i<pcIDs.size();++i)assert(FindEventInfo(pcIDs[i]));
 ++finalizers;if(data->replace){assert(replacement&&replacement!=L);observed->L=replacement;}phase=2;return 0;
}
static void OwnVM(quest::CQuestManager& owner,bool replace){
 owner.L=lua_open();assert(owner.L);FinalizerPayload* data=new(lua_newuserdata(owner.L,sizeof(FinalizerPayload)))FinalizerPayload;data->marker=0x13579;data->replace=replace?1:0;
 lua_newtable(owner.L);lua_pushstring(owner.L,"__gc");lua_pushcfunction(owner.L,Finalizer);lua_settable(owner.L,-3);lua_setmetatable(owner.L,-2);lua_setglobal(owner.L,"owned_userdata");assert(lua_gettop(owner.L)==0);
}
static long NeverRun(LPEVENT){assert(!"Canceled ownershiptimer unexpectedly executed");return 0;}
static LPEVENT Timer(unsigned pid,const char* name,DWORD& id){
 quest::quest_event_info* info;id=__AllocEventInfo(sizeof(*info),reinterpret_cast<void**>(&info));info->time_cycle=17;info->player_id=pid;info->npc_id=88;info->name=new char[std::strlen(name)+1];std::strcpy(info->name,name);return event_create_ex(NeverRun,id,50,const_cast<char*>("ownership-regression"),1);
}
static void NPCData(quest::NPC& npc){
 npc.m_vnum=919;for(unsigned i=0;i<20;++i){
  const char code[]={'a',0,'z'};npc.m_mapOwnQuest[i][100+i][-1].m_code.assign(code,code+3);npc.m_mapOwnQuest[i][100+i][2].m_code.assign(1,'q');
  // Complete by-value AArgScript container with reservedRAWstorage but NO ArgScriptobject construction.
  npc.m_mapOwnArgQuest[i][200+i][-3].reserve(i%3+1);assert(npc.m_mapOwnArgQuest[i][200+i][-3].empty());
 }
}
static void CheckNPC(const quest::NPC& npc){
 assert(npc.m_vnum==919);for(unsigned i=0;i<20;++i){
  quest::NPC::QuestMapType::const_iterator q=npc.m_mapOwnQuest[i].find(100+i);assert(q!=npc.m_mapOwnQuest[i].end()&&q->second.size()==2);quest::NPC::AQuestScriptType::const_iterator s=q->second.find(-1);assert(s!=q->second.end()&&s->second.m_code.size()==3&&s->second.m_code[0]=='a'&&!s->second.m_code[1]&&s->second.m_code[2]=='z');
  quest::NPC::ArgQuestMapType::const_iterator a=npc.m_mapOwnArgQuest[i].find(200+i);assert(a!=npc.m_mapOwnArgQuest[i].end()&&a->second.size()==1&&a->second.find(-3)!=a->second.end()&&a->second.find(-3)->second.empty());
 }
}
static void NPCStandalone(){
 quest::NPC original;assert(original.m_vnum==0);for(unsigned i=0;i<20;++i)assert(original.m_mapOwnQuest[i].empty()&&original.m_mapOwnArgQuest[i].empty());NPCData(original);
 // Original081ab460 is implicitCOPY, not a defaultconstructor. Full members remain compilerowned.
 quest::NPC copied(original);CheckNPC(copied);original.m_mapOwnQuest[0][100][-1].m_code[0]='X';original.m_mapOwnQuest[19].clear();original.m_mapOwnArgQuest[19].clear();CheckNPC(copied);
 quest::NPC* heap=new quest::NPC(copied);CheckNPC(*heap);delete heap;
}
static void Populate(quest::CQuestManager& owner,LPEVENT& external,DWORD& externalID){
 assert(owner.m_mapPC.empty()&&owner.m_mapNPC.empty());owner.m_mapEventName["phase"]=7;owner.m_strScript="owned-context";owner.m_bNoSend=true;owner.m_bError=true;owner.m_iCurrentSkin=5;owner.m_pCurrentPC=0;owner.m_pCurrentCharacter=0;owner.m_CurrentRunningState=0;
 owner.m_mapEventFlag["flag"]=-8;owner.m_registeredNPCVnum.insert(19);owner.m_mapNPCNameID["name"]=19;owner.m_mapTimerID["timer"]=29;owner.m_hmQuestName["quest"]=31;owner.m_hmQuestStartScript[31].assign(3,'s');owner.m_mapQuestNameByIndex[31]="quest";owner.compression_vector.assign(33,0x5a);
 NPCData(owner.m_mapNPC[0]);NPCData(owner.m_mapNPC[19]);
 // Seed LastState BEFORE any implicitPCcopy, preserving originalproduction indeterminate default.
 quest::PC source;source.m_iLastState=0;owner.m_mapPC.insert(std::make_pair(37u,source));owner.m_mapPC.insert(std::make_pair(77u,source));pcIDs.clear();
 for(unsigned i=0;i<2;++i){unsigned pid=i?77:37;quest::PC& pc=owner.m_mapPC.find(pid)->second;pc.SetID(pid);pc.m_iLastState=17;quest::QuestState state;state._clock_value=11;state._counter_value=-12;state.chat_scripts.push_back(0);pc.m_QuestInfo.insert(std::make_pair(1u,state));DWORD id;pc.m_TimerMap["owned"]=Timer(pid,"owned",id);pcIDs.push_back(id);}
 // The server-timer map owns only a raw eventpointer node. Use a fullyvalid generalEVENT payload;
 // its TESTcallback never runs and containerdestruction must notinvent pointer/info ownership.
 external=Timer(99,"external-map-pointer",externalID);owner.m_mapServerTimer[std::make_pair(std::string("external"),static_cast<DWORD>(9))]=external;
}
static void PendingAfterDestruction(LPEVENT& external,DWORD externalID){
 assert(!quest::CQuestManager::instance_ptr()&&timerLogs==2&&finalizers==1);for(unsigned i=0;i<pcIDs.size();++i)assert(!FindEventInfo(pcIDs[i]));assert(FindEventInfo(externalID)&&external);quest::CancelTimerEvent(&external);assert(!external&&!FindEventInfo(externalID));pulse+=100;thecore_heart->pulse=pulse;assert(event_process(pulse)==0&&!event_count());phase=0;observed=0;
}
static void ExplicitDestroy(){
 LPEVENT external;DWORD externalID;finalizers=timerLogs=0;replacement=lua_open();assert(replacement);
 {quest::CQuestManager owner;observed=&owner;assert(owner.L==0);owner.Destroy();assert(!owner.L);Populate(owner,external,externalID);OwnVM(owner,true);phase=1;owner.Destroy();assert(owner.L==0&&finalizers==1&&lua_gettop(replacement)==0&&quest::CQuestManager::instance_ptr()==&owner);
  assert(owner.m_mapPC.size()==2&&owner.m_mapNPC.size()==2&&owner.m_strScript=="owned-context"&&owner.m_iCurrentSkin==5&&owner.m_bNoSend&&owner.m_bError);for(unsigned i=0;i<pcIDs.size();++i)assert(FindEventInfo(pcIDs[i]));owner.Destroy();assert(finalizers==1&&owner.L==0);lua_close(replacement);replacement=0;phase=2;}
 PendingAfterDestruction(external,externalID);
}
static void VirtualDestructor(){
 LPEVENT external;DWORD externalID;finalizers=timerLogs=0;quest::CQuestManager* owner=new quest::CQuestManager;observed=owner;Populate(*owner,external,externalID);OwnVM(*owner,false);phase=1;
 // Destructor itself must call actualDestroy, then originalmembertearorder. The GCcallback entersphase2
 // via separateobserver flag below, without dereferencing alreadydestroyed script/mappartials.
 singleton<quest::CQuestManager>* base=owner;delete base;PendingAfterDestruction(external,externalID);
}
int main(){desc_test_allow_clock();thecore_heart->pulse=0;pulse=0;test_server=0;assert(sizeof(DWORD)==4);NPCStandalone();ExplicitDestroy();VirtualDestructor();event_destroy();std::puts("whole CQM Destroy/dtor andNPC lifecycle actual SDK/container/event passed; populatedAArgScript ctor domains remainstatic, no pointee-delete repair");return 0;}
