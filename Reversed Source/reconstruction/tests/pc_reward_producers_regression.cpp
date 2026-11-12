// Actual full PC lifecycle, GetFlag and RewardData vector; nativeOS/log observer only.
#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <ext/hash_map>
#define private public
#include "../../game/src/questpc.h"
#undef private
#include "../../game/src/config.h"
#include <cassert>
#include <climits>
#include <cstdio>
#include <cstdlib>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static quest::PC* current;
static std::string* liveLabel;
static int mode,producerLogs,getLogs;
static DWORD observedFirst;
static int observedSecond;
static std::string observedLabel,observedKey;
extern "C" void pc_reward_producer_log(int kind,const char* name,unsigned long first,int second){
 assert(name);++producerLogs;observedLabel=name;observedFirst=first;observedSecond=second;
 assert(kind==1||kind==2);
 if(mode==1){current->m_stCurQuest="live";*liveLabel="changed";mode=0;}
 else if(mode==4){assert(liveLabel==&current->m_stCurQuest);current->m_stCurQuest="alias-live";mode=0;}
}
extern "C" void pc_reward_get_log(const char* name,int value){
 assert(name);++getLogs;observedKey=name;
 if(mode==2){assert(value==5);current->m_FlagMap["q.key"]=0;test_server=1;mode=3;}
 else if(mode==3){assert(value==0);current->m_FlagMap["q.key"]=-7;mode=0;}
 else if(mode==5){assert(value==-9);current->m_FlagMap["q.key"]=0;current->m_stCurQuest="different";*liveLabel="different";mode=0;}
}
static void Reset(quest::PC& pc){current=&pc;mode=producerLogs=getLogs=0;observedFirst=0;observedSecond=0;observedLabel.clear();observedKey.clear();pc.m_stCurQuest="q";pc.m_FlagMap.clear();pc.m_FlagSaveMap.clear();pc.m_vRewardData.clear();pc.m_bIsGivenReward=false;test_server=0;}
static void Row(const quest::PC& pc,unsigned i,quest::RewardData::RewardType type,DWORD first,int second){assert(i<pc.m_vRewardData.size());const quest::RewardData& r=pc.m_vRewardData[i];assert(r.type==type&&r.value1==first&&r.value2==second);}
int main(){
 assert(sizeof(DWORD)==4&&sizeof(int)==4&&sizeof(quest::RewardData)==12);desc_test_allow_clock();quest::PC pc;Reset(pc);assert(!pc.HasReward());
 const DWORD amounts[]={0,1,0x7fffffffUL,0x80000000UL,0xffffffffUL};const int counts[]={0,1,-1,INT_MIN,INT_MAX};
 // Repeated zero/absent flag never writes reward flags or deduplicates pending requests.
 pc.m_vRewardData.reserve(4);unsigned i=0;
 for(unsigned a=0;a<5;++a)for(unsigned c=0;c<5;++c){pc.GiveItem("same",amounts[a],counts[c]);Row(pc,i++,quest::RewardData::REWARD_TYPE_ITEM,amounts[a],counts[c]);assert(observedFirst==amounts[a]&&observedSecond==counts[c]);}
 for(unsigned a=0;a<5;++a){pc.GiveExp("same",amounts[a]);Row(pc,i++,quest::RewardData::REWARD_TYPE_EXP,amounts[a],0);assert(observedFirst==amounts[a]);}
 assert(pc.m_vRewardData.size()==30&&producerLogs==30&&!getLogs&&pc.m_FlagMap.empty()&&pc.m_FlagSaveMap.empty()&&!pc.m_bIsGivenReward&&pc.HasReward());
 // Every nonzero sign marks sticky duplicate status without consuming/changing existing queue.
 const int nonzero[]={1,-1,INT_MIN,INT_MAX};
 for(unsigned a=0;a<4;++a){Reset(pc);pc.m_vRewardData.push_back(quest::RewardData(quest::RewardData::REWARD_TYPE_ITEM,77,-8));pc.SetFlag("q.key",nonzero[a],true);pc.GiveItem("key",0xffffffffUL,INT_MIN);pc.GiveExp("key",0);assert(pc.m_vRewardData.size()==1&&pc.m_bIsGivenReward&&pc.m_FlagMap["q.key"]==nonzero[a]&&pc.m_FlagSaveMap.empty());Row(pc,0,quest::RewardData::REWARD_TYPE_ITEM,77,-8);pc.GiveExp("other",9);assert(pc.m_bIsGivenReward&&pc.m_vRewardData.size()==2);Row(pc,1,quest::RewardData::REWARD_TYPE_EXP,9,0);}
 // Stored zero behaves exactly like missing; empty and dotted labels are accepted unchanged.
 Reset(pc);pc.m_stCurQuest="";pc.m_FlagMap["."]=0;pc.GiveItem("",0,0);pc.GiveExp("label.part",1);assert(pc.m_vRewardData.size()==2&&pc.m_FlagMap.size()==1&&pc.m_FlagMap["."]==0&&!pc.m_bIsGivenReward&&getLogs==1);
 // String concatenation and lookup include embeddedNUL bytes even though producer logs use C prefixes.
 Reset(pc);pc.m_stCurQuest=std::string("q\0X",3);std::string label("l\0Y",3);std::string whole=pc.m_stCurQuest+"."+label;pc.m_FlagMap[whole]=-3;pc.GiveItem(label,8,9);assert(pc.m_bIsGivenReward&&pc.m_vRewardData.empty()&&observedLabel=="l");pc.m_FlagMap[whole]=0;pc.GiveExp(label,10);assert(pc.m_vRewardData.size()==1);Row(pc,0,quest::RewardData::REWARD_TYPE_EXP,10,0);
 // Producer diagnostics happen before either live currentquest or live label is consumed.
 Reset(pc);label="before";liveLabel=&label;pc.m_FlagMap["q.before"]=1;mode=1;pc.GiveItem(label,0x80000000UL,-7);assert(observedLabel=="before"&&label=="changed"&&pc.m_stCurQuest=="live"&&!pc.m_bIsGivenReward);Row(pc,0,quest::RewardData::REWARD_TYPE_ITEM,0x80000000UL,-7);
 Reset(pc);liveLabel=&pc.m_stCurQuest;mode=4;pc.m_FlagMap["alias-live.alias-live"]=1;pc.GiveExp(pc.m_stCurQuest,123);assert(pc.m_bIsGivenReward&&pc.m_vRewardData.empty()&&observedKey=="alias-live.alias-live");
 // Actual GetFlag rereads map value after both logs; test_server itself is read after first callback.
 Reset(pc);label="key";liveLabel=&label;pc.m_FlagMap["q.key"]=5;mode=2;pc.GiveExp(label,19);assert(getLogs==2&&mode==0&&test_server==1&&pc.m_bIsGivenReward&&pc.m_vRewardData.empty()&&pc.m_FlagMap["q.key"]==-7);
 // Key temporaries have already been formed before GetFlag diagnostics change quest/label.
 Reset(pc);label="key";liveLabel=&label;pc.m_FlagMap["q.key"]=-9;mode=5;pc.GiveItem(label,27,-28);assert(getLogs==1&&mode==0&&observedKey=="q.key"&&!pc.m_bIsGivenReward&&pc.m_stCurQuest=="different"&&label=="different");Row(pc,0,quest::RewardData::REWARD_TYPE_ITEM,27,-28);
 current=0;liveLabel=0;test_server=0;std::puts("whole PC GiveItem/GiveExp queue producers passed: actual flags/vector, log ordering, sticky duplicate status; no reward execution or VM claim");return 0;
}
