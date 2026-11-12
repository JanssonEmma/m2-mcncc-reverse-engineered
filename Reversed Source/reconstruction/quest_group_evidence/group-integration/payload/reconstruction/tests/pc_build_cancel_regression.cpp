// Actual complete PC lifecycle/events/manager graph; Build matching suffix path is STATIC ONLY.
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#undef private
#include <cassert>
#include <climits>
#include <cstdio>
#include <cstdlib>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static void Init(quest::QuestState& state){state._clock_value=17;state._counter_value=-18;state.ico=5;state.args=-2;state.iIndex=23;state.bStart=true;state.st=41;state._title="retained";state._clock_name="clock";state._counter_name="counter";state._icon_file="icon";state.chat_scripts.push_back(0);}
static void Unchanged(const quest::QuestState& state){assert(state._clock_value==17&&state._counter_value==-18&&state.ico==5&&state.args==-2&&state.iIndex==23&&state.bStart&&state.st==41);assert(state._title=="retained"&&state._clock_name=="clock"&&state._counter_name=="counter"&&state._icon_file=="icon"&&state.chat_scripts.size()==1&&!state.chat_scripts[0]);}
int main(){
 desc_test_allow_clock();assert(sizeof(int)==4&&sizeof(DWORD)==4);quest::CQuestManager manager;quest::PC pc;
 assert(!pc.IsLoaded());pc.Build();assert(pc.m_QuestInfo.empty()&&pc.m_FlagMap.empty()&&!pc.IsLoaded());
 quest::QuestState state;Init(state);pc.m_QuestInfo.insert(std::make_pair(7u,state));quest::QuestState* retained=&pc.m_QuestInfo.find(7u)->second;
 pc.m_dwID=0xaabbccddUL;pc.m_stCurQuest="retained-context";pc.m_iLastState=-83;pc.m_FlagSaveMap["pending.flag"]=INT_MIN;pc.m_vRewardData.push_back(quest::RewardData(quest::RewardData::REWARD_TYPE_NONE,0));pc.m_bIsGivenReward=true;pc.SetConfirmWait(0x12345678UL);
 // Strict >9 length gate and nonmatching suffixes ONLY: never create/copy a default state.
 const char* keys[]={"","x","__status",".__status","x.__statu","x.__statusX","x.__statusx","x.__STATUS","x.__status.more"};
 for(unsigned i=0;i<sizeof(keys)/sizeof(keys[0]);++i)pc.m_FlagMap[keys[i]]=static_cast<int>(i)-4;
 pc.m_FlagMap[std::string("x.__status\0",11)]=INT_MAX; // embedded trailing NUL changes actual suffix.
 const std::map<std::string,int> flags=pc.m_FlagMap;
 pc.m_RunningQuestState=retained;pc.m_iSendToClient=-1;pc.m_bShouldSendDone=true;pc.Build();pc.Build();
 assert(pc.m_FlagMap==flags&&pc.m_QuestInfo.size()==1&&pc.m_RunningQuestState==retained&&pc.m_iSendToClient==-1&&pc.m_bShouldSendDone);Unchanged(*retained);
 assert(pc.m_FlagSaveMap.size()==1&&pc.m_FlagSaveMap.find("pending.flag")->second==INT_MIN&&pc.m_vRewardData.size()==1&&pc.m_bIsGivenReward&&!pc.IsLoaded());
 // SetLoaded stores only loaded=true, is idempotent, and never calls Build.
 pc.SetLoaded();pc.SetLoaded();assert(pc.IsLoaded()&&pc.m_QuestInfo.size()==1&&pc.m_FlagMap==flags&&pc.m_RunningQuestState==retained&&pc.m_iSendToClient==-1&&pc.m_bShouldSendDone);
 const int masks[]={0,1,127,INT_MIN,INT_MAX,-1};
 for(unsigned loaded=0;loaded<2;++loaded)for(unsigned pointer=0;pointer<2;++pointer)for(unsigned done=0;done<2;++done)for(unsigned i=0;i<sizeof(masks)/sizeof(masks[0]);++i){
  pc.m_bLoaded=loaded!=0;pc.m_RunningQuestState=pointer?retained:0;pc.m_bShouldSendDone=done!=0;pc.m_iSendToClient=masks[i];pc.CancelRunning();
  assert(!pc.IsRunning()&&!pc.GetRunningQuestState()&&!pc.m_iSendToClient&&!pc.m_bShouldSendDone&&pc.IsLoaded()==(loaded!=0));
  assert(pc.m_dwID==0xaabbccddUL&&pc.m_stCurQuest=="retained-context"&&pc.m_iLastState==-83&&pc.m_FlagMap==flags&&pc.m_FlagSaveMap.size()==1&&pc.m_FlagSaveMap.find("pending.flag")->second==INT_MIN&&pc.m_QuestInfo.size()==1&&pc.m_vRewardData.size()==1&&pc.m_bIsGivenReward&&pc.IsConfirmWait(0x12345678UL));Unchanged(*retained);
 }
 pc.m_RunningQuestState=0;std::puts("actual PC CancelRunning/SetLoaded + Build empty/nonmatching passed; qualifying Build branches static-only due original indeterminate-counter copy");return 0;
}
