#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/questpc.h"
#include "../../game/src/questevent.h"
#undef protected
#undef private
#include "../../game/src/config.h"
#include "../../libthecore/include/main.h"
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError() { std::abort(); }
extern "C" void desc_test_allow_clock(void);
struct Log {
    int kind,level,value;std::string text;void* pointer;unsigned long id;
    Log(int k,int l,const char* t,int v,void* p,unsigned long i):kind(k),level(l),value(v),text(t?t:""),pointer(p),id(i){}
};
static std::vector<Log> logs;
static quest::PC* active;
static std::string activeKey;
static int mode,pulse,executed;
static LPEVENT replacement;
static bool inFinish;
extern "C" void pc_storage_log(int kind,int level,const char* text,int value,void* pointer,unsigned long id) {
    logs.push_back(Log(kind,level,text,value,pointer,id));
    if(kind==8) { // Core FreeEventInfo logs before its actual record free.
        quest::quest_event_info* info=static_cast<quest::quest_event_info*>(FindEventInfo(id));
        assert(info&&info->name==0);
    }
    if(!active)return;
    if(mode==1&&kind==1) {active->m_FlagMap[activeKey]=value;mode=0;}
    else if(mode==2&&kind==2&&level==1) {active->m_FlagMap[activeKey]=9;test_server=1;mode=3;}
    else if(mode==3&&kind==2&&level==0) {active->m_FlagMap[activeKey]=10;mode=0;}
    else if(mode==4&&kind==3) {active->m_TimerMap[activeKey]=replacement;mode=0;}
    else if(mode==5&&kind==4) {active->m_TimerMap.insert(std::make_pair(activeKey,replacement));mode=0;}
}
static void Begin(quest::PC& pc,const std::string& key="") {active=&pc;activeKey=key;mode=0;logs.clear();}
static int KindCount(int kind) {int n=0;for(unsigned i=0;i<logs.size();++i)if(logs[i].kind==kind)++n;return n;}
static void Flags() {
    quest::PC pc;Begin(pc);
    for(int debug=0;debug<2;++debug) {
        test_server=debug;pc.m_FlagMap.clear();pc.m_FlagSaveMap.clear();
        Begin(pc);assert(pc.GetFlag("missing")==0&&logs.empty()&&pc.m_FlagMap.empty());
        assert(!pc.DeleteFlag("missing")&&pc.m_FlagSaveMap.empty());
        pc.SetFlag("key",7,false);assert(pc.m_FlagMap["key"]==7&&pc.m_FlagSaveMap["key"]==7);
        assert(logs.back().kind==1&&logs.back().level==(debug?0:1)&&logs.back().value==7);
        pc.SetFlag("key",8,true);assert(pc.m_FlagMap["key"]==8&&pc.m_FlagSaveMap["key"]==7);
        pc.SetFlag("key",8,false);assert(pc.m_FlagSaveMap["key"]==7); // same current value forces skip
        pc.SetFlag("key",0,true);assert(!pc.m_FlagMap.count("key")&&pc.m_FlagSaveMap["key"]==0); // skip ignored for delete
        size_t queued=pc.m_FlagSaveMap.size();pc.SetFlag("absent",0,false);
        assert(pc.m_FlagSaveMap.size()==queued&&!pc.m_FlagSaveMap.count("absent"));
        pc.m_FlagMap["existing-zero"]=0;logs.clear();assert(pc.GetFlag("existing-zero")==0);
        assert(KindCount(2)==(debug?2:1));
        assert(logs[0].level==1&&(!debug||logs[1].level==0));
        assert(pc.DeleteFlag("existing-zero")&&pc.m_FlagSaveMap["existing-zero"]==0);
        std::string binary("A\0B",3);
        pc.SetFlag("",INT_MIN,false);pc.SetFlag(binary,INT_MAX,false);pc.SetFlag("A",-17,false);
        assert(pc.GetFlag("")==INT_MIN&&pc.GetFlag(binary)==INT_MAX&&pc.GetFlag("A")==-17);
        assert(pc.m_FlagSaveMap[binary]==INT_MAX&&pc.m_FlagSaveMap["A"]==-17);
        // Actual private SaveFlag is queue-only and preserves node identity.
        std::map<std::string,int> before=pc.m_FlagMap;
        pc.SaveFlag("queue",INT_MIN);int* location=&pc.m_FlagSaveMap.find("queue")->second;
        pc.SaveFlag("queue",INT_MAX);pc.SaveFlag("queue",INT_MAX);pc.SaveFlag("queue",0);
        assert(&pc.m_FlagSaveMap.find("queue")->second==location&&*location==0&&pc.m_FlagMap==before);
    }
    test_server=0;pc.m_FlagMap.clear();pc.m_FlagSaveMap.clear();
    Begin(pc,"before-log");mode=1;pc.SetFlag(activeKey,5,false);
    assert(mode==0&&pc.m_FlagMap[activeKey]==5&&pc.m_FlagSaveMap.empty()); // logger ran before find
    pc.m_FlagMap["reload"]=8;Begin(pc,"reload");mode=2;
    assert(pc.GetFlag(activeKey)==10&&mode==0&&logs.size()==2);
    assert(logs[0].level==1&&logs[0].value==8&&logs[1].level==0&&logs[1].value==9);
    test_server=0;active=0;
}
static long NeverRun(LPEVENT) {assert(!"Canceled fixture timer executed");return 0;}
static long Finishes(LPEVENT event) {
    quest::quest_event_info* info=static_cast<quest::quest_event_info*>(event->info);
    assert(info&&info==FindEventInfo(event->dwInfoID));
    if(inFinish) {active->RemoveTimerNotCancel(activeKey);assert(!event->is_force_to_end);}
    delete[] info->name;info->name=0;++executed;return 0;
}
static long RemovesProcessing(LPEVENT event) {
    active->RemoveTimer(activeKey);
    assert(event->is_force_to_end&&FindEventInfo(event->dwInfoID));
    assert(static_cast<quest::quest_event_info*>(event->info)->name==0);
    ++executed;return 0;
}
static LPEVENT Timer(const char* name,DWORD& id,TEVENTFUNC function=NeverRun) {
    quest::quest_event_info* info;
    id=__AllocEventInfo(sizeof(*info),reinterpret_cast<void**>(&info));
    info->time_cycle=1;info->player_id=77;info->npc_id=88;
    info->name=new char[std::strlen(name)+1];std::strcpy(info->name,name);
    return event_create_ex(function,id,5,const_cast<char*>("pc-storage-regression"),1);
}
static int Advance() {pulse+=10;thecore_heart->pulse=pulse;return event_process(pulse);}
static void TimerCases() {
    quest::PC pc;DWORD aID,bID;
    Begin(pc,"missing");pc.RemoveTimer(activeKey);
    assert(logs.size()==1&&logs[0].kind==4&&logs[0].level==1&&logs[0].value==0);
    logs.clear();pc.RemoveTimerNotCancel(activeKey);
    assert(logs.size()==1&&logs[0].kind==5&&logs[0].value==0);
    LPEVENT a=Timer("a",aID);Begin(pc,"replace");pc.AddTimer(activeKey,a);
    assert(logs.size()==2&&logs[0].kind==4&&logs[1].kind==6&&logs[1].pointer==a&&pc.m_TimerMap[activeKey]==a);
    LPEVENT b=Timer("b",bID);Begin(pc,"replace");pc.AddTimer(activeKey,b);
    assert(!FindEventInfo(aID)&&FindEventInfo(bID)&&pc.m_TimerMap[activeKey]==b);
    assert(logs.size()==4&&logs[0].kind==3&&logs[0].pointer==a&&logs[1].kind==8&&logs[1].id==aID&&logs[2].kind==4&&logs[3].kind==6);
    Begin(pc,"replace");pc.RemoveTimerNotCancel(activeKey);
    assert(pc.m_TimerMap.empty()&&FindEventInfo(bID)&&logs.size()==2&&logs[0].kind==7&&logs[1].kind==5);
    quest::CancelTimerEvent(&b);assert(!b&&!FindEventInfo(bID));assert(Advance()==0&&!event_count());
    // Logger changes LIVE map value; actual RemoveTimer cancels replacement, not logged pointer.
    a=Timer("old",aID);b=Timer("replacement",bID);Begin(pc,"live");pc.AddTimer(activeKey,a);
    Begin(pc,"live");replacement=b;mode=4;pc.RemoveTimer(activeKey);
    assert(mode==0&&FindEventInfo(aID)&&!FindEventInfo(bID)&&pc.m_TimerMap.empty());
    assert(logs[0].kind==3&&logs[0].pointer==a&&logs[1].kind==8&&logs[1].id==bID);
    quest::CancelTimerEvent(&a);assert(Advance()==0&&!event_count());
    // Logger inserts during RemoveTimer's final size log; AddTimer INSERT must fail,
    // retain the competing pointer and still log the supplied unstored pointer.
    a=Timer("supplied",aID);b=Timer("competing",bID);Begin(pc,"collision");replacement=b;mode=5;pc.AddTimer(activeKey,a);
    assert(mode==0&&pc.m_TimerMap[activeKey]==b&&FindEventInfo(aID)&&FindEventInfo(bID));
    assert(logs.back().kind==6&&logs.back().pointer==a&&logs.back().value==1);
    pc.RemoveTimer(activeKey);quest::CancelTimerEvent(&a);assert(Advance()==0&&!event_count());
    // NULL accepted by AddTimer; erase-without-cancel is safe before destructor.
    Begin(pc,"null");pc.AddTimer(activeKey,0);assert(pc.m_TimerMap.count(activeKey)&&pc.m_TimerMap[activeKey]==0);
    pc.RemoveTimerNotCancel(activeKey);assert(pc.m_TimerMap.empty()&&KindCount(8)==0);
    // Original same-pointer replacement stores already-canceled pooled EVENT.
    a=Timer("same",aID);Begin(pc,"same");pc.AddTimer(activeKey,a);pc.AddTimer(activeKey,a);
    assert(pc.m_TimerMap[activeKey]==a&&!FindEventInfo(aID));
    pc.RemoveTimerNotCancel(activeKey);assert(Advance()==0&&!event_count());
    // Binary keys independent even though diagnostics display C strings.
    std::string binary("T\0x",3);a=Timer("binary",aID);b=Timer("prefix",bID);
    Begin(pc,binary);pc.AddTimer(binary,a);pc.AddTimer("T",b);pc.RemoveTimer(binary);
    assert(!FindEventInfo(aID)&&FindEventInfo(bID)&&pc.m_TimerMap.size()==1);
    pc.RemoveTimer("T");assert(Advance()==0&&!event_count());
    // Detached real event still executes its TEST workload and engine frees info.
    executed=0;inFinish=false;a=Timer("detached",aID,Finishes);Begin(pc,"detached");pc.AddTimer(activeKey,a);pc.RemoveTimerNotCancel(activeKey);
    assert(FindEventInfo(aID)&&pc.m_TimerMap.empty()&&Advance()==1&&executed==1&&!FindEventInfo(aID));
    // Processing no-cancel removes ownership without force; processing cancel forces end.
    executed=0;inFinish=true;a=Timer("processing-no-cancel",aID,Finishes);Begin(pc,"processing");pc.AddTimer(activeKey,a);
    assert(Advance()==1&&executed==1&&pc.m_TimerMap.empty()&&!FindEventInfo(aID));
    executed=0;inFinish=false;a=Timer("processing-cancel",aID,RemovesProcessing);Begin(pc,"processing");pc.AddTimer(activeKey,a);
    assert(Advance()==1&&executed==1&&pc.m_TimerMap.empty()&&!FindEventInfo(aID));
    assert(!event_count());active=0;
}
int main() {
    assert(sizeof(int)==4&&sizeof(DWORD)==4);
    desc_test_allow_clock();thecore_heart->pulse=0;pulse=0;test_server=0;
    Flags();TimerCases();event_destroy();
    std::puts("PC flags/timers passed: actual maps, callbacks, registry and scheduler; pending-save only, no VM/DB claim");
}
