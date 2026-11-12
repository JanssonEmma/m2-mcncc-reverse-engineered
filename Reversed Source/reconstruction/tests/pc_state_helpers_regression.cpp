#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <ext/hash_map>
#define private public
#include "../../game/src/questpc.h"
#undef private
#include "../../libthecore/include/main.h"
#include <cassert>
#include <climits>
#include <cstdio>
#include <cstdlib>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError() { std::abort(); }
extern "C" void desc_test_allow_clock(void);
static void CheckDone(quest::PC& pc) {
    assert(!pc.GetAndResetDoneFlag());
    pc.SetSendDoneFlag();pc.SetSendDoneFlag();
    assert(pc.m_bShouldSendDone&&pc.GetAndResetDoneFlag());
    assert(!pc.m_bShouldSendDone&&!pc.GetAndResetDoneFlag());
    pc.m_bShouldSendDone=true;pc.SetID(17);
    assert(!pc.GetAndResetDoneFlag()&&pc.m_dwID==17);
}
static void CheckConfirm(quest::PC& pc) {
    const DWORD ids[]={0,1,0x7fffffffUL,0x80000000UL,0xffffffffUL};
    assert(!pc.m_bConfirmWait&&pc.m_dwWaitConfirmFromPID==0);
    for(unsigned i=0;i<sizeof(ids)/sizeof(ids[0]);++i) {
        pc.SetConfirmWait(ids[i]);
        assert(pc.m_bConfirmWait&&pc.m_dwWaitConfirmFromPID==ids[i]);
        for(unsigned j=0;j<sizeof(ids)/sizeof(ids[0]);++j) assert(pc.IsConfirmWait(ids[j])==(i==j));
        pc.ClearConfirmWait();pc.ClearConfirmWait();
        assert(!pc.m_bConfirmWait&&pc.m_dwWaitConfirmFromPID==ids[i]);
        for(unsigned j=0;j<sizeof(ids)/sizeof(ids[0]);++j) assert(!pc.IsConfirmWait(ids[j]));
    }
    pc.SetConfirmWait(123);pc.SetConfirmWait(456);
    assert(pc.IsConfirmWait(456)&&!pc.IsConfirmWait(123));pc.ClearConfirmWait();
}
static void CheckSendBits(quest::PC& pc) {
    const int initial[]={0,1,0x13579bdf,INT_MAX,INT_MIN,-1};
    const int masks[]={0,1,2,0x02468ace,INT_MAX,INT_MIN,-1};
    for(unsigned i=0;i<sizeof(initial)/sizeof(initial[0]);++i)
      for(unsigned j=0;j<sizeof(masks)/sizeof(masks[0]);++j) {
        pc.m_iSendToClient=initial[i];pc.SetSendFlag(masks[j]);
        assert(pc.m_iSendToClient==(initial[i]|masks[j]));
        int before=pc.m_iSendToClient;pc.SetSendFlag(masks[j]);assert(pc.m_iSendToClient==before);
      }
}
// Future addition before main in pc_state_helpers_regression.cpp. Existing
// private-access test header and actual PC graph are reused; no fake lifecycle.
static void CheckConstructedInputs(quest::PC& pc) {
    quest::QuestState state;
    assert(!state.co&&state.ico==0&&state.args==0&&state.suspend_state==0);
    assert(state.iIndex==0&&!state.bStart&&state.st==-1);
    assert(state._title.empty()&&state._clock_name.empty()&&state._counter_name.empty());
    assert(state._icon_file.empty()&&state.chat_scripts.empty());
    // Original constructor deliberately leaves these indeterminate.
    // Initialize BEFORE explicit or implicit memberwise copy; do not test-read defaults.
    state._clock_value=17;state._counter_value=-18;
    pc.m_RunningQuestState=&state;
    assert(pc.IsRunning()&&pc.GetRunningQuestState()==&state);
    state.ico=-23;state.args=-2;state.suspend_state=5;state.iIndex=37;
    state.bStart=true;state.st=-45;state._title="actual-state";
    state._clock_name="clock";state._counter_name="counter";state._icon_file="icon";
    state.chat_scripts.push_back(0); // Real vector input; no AArgScript allocation/ctor.
    quest::QuestState copy(state);
    assert(copy.co==state.co&&copy.ico==-23&&copy.args==-2&&copy.suspend_state==5);
    assert(copy.iIndex==37&&copy.bStart&&copy.st==-45);
    assert(copy._title=="actual-state"&&copy._clock_name=="clock"&&copy._counter_name=="counter");
    assert(copy._clock_value==17&&copy._counter_value==-18&&copy._icon_file=="icon");
    assert(copy.chat_scripts.size()==1&&copy.chat_scripts[0]==0);
    copy._title="independent";copy.chat_scripts.clear();
    assert(state._title=="actual-state"&&state.chat_scripts.size()==1);
    assert(pc.GetRunningQuestState()->ico==-23);
    pc.m_RunningQuestState=&copy;assert(pc.IsRunning()&&pc.GetRunningQuestState()==&copy);
    pc.m_RunningQuestState=0;assert(!pc.IsRunning()&&!pc.GetRunningQuestState());
    quest::RewardData none(quest::RewardData::REWARD_TYPE_NONE,0);
    quest::RewardData exp(quest::RewardData::REWARD_TYPE_EXP,0xffffffffUL);
    quest::RewardData item(quest::RewardData::REWARD_TYPE_ITEM,0x80000000UL,INT_MIN);
    assert(none.type==quest::RewardData::REWARD_TYPE_NONE&&!none.value1&&!none.value2);
    assert(exp.type==quest::RewardData::REWARD_TYPE_EXP&&exp.value1==0xffffffffUL&&!exp.value2);
    assert(item.type==quest::RewardData::REWARD_TYPE_ITEM&&item.value1==0x80000000UL&&item.value2==INT_MIN);
    pc.m_bIsGivenReward=false;assert(!pc.HasReward());
    pc.m_vRewardData.push_back(none);assert(pc.HasReward());
    pc.m_vRewardData.push_back(exp);pc.m_vRewardData.push_back(item);assert(pc.HasReward());
    pc.m_bIsGivenReward=true;assert(pc.HasReward());
    pc.m_vRewardData.clear();assert(pc.HasReward());
    pc.m_bIsGivenReward=false;assert(!pc.HasReward());
}
// Call CheckConstructedInputs(pc) in main after the initial empty/default checks.
// No VM-owned coroutine is allocated; string/vector destructors are actual STL.

int main() {
    desc_test_allow_clock();assert(sizeof(int)==4&&sizeof(DWORD)==4);
    quest::PC pc;
    assert(!pc.IsRunning()&&pc.GetRunningQuestState()==0);
    assert(pc.m_vRewardData.empty()&&!pc.HasReward());
    pc.m_bIsGivenReward=true;assert(pc.HasReward());pc.m_bIsGivenReward=false;assert(!pc.HasReward());
    CheckConstructedInputs(pc);
    pc.SetFlag("sentinel",37,false);pc.m_stCurQuest="sentinel-current";
    CheckDone(pc);CheckConfirm(pc);CheckSendBits(pc);
    assert(pc.GetFlag("sentinel")==37&&pc.m_FlagSaveMap["sentinel"]==37);
    assert(pc.m_stCurQuest=="sentinel-current"&&!pc.IsRunning()&&pc.GetRunningQuestState()==0);
    assert(pc.m_QuestInfo.empty()&&pc.m_TimerMap.empty()&&pc.m_vRewardData.empty());
    assert(!event_count());event_destroy();
    std::puts("PC state helpers passed: real full PC lifecycle; no VM/QuestState/RewardData constructor substitution");
}
// Non-null QuestState and nonempty reward vector runtime branches deliberately
// omitted: those original constructors are not recovered in this bounded group.
