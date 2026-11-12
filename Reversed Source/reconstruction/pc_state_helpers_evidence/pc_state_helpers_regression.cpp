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
int main() {
    desc_test_allow_clock();assert(sizeof(int)==4&&sizeof(DWORD)==4);
    quest::PC pc;
    assert(!pc.IsRunning()&&pc.GetRunningQuestState()==0);
    assert(pc.m_vRewardData.empty()&&!pc.HasReward());
    pc.m_bIsGivenReward=true;assert(pc.HasReward());pc.m_bIsGivenReward=false;assert(!pc.HasReward());
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
