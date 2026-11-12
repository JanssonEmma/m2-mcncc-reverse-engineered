// Actual full PC/state/STL/manager queries/flags/timer lifecycle; external logging observation only.
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#include "../../game/src/questevent.h"
#undef private
#include "../../game/src/config.h"
#include "../../libthecore/include/main.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static quest::PC* observed;
static quest::CQuestManager* manager;
static quest::QuestState* redirectState;
static quest::QuestState* inputState;
static int mode,gets,reserves,sets,clears;
static unsigned int expectedIndex;
static std::string flagName;
extern "C" void pc_quest_storage_log(int kind,int level,const char* text,int value,unsigned long index,int previous,int next){
    if(!observed)return;
    if(kind==1){
        ++sets;
        if(mode==1){
            assert(text && std::string(text)=="ordered.__status" && value==5);
            assert(observed->m_stCurQuest=="ordered" && observed->m_RunningQuestState==&observed->m_QuestInfo[expectedIndex]);
            assert(observed->m_iSendToClient==0 && observed->m_iLastState==5 && observed->m_bShouldSendDone);
            observed->m_RunningQuestState=redirectState;observed->m_iSendToClient=77;observed->m_iLastState=88;
            inputState->st=123;observed->m_bShouldSendDone=true;mode=0;
        }
    }else if(kind==2){
        ++gets;
        if(mode==2)observed->m_FlagMap[flagName]=value+1;
        assert(level==1 || level==0);
    }else if(kind==3){
        ++reserves;
        if(mode==3){
            assert(index==17 && previous==4 && next==8);
            manager->m_hmQuestName["change"]=99;
            observed->m_QuestStateChange.clear();
            observed->m_QuestStateChange.push_back(quest::PC::TQuestStateChangeInfo(222,333,444));
            mode=0;
        }
    }else if(kind==4){
        ++clears;
        assert(value==static_cast<int>(observed->m_TimerMap.size()));
        if(mode==4){manager->m_hmQuestName["clear"]=22;mode=0;}
    }
}
static void Begin(quest::PC& pc){observed=&pc;mode=0;gets=reserves=sets=clears=0;}
static void Init(quest::QuestState& s,int state){
    s.co=0;s.ico=12;s.args=-3;s.suspend_state=255;s.iIndex=777;s.bStart=true;s.st=state;
    s._title.assign("title\0tail",10);s._clock_name="clock";s._counter_name="counter";s._icon_file="icon";
    s._clock_value=INT_MIN;s._counter_value=INT_MAX;s.chat_scripts.clear();s.chat_scripts.push_back(0);s.chat_scripts.push_back(0);
}
static void EqualPayload(const quest::QuestState& a,const quest::QuestState& b){
    assert(a.co==b.co && a.ico==b.ico && a.args==b.args && a.suspend_state==b.suspend_state && a.bStart==b.bStart && a.st==b.st);
    assert(a._title==b._title && a._clock_name==b._clock_name && a._counter_name==b._counter_name && a._icon_file==b._icon_file);
    assert(a._clock_value==b._clock_value && a._counter_value==b._counter_value && a.chat_scripts==b.chat_scripts);
}
static void LookupCases(quest::CQuestManager& qm){
    quest::PC pc;Begin(pc);assert(pc.quest_begin()==pc.quest_end() && pc.quest_find(0)==pc.quest_end());
    assert(!pc.HasQuest("unknown") && pc.m_QuestInfo.empty());
    quest::QuestState& zero=pc.GetQuest("unknown");
    assert(pc.m_QuestInfo.size()==1 && &zero==&pc.m_QuestInfo.find(0)->second);
    // Never inspect old default indeterminate _clock_value/_counter_value.
    assert(!zero.co && !zero.ico && !zero.args && !zero.suspend_state && !zero.iIndex && !zero.bStart && zero.st==-1);
    assert(zero._title.empty() && zero._clock_name.empty() && zero._counter_name.empty() && zero._icon_file.empty() && zero.chat_scripts.empty());
    zero._clock_value=0;zero._counter_value=0;
    assert(pc.HasQuest("another-unknown") && &pc.GetQuest("another-unknown")==&zero);
    const unsigned int keys[]={1,0x7fffffffU,0x80000000U,0xffffffffU};
    const std::string names[]={"one",std::string("binary\0name",11),std::string("\xff\x80",2),std::string(300,'q')};
    for(unsigned i=0;i<4;++i){
        qm.m_hmQuestName[names[i]]=static_cast<int>(keys[i]);
        assert(!pc.HasQuest(names[i]));quest::QuestState& s=pc.GetQuest(names[i]);
        assert(s.iIndex==0 && s.st==-1);s._clock_value=1;s._counter_value=2;
        assert(&s==&pc.quest_find(static_cast<DWORD>(keys[i]))->second && pc.HasQuest(names[i]));
        s._title=names[i];s.st=static_cast<int>(i+10);assert(pc.GetQuest(names[i])._title==names[i]);
        pc.m_stCurQuest=names[i];assert(pc.GetCurrentQuestIndex()==static_cast<int>(keys[i]));
    }
    unsigned position=0;for(quest::PC::QuestInfoIterator it=pc.quest_begin();it!=pc.quest_end();++it){assert(it->first==(position?keys[position-1]:0));++position;}assert(position==5);
    unsigned count=pc.m_QuestInfo.size();assert(pc.quest_find(7)==pc.quest_end() && pc.m_QuestInfo.size()==count);
    pc.m_stCurQuest="no-such-name";assert(pc.GetCurrentQuestIndex()==0 && pc.m_QuestInfo.size()==count);
    assert(qm.m_hmQuestName.find("unknown")==qm.m_hmQuestName.end());observed=0;
}
static void SetCases(quest::CQuestManager& qm){
    quest::PC pc;quest::QuestState input;Init(input,5);qm.m_hmQuestName["ordered"]=17;expectedIndex=17;
    Begin(pc);pc.m_bShouldSendDone=true;pc.SetQuest("ordered",input);
    quest::QuestState* stored=&pc.GetQuest("ordered");EqualPayload(*stored,input);assert(stored->iIndex==17 && stored!=&input);
    assert(&stored->chat_scripts[0]!=&input.chat_scripts[0]);
    assert(pc.m_RunningQuestState==stored && pc.m_stCurQuest=="ordered" && pc.m_iSendToClient==0 && pc.m_iLastState==5 && !pc.m_bShouldSendDone);
    assert(pc.m_FlagMap["ordered.__status"]==5 && pc.m_FlagSaveMap["ordered.__status"]==5);
    input._title="changed payload";input.st=INT_MIN;input._clock_value=444;input.chat_scripts.push_back(0);
    Begin(pc);pc.SetQuest("ordered",input);assert(&pc.GetQuest("ordered")==stored);EqualPayload(*stored,input);assert(stored->iIndex==17);
    Begin(pc);pc.SetQuest(pc.m_stCurQuest,*stored);assert(pc.m_RunningQuestState==stored && stored->iIndex==17);
    // Name is a live field of the very map value being overwritten: preserve no-early-copy behavior.
    qm.m_hmQuestName["alias"]=33;qm.m_hmQuestName["replacement-name"]=44;
    quest::QuestState& alias=pc.GetQuest("alias");Init(alias,3);alias._title="alias";
    Init(input,9);input._title="replacement-name";Begin(pc);pc.SetQuest(alias._title,input);
    assert(pc.m_RunningQuestState==&alias && alias.iIndex==33 && pc.m_stCurQuest=="replacement-name");
    assert(pc.m_FlagMap["replacement-name.__status"]==9 && pc.GetCurrentQuestIndex()==44);
    // Query key0 is a real storage key; zero state removes an existing STATUS flag through actual SetFlag.
    Init(input,0);pc.SetFlag("unknown-set.__status",9,false);Begin(pc);pc.SetQuest("unknown-set",input);
    assert(pc.m_QuestInfo.count(0) && pc.m_RunningQuestState==&pc.m_QuestInfo.find(0)->second);
    assert(!pc.m_FlagMap.count("unknown-set.__status") && pc.m_FlagSaveMap["unknown-set.__status"]==0);
    // Callback proves state/flag values were captured but CURRENT running pointer is reread after SetFlag.
    quest::QuestState redirected;Init(redirected,-7);Init(input,5);redirectState=&redirected;inputState=&input;
    Begin(pc);pc.m_bShouldSendDone=true;mode=1;pc.SetQuest("ordered",input);
    assert(mode==0 && redirected.iIndex==17 && pc.m_RunningQuestState==&redirected);
    assert(pc.m_QuestInfo.find(17)->second.st==5 && pc.m_QuestInfo.find(17)->second.iIndex==777 && pc.m_FlagMap["ordered.__status"]==5);
    assert(input.st==123 && pc.m_iSendToClient==77 && pc.m_iLastState==88 && !pc.m_bShouldSendDone);
    pc.m_RunningQuestState=0;observed=0;
}
static void QueueCases(quest::CQuestManager& qm){
    quest::PC pc;qm.m_hmQuestName["change"]=17;flagName="change.__status";pc.m_FlagMap[flagName]=3;
    pc.m_stCurQuest="unchanged";Begin(pc);mode=2;test_server=0;pc.SetQuestState(std::string("change"),4);
    assert(gets==1 && reserves==0 && pc.m_QuestStateChange.empty() && pc.m_FlagMap[flagName]==4);
    Begin(pc);mode=2;test_server=-1;pc.SetQuestState(std::string("change"),6);
    assert(gets==2 && reserves==0 && pc.m_QuestStateChange.empty() && pc.m_FlagMap[flagName]==6);
    pc.m_FlagMap[flagName]=4;Begin(pc);test_server=0;mode=3;pc.SetQuestState(std::string("change"),8);
    assert(mode==0 && reserves==1 && pc.m_QuestStateChange.size()==2);
    assert(pc.m_QuestStateChange[0].quest_idx==222 && pc.m_QuestStateChange[0].prev_state==333 && pc.m_QuestStateChange[0].next_state==444);
    assert(pc.m_QuestStateChange[1].quest_idx==17 && pc.m_QuestStateChange[1].prev_state==4 && pc.m_QuestStateChange[1].next_state==8);
    assert(pc.m_FlagMap[flagName]==4 && pc.m_FlagSaveMap.empty() && pc.m_QuestInfo.empty() && pc.m_stCurQuest=="unchanged");
    Begin(pc);pc.SetQuestState(std::string("missing"),0);assert(!gets && !reserves);
    pc.SetQuestState(std::string("missing"),INT_MIN);assert(reserves==1 && pc.m_QuestStateChange.back().quest_idx==0 && pc.m_QuestStateChange.back().prev_state==0 && pc.m_QuestStateChange.back().next_state==INT_MIN);
    const int numbers[]={0,1,-1,INT_MIN,INT_MAX};
    for(unsigned i=0;i<25;++i){pc.AddQuestStateChange("change",numbers[i%5],numbers[i/5]);const quest::PC::TQuestStateChangeInfo& r=pc.m_QuestStateChange.back();assert(r.quest_idx==99 && r.prev_state==numbers[i%5] && r.next_state==numbers[i/5]);}
    observed=0;
}
static long NeverRun(LPEVENT){assert(!"cleared timer executed");return 0;}
static LPEVENT Timer(const char* name,DWORD& id){quest::quest_event_info* info;id=__AllocEventInfo(sizeof(*info),reinterpret_cast<void**>(&info));info->time_cycle=1;info->player_id=77;info->npc_id=88;info->name=new char[std::strlen(name)+1];std::strcpy(info->name,name);return event_create_ex(NeverRun,id,5,const_cast<char*>("pc-quest-storage-regression"),1);}
static void ClearCases(quest::CQuestManager& qm){
    quest::PC pc;qm.m_hmQuestName["clear"]=11;quest::QuestState& first=pc.GetQuest("clear");Init(first,7);
    qm.m_hmQuestName["second"]=22;quest::QuestState& second=pc.GetQuest("second");Init(second,8);
    pc.m_FlagMap["clear.zero"]=0;pc.m_FlagMap["clear."]=0;pc.m_FlagMap["clear"]=9;pc.m_FlagMap["clearer.flag"]=10;pc.m_FlagMap["other.flag"]=11;
    pc.m_stCurQuest="keep";pc.m_RunningQuestState=&second;pc.m_bShouldSendDone=true;pc.m_iLastState=INT_MIN;
    DWORD aID,bID;pc.AddTimer("a",Timer("a",aID));pc.AddTimer("unrelated",Timer("b",bID));
    Begin(pc);mode=4;pc.ClearQuest("clear");
    assert(mode==0 && clears==1 && pc.m_TimerMap.empty() && !FindEventInfo(aID) && !FindEventInfo(bID));
    assert(first.st==7 && second.st==0 && second.iIndex==777 && pc.m_QuestInfo.size()==2);
    assert(pc.m_FlagMap.size()==5 && pc.m_FlagSaveMap.empty() && pc.m_FlagMap["clear.zero"]==0 && pc.m_FlagMap["clear"]==9);
    assert(pc.m_stCurQuest=="keep" && pc.m_RunningQuestState==&second && pc.m_bShouldSendDone && pc.m_iLastState==INT_MIN);
    Begin(pc);pc.ClearQuest("absent");assert(pc.m_QuestInfo.size()==2 && second.st==0 && first.st==7 && clears==1);
    thecore_heart->pulse=100;assert(event_process(100)==0 && !event_count());pc.m_RunningQuestState=0;observed=0;
    // Matched-nonzero flag path deliberately excluded: original DeleteFlag receives erased live key reference.
}
int main(){desc_test_allow_clock();thecore_heart->pulse=0;quest::CQuestManager qm;manager=&qm;test_server=0;LookupCases(qm);SetCases(qm);QueueCases(qm);ClearCases(qm);observed=0;event_destroy();std::puts("PC quest storage passed: actual manager query/state/flags/timers, no Lua state resolution claim");return 0;}
