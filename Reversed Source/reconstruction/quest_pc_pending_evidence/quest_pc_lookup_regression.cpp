#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/questmanager.h"
#include "../../game/src/questevent.h"
#include "../../game/src/char_manager.h"
#include "../../game/src/char.h"
#undef protected
#undef private
#include "../../libthecore/include/main.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError() { std::abort(); }
static std::vector<DWORD> expectedIDs;
static unsigned freeAt;
static bool observing;
static quest::PC* observedPC;
static LPEVENT processingHandle;
static DWORD processingID;
static int forcedErrors;
static long NeverRun(LPEVENT) { assert(!"Canceled timer callback executed");return 0; }
extern "C" void pc_test_clear_log(int count) {
    if(observing) assert(observedPC && count==static_cast<int>(observedPC->m_TimerMap.size()));
}
extern "C" void pc_test_free_log(DWORD id) {
    if(!observing)return;
    assert(freeAt<expectedIDs.size()&&id==expectedIDs[freeAt++]);
    quest::quest_event_info* info=static_cast<quest::quest_event_info*>(FindEventInfo(id));
    assert(info&&info->name==0); // Actual registry still owns record, name cleared before free.
}
extern "C" void pc_test_event_error(const char* fn,int line,const char* fmt) {
    assert(!std::strcmp(fn,"event_process")&&line==211);
    assert(!std::strcmp(fmt,"EVENT BUG! pointer may invalid! create in %s:%d"));
    assert(processingHandle==0&&FindEventInfo(processingID));++forcedErrors;
}
static LPEVENT Timer(unsigned int pid,const char* name,int delay,DWORD& id) {
    quest::quest_event_info* info;
    id=__AllocEventInfo(sizeof(*info),reinterpret_cast<void**>(&info));
    info->time_cycle=17;info->player_id=pid;info->npc_id=88;
    info->name=new char[std::strlen(name)+1];std::strcpy(info->name,name);
    LPEVENT event=event_create_ex(NeverRun,id,delay,const_cast<char*>("pc-regression"),1);
    assert(!event->info&&FindEventInfo(id)==info);
    return event;
}
static long CancelProcessing(LPEVENT event) {
    assert(event==processingHandle&&event->info==FindEventInfo(processingID));
    observedPC->ClearTimer();
    assert(!processingHandle || observedPC->m_TimerMap.empty());
    // PC's map value is the owning handle canceled by ClearTimer.
    processingHandle=0;
    assert(event->is_force_to_end&&FindEventInfo(processingID));
    return 99;
}
static void Lifecycle() {
    quest::PC pc;
    assert(!pc.m_bIsGivenReward&&!pc.m_bShouldSendDone&&pc.m_dwID==0);
    assert(!pc.m_RunningQuestState&&pc.m_iSendToClient==0&&!pc.IsLoaded());
    assert(!pc.m_dwWaitConfirmFromPID&&!pc.m_bConfirmWait);
    assert(pc.m_QuestStateChange.empty()&&pc.m_vRewardData.empty()&&pc.m_QuestInfo.empty());
    assert(pc.m_FlagMap.empty()&&pc.m_FlagSaveMap.empty()&&pc.m_TimerMap.empty());
    // Original m_iLastState is indeterminate; never inspect it.
    pc.m_bShouldSendDone=true;pc.m_bLoaded=true;pc.m_stCurQuest="active";
    pc.m_FlagMap["a"]=11;pc.m_FlagSaveMap["a"]=12;
    pc.SetID(0xffffffffUL);
    assert(pc.m_dwID==0xffffffffUL&&!pc.m_bShouldSendDone&&pc.IsLoaded());
    assert(&pc.GetCurrentQuestName()==&pc.m_stCurQuest&&pc.GetCurrentQuestName()=="active");
    DWORD ids[3];LPEVENT a=Timer(77,"a-name",20,ids[0]);
    LPEVENT m=Timer(77,"m-name",20,ids[1]);LPEVENT z=Timer(77,"z-name",20,ids[2]);
    pc.m_TimerMap["z"]=z;pc.m_TimerMap[std::string("m\0key",5)]=m;pc.m_TimerMap["a"]=a;
    expectedIDs.assign(ids,ids+3);freeAt=0;observedPC=&pc;observing=true;
    pc.Destroy();observing=false;
    assert(freeAt==3&&pc.m_TimerMap.empty()&&event_count()==3);
    for(int i=0;i<3;++i)assert(!FindEventInfo(ids[i]));
    assert(pc.m_dwID==0xffffffffUL&&pc.IsLoaded()&&pc.m_stCurQuest=="active");
    assert(pc.m_FlagMap["a"]==11&&pc.m_FlagSaveMap["a"]==12);
    assert(event_process(20)==0&&event_count()==0);
    pc.Destroy();assert(pc.m_FlagMap.size()==1); // Repeat only timer cleanup.
    // Null name still cancels registered info.
    DWORD id;LPEVENT e=Timer(77,"unused",1,id);
    quest::quest_event_info* info=static_cast<quest::quest_event_info*>(FindEventInfo(id));
    delete[] info->name;info->name=0;quest::CancelTimerEvent(&e);
    assert(!e&&!FindEventInfo(id));assert(event_process(21)==0&&!event_count());
    // Complete raw EVENT processing-state input for absent numeric registry ID.
    EVENT raw;std::memset(&raw,0,sizeof(raw));raw.dwInfoID=0xffffffffUL;raw.is_processing=1;
    LPEVENT handle=&raw;quest::CancelTimerEvent(&handle);
    assert(!handle&&raw.is_force_to_end);
}
static void Processing() {
    quest::PC pc;observedPC=&pc;
    quest::quest_event_info* info;
    processingID=__AllocEventInfo(sizeof(*info),reinterpret_cast<void**>(&info));
    info->time_cycle=1;info->player_id=77;info->npc_id=88;
    info->name=new char[4];std::strcpy(info->name,"run");
    thecore_heart->pulse=30;
    processingHandle=event_create_ex(CancelProcessing,processingID,1,const_cast<char*>("pc-regression"),2);
    pc.m_TimerMap["processing"]=processingHandle;
    expectedIDs.assign(1,processingID);freeAt=0;observing=true;forcedErrors=0;
    assert(event_process(31)==1);
    observing=false;assert(forcedErrors==1&&freeAt==1&&!FindEventInfo(processingID)&&pc.m_TimerMap.empty());
}
static void Lookup(quest::CQuestManager& qm,CHARACTER_MANAGER& chars,CHARACTER& a,CHARACTER& b) {
    const unsigned int keys[]={0,1,0x7fffffffU,0x80000000U,UINT_MAX};
    for(unsigned i=0;i<5;++i) {
        quest::PC* pc=qm.GetPCForce(keys[i]);
        assert(pc->m_dwID==keys[i]&&!pc->m_bShouldSendDone&&!pc->IsLoaded());
        pc->m_bShouldSendDone=true;pc->m_bLoaded=true;pc->m_FlagMap["keep"]=42;
        assert(qm.GetPCForce(keys[i])==pc&&pc->m_bShouldSendDone&&pc->IsLoaded()&&pc->m_FlagMap["keep"]==42);
    }
    // Borrowed actual CHARACTER objects in actual manager PID map; clear before teardown.
    chars.m_map_pkChrByPID[1]=&a;chars.m_map_pkChrByPID[UINT_MAX]=&b;chars.m_map_pkChrByPID[77]=0;
    quest::PC* sentinel=qm.GetPCForce(999);
    CDungeon* dungeon=reinterpret_cast<CDungeon*>(&b);
    qm.m_pCurrentPC=sentinel;qm.m_pCurrentCharacter=&b;qm.m_pSelectedDungeon=dungeon;
    size_t before=qm.m_mapPC.size();
    assert(!qm.GetPC(77)&&!qm.GetPC(888)&&!qm.GetPC(0)); // Includes existing PC0 without character.
    assert(qm.m_mapPC.size()==before&&qm.m_pCurrentPC==sentinel&&qm.m_pCurrentCharacter==&b&&qm.m_pSelectedDungeon==dungeon);
    assert(qm.GetPC(1)==qm.GetPCForce(1)&&qm.m_pCurrentCharacter==&a&&!qm.m_pSelectedDungeon);
    assert(qm.GetPC(UINT_MAX)==qm.GetPCForce(UINT_MAX)&&qm.m_pCurrentCharacter==&b);
    chars.m_map_pkChrByPID.clear();
}
int main() {
    assert(sizeof(DWORD)==4&&sizeof(unsigned int)==4);
    // Actual manager constructor initializes ONLY L; context set explicitly in Lookup.
    quest::CQuestManager qm;assert(qm.L==0);
    CHARACTER_MANAGER chars;CHARACTER a,b;
    thecore_heart->pulse=0;
    Lifecycle();Processing();Lookup(qm,chars,a,b);
    qm.m_mapPC.clear(); // Actual PC destructors, no fake VM cleanup.
    event_destroy();
    std::puts("PC lifecycle/lookup passed: full records, real timer registry/scheduler and PID map; Lua ownership not exercised");
}
