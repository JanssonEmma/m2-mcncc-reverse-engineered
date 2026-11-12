// Real target/walking integration; external log, clock, transport-memory and abort seams only.
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/char_manager.h"
#include "../../game/src/sectree.h"
#include "../../game/src/packet.h"
#include "../../game/src/config.h"
#include "../../game/src/utils.h"
#include "../../libthecore/include/memcpy.h"
#include "../../libthecore/include/utils.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <csignal>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
extern "C" void _exit(int);
#endif
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" {
void desc_test_allow_clock(void);
void desc_test_set_millis(unsigned long);
extern int target_walk_clock_reads;
}
static std::vector<LPBUFFER> observedBuffers;
struct ExpectedCopy {
    LPBUFFER output;
    std::vector<BYTE> data;
    CHARACTER* recipient;
    ExpectedCopy(LPBUFFER b,const std::vector<BYTE>& d,CHARACTER* r=0):output(b),data(d),recipient(r) {}
};
static struct CopyContext {
    bool active,eraseCurrent,checkWalking;
    unsigned seen;
    CHARACTER* source;
    bool walking;
    DWORD walkStart;
    std::vector<ExpectedCopy> expected;
    CopyContext():active(false),eraseCurrent(false),checkWalking(false),seen(0),source(0),walking(false),walkStart(0) {}
} copy;
static void* ObserveCopy(void* out,const void* in,size_t bytes) {
    bool destination=false;
    if(copy.active) for(unsigned i=0;i<observedBuffers.size();++i)
        if(out==observedBuffers[i]->write_point) destination=true;
    if(copy.active && destination) {
        assert(copy.seen<copy.expected.size());
        const ExpectedCopy& expected=copy.expected[copy.seen];
        assert(out==expected.output->write_point && bytes==expected.data.size());
        assert(!std::memcmp(in,&expected.data[0],bytes));
        if(copy.checkWalking) {
            assert(copy.source->m_bNowWalking==copy.walking);
            assert(copy.source->m_dwWalkStartTime==copy.walkStart);
        }
        ++copy.seen;
        if(copy.eraseCurrent) {
            assert(expected.recipient);
            assert(copy.source->m_set_pkChrTargetedBy.erase(expected.recipient)==1);
        }
    }
    return std::memcpy(out,in,bytes);
}
static void Put4(std::vector<BYTE>& out,unsigned at,DWORD value) {
    for(unsigned i=0;i<4;++i) out[at+i]=static_cast<BYTE>(value>>(8*i));
}
static std::vector<BYTE> Target(DWORD vid,BYTE hp) {
    std::vector<BYTE> bytes(6,0);bytes[0]=63;Put4(bytes,1,vid);bytes[5]=hp;return bytes;
}
static std::vector<BYTE> Walk(DWORD vid,bool walking) {
    std::vector<BYTE> bytes(6,0);bytes[0]=111;Put4(bytes,1,vid);bytes[5]=walking?1:0;return bytes;
}
static std::vector<BYTE> Chat(bool walking) {
    // Existing real MonsterLog prefixes source VID1234 and keeps original CP949 bytes.
    const char* word=walking?"\xb0\xc8\xb4\xc2\xb4\xd9":"\xb6\xda\xb4\xd9";
    std::string text=std::string("1234)")+word;
    std::vector<BYTE> bytes(9+text.size()+1,0);
    bytes[0]=4;const unsigned length=static_cast<unsigned>(bytes.size());
    bytes[1]=static_cast<BYTE>(length);bytes[2]=static_cast<BYTE>(length>>8);
    bytes[3]=0;Put4(bytes,4,1234);bytes[8]=0;
    std::memcpy(&bytes[9],text.c_str(),text.size()+1);return bytes;
}
struct Fixture {
    quest::CQuestManager quests;
    CHARACTER_MANAGER manager;
    SECTREE sector;
    CHARACTER source,a,b,offline,monitor;
    DescOutputProbe ownDesc,aDesc,bDesc,monitorDesc;
    LPBUFFER own,first,second,monitored;
    LPFDWATCH watch;
    Fixture() {
        own=buffer_new(0,8192);first=buffer_new(0,8192);
        second=buffer_new(0,8192);monitored=buffer_new(0,8192);
        watch=fdwatch_new(16);
        ownDesc.Attach(watch,5,own);ownDesc.SetPacketState(5);
        aDesc.Attach(watch,6,first);aDesc.SetPacketState(5);
        bDesc.Attach(watch,7,second);bDesc.SetPacketState(5);
        monitorDesc.Attach(watch,8,monitored);monitorDesc.SetPacketState(5);
        a.BindDesc(&aDesc);b.BindDesc(&bDesc);monitor.BindDesc(&monitorDesc);
        source.m_vid=VID(1234,99);source.m_dwPlayerID=9876;
        source.m_stName="subject";a.m_stName="a";b.m_stName="b";offline.m_stName="offline";
        source.SetXYZ(0,0,0);monitor.SetXYZ(0,0,0);
        observedBuffers.push_back(own);observedBuffers.push_back(first);
        observedBuffers.push_back(second);observedBuffers.push_back(monitored);
        thecore_memcpy=ObserveCopy;
    }
    LPBUFFER Output(CHARACTER* ch) {
        if(ch==&source)return own;if(ch==&a)return first;
        if(ch==&b)return second;if(ch==&monitor)return monitored;
        assert(!"unexpected recipient");return 0;
    }
    void View() {
        source.m_map_view.clear();
        source.m_map_view[&b]=1;source.m_map_view[&offline]=1;source.m_map_view[&a]=1;
    }
    ~Fixture() {
        copy.active=false;thecore_memcpy=std::memcpy;
        manager.UnregisterForMonsterLog(&monitor);
        source.m_set_pkChrTargetedBy.clear();source.m_map_view.clear();source.SetSectree(0);
        source.BindDesc(0);a.BindDesc(0);b.BindDesc(0);monitor.BindDesc(0);
        ownDesc.Attach(0,-1,0);aDesc.Attach(0,-1,0);bDesc.Attach(0,-1,0);monitorDesc.Attach(0,-1,0);
        observedBuffers.clear();
        buffer_delete(own);buffer_delete(first);buffer_delete(second);buffer_delete(monitored);fdwatch_delete(watch);
    }
};
static void Begin(Fixture& f) {
    copy.active=false;copy.expected.clear();copy.seen=0;copy.eraseCurrent=false;
    copy.checkWalking=false;copy.source=&f.source;
    for(unsigned i=0;i<observedBuffers.size();++i) buffer_reset(observedBuffers[i]);
}
static void Expect(LPBUFFER buffer,const std::vector<BYTE>& data,CHARACTER* recipient=0) {
    copy.expected.push_back(ExpectedCopy(buffer,data,recipient));
}
static void ExpectView(Fixture& f,const std::vector<BYTE>& packet) {
    if(!f.source.GetSectree())return;
    if(!f.source.m_bIsObserver)
        for(CEntity::ENTITY_MAP::iterator it=f.source.m_map_view.begin();it!=f.source.m_map_view.end();++it)
            if(it->first->GetDesc()) Expect(f.Output(static_cast<CHARACTER*>(it->first)),packet);
    if(f.source.GetDesc())Expect(f.own,packet);
}
static void Finish() {
    copy.active=false;assert(copy.seen==copy.expected.size());
    for(unsigned i=0;i<observedBuffers.size();++i) {
        std::vector<BYTE> joined;
        for(unsigned j=0;j<copy.expected.size();++j)
            if(copy.expected[j].output==observedBuffers[i])
                joined.insert(joined.end(),copy.expected[j].data.begin(),copy.expected[j].data.end());
        assert(buffer_size(observedBuffers[i])==joined.size());
        if(!joined.empty())assert(!std::memcmp(buffer_read_peek(observedBuffers[i]),&joined[0],joined.size()));
    }
}
static void TargetSet(Fixture& f) {
    f.source.m_set_pkChrTargetedBy.clear();
    // Insertion and VID order deliberately do not determine pointer-set order.
    f.a.m_vid=VID(9000,1);f.b.m_vid=VID(1,2);f.monitor.m_vid=VID(300,3);
    f.source.m_set_pkChrTargetedBy.insert(&f.monitor);
    f.source.m_set_pkChrTargetedBy.insert(&f.b);f.source.m_set_pkChrTargetedBy.insert(&f.a);
}
static void ExpectedTargets(Fixture& f,BYTE percent) {
    for(std::set<CHARACTER*>::iterator it=f.source.m_set_pkChrTargetedBy.begin();it!=f.source.m_set_pkChrTargetedBy.end();++it)
        Expect(f.Output(*it),Target(static_cast<DWORD>(f.source.GetVID()),percent),*it);
}
static void CheckTargets(Fixture& f) {
    f.source.BindDesc(0);f.source.SetSectree(0);f.source.m_set_pkChrTargetedBy.clear();
    f.source.m_points.hp=50;f.source.m_pointsInstant.iMaxHP=0;
    Begin(f);copy.active=true;f.source.BroadcastTargetPacket();Finish(); // Must return before division.
    f.source.m_vid=VID(0x89abcdefUL,0x11223344UL);TargetSet(f);
    f.source.BindDesc(&f.ownDesc);f.source.m_bCharType=0;
    f.source.m_points.hp=INT_MIN;f.source.m_pointsInstant.iMaxHP=0;
    Begin(f);ExpectedTargets(f,0);copy.active=true;f.source.BroadcastTargetPacket();Finish();
    f.source.BindDesc(0);
    struct Row {int hp,maxHP;BYTE percent;} rows[]={
        {0,100,0},{1,3,33},{2,3,66},{100,100,100},{150,100,100},
        {-1,100,0},{-1,-1,100},{1,-1,0},{INT_MAX,100,0},
        {536870911,100,100},{536870912,100,0},{536870913,100,0},{INT_MIN,100,0}
    };
    for(unsigned i=0;i<sizeof(rows)/sizeof(*rows);++i) {
        f.source.m_points.hp=rows[i].hp;f.source.m_pointsInstant.iMaxHP=rows[i].maxHP;
        Begin(f);ExpectedTargets(f,rows[i].percent);copy.active=true;
        f.source.BroadcastTargetPacket();Finish();
        assert(f.source.m_points.hp==rows[i].hp&&f.source.m_pointsInstant.iMaxHP==rows[i].maxHP);
    }
    // A transport-copy callback erases each CURRENT recipient after iterator advance.
    f.source.m_points.hp=25;f.source.m_pointsInstant.iMaxHP=100;
    Begin(f);ExpectedTargets(f,25);copy.eraseCurrent=true;copy.active=true;
    f.source.BroadcastTargetPacket();Finish();assert(f.source.m_set_pkChrTargetedBy.empty());
}
static void WalkingCall(Fixture& f,bool requested,bool action,DWORD now,bool chat=false) {
    const DWORD oldStart=f.source.m_dwWalkStartTime;
    const bool preference=f.source.m_bWalking;
    desc_test_set_millis(now);const int clocks=target_walk_clock_reads;
    Begin(f);
    if(action) {
        ExpectView(f,Walk(1234,requested));
        if(chat)Expect(f.monitored,Chat(requested));
    }
    copy.checkWalking=true;copy.walking=requested;
    copy.walkStart=action&&requested?now:oldStart;
    copy.active=true;f.source.SetNowWalking(requested);Finish();
    assert(f.source.m_bNowWalking==requested&&f.source.m_bWalking==preference);
    assert(f.source.m_dwWalkStartTime==(action&&requested?now:oldStart));
    assert(target_walk_clock_reads==clocks+(action&&requested?1:0));
}
static void CheckWalking(Fixture& f) {
    f.source.m_vid=VID(1234,55);f.source.BindDesc(0);f.source.SetSectree(&f.sector);
    f.source.m_bIsObserver=false;f.View();test_server=0;
    for(unsigned type=0;type<256;++type) for(unsigned initial=0;initial<2;++initial)
        for(unsigned requested=0;requested<2;++requested) {
            f.source.m_bCharType=static_cast<BYTE>(type);
            f.source.m_bNowWalking=initial!=0;f.source.m_bWalking=requested==0;
            f.source.m_points.stamina=0;f.source.m_dwWalkStartTime=0x10203040UL;
            const bool action=initial!=requested||type!=6;
            WalkingCall(f,requested!=0,action,1000+type*4+initial*2+requested);
        }
    // Exact real PacketView sector/observer/own-descriptor fanout.
    for(unsigned sector=0;sector<2;++sector) for(unsigned observer=0;observer<2;++observer)
        for(unsigned own=0;own<2;++own) {
            f.source.SetSectree(sector?&f.sector:0);f.source.m_bIsObserver=observer!=0;
            f.source.BindDesc(own?&f.ownDesc:0);
            f.source.m_bCharType=6;f.source.m_bNowWalking=false;f.source.m_dwWalkStartTime=77;
            WalkingCall(f,true,true,2000+sector*4+observer*2+own);
        }

    // Real MonsterLog -> TEMP_BUFFER -> CHARACTER_MANAGER::PacketMonsterLog -> DESC::Packet.
    f.manager.RegisterForMonsterLog(&f.monitor);test_server=1;
    f.source.BindDesc(0);f.source.SetSectree(&f.sector);f.source.m_bIsObserver=false;
    f.source.m_bCharType=0;f.monitor.SetXYZ(0,0,0);
    for(unsigned initial=0;initial<2;++initial)for(unsigned requested=0;requested<2;++requested) {
        f.source.m_bNowWalking=initial!=0;f.source.m_dwWalkStartTime=91;
        WalkingCall(f,requested!=0,true,3000+initial*2+requested,true);
    }
    // Actual DISTANCE_APPROX(x,0) rescales x by246/256.
    assert(DISTANCE_APPROX(6244,0)==6000&&DISTANCE_APPROX(6245,0)==6001);
    // Manager distance<=6000 includesX6244, excludesX6245.
    f.monitor.SetXYZ(6245,0,0);f.source.m_bNowWalking=false;
    WalkingCall(f,true,true,3100,false);
    f.monitor.SetXYZ(6244,0,0);f.source.m_bNowWalking=true;
    WalkingCall(f,false,true,3101,true);
    f.monitor.SetXYZ(0,0,0);
    // Raw char type, not descriptor presence, suppresses the call on PC type6.
    f.source.m_bCharType=6;f.source.m_bNowWalking=false;
    WalkingCall(f,true,true,3200,false);
    // A non-PC actor with a descriptor still acts, but real MonsterLog returns early.
    f.source.m_bCharType=0;f.source.BindDesc(&f.ownDesc);f.source.m_bNowWalking=true;
    WalkingCall(f,true,true,3201,false);
    // No sector and observer mode do not suppress the separate monitored log.
    f.source.BindDesc(0);f.source.SetSectree(0);f.source.m_bNowWalking=false;
    WalkingCall(f,true,true,3300,true);
    f.source.SetSectree(&f.sector);f.source.m_bIsObserver=true;f.source.m_bNowWalking=true;
    WalkingCall(f,false,true,3301,true);
    f.manager.UnregisterForMonsterLog(&f.monitor);test_server=0;
    f.source.m_bIsObserver=false;f.source.m_map_view.clear();f.source.SetSectree(0);
}

static volatile std::sig_atomic_t fatalSeen;
static FILE* fatalMarker;
static CHARACTER* fatalRecipient;
extern "C" void target_walk_observe_missing_desc(const char* function,int line,const char* format,const char* name,void* recipient) {
    assert(fatalMarker && !fatalSeen && recipient==fatalRecipient);
    assert(!std::strcmp(function,"BroadcastTargetPacket")&&line==4801);
    assert(!std::strcmp(format,"%s %p does not have desc")&&!std::strcmp(name,"missing-target"));
    assert(std::fputs("BroadcastTargetPacket:4801:missing-target\n",fatalMarker)>=0);
    assert(std::fflush(fatalMarker)==0);fatalSeen=1;
}
static void ChildAbort(int signal) {
    // The real production abort must raise SIGABRT AFTER the diagnostic.
    // Immediate child exit avoids CRT report dialogs; handler never returns.
    const int result=signal==SIGABRT&&fatalSeen?87:88;
#ifdef _WIN32
    _exit(result);
#else
    _exit(result);
#endif
}
static int MissingDescriptorChild(const char* marker) {
    fatalMarker=std::fopen(marker,"wb");assert(fatalMarker);
    std::signal(SIGABRT,ChildAbort);desc_test_allow_clock();test_server=0;
    quest::CQuestManager quests;CHARACTER source,missing;
    missing.m_stName="missing-target";fatalRecipient=&missing;
    source.m_points.hp=50;source.m_pointsInstant.iMaxHP=100;source.m_vid=VID(1,2);
    source.m_set_pkChrTargetedBy.insert(&missing);
    source.BroadcastTargetPacket();
    return 89; // Returning normally is a test failure.
}
#ifndef _WIN32
static std::string QuoteShell(const std::string& text) {
    std::string result="'";
    for(unsigned i=0;i<text.size();++i) result+=text[i]=='\''?"'\\''":std::string(1,text[i]);
    return result+"'";
}
#endif
static void CheckFatalChild(const char* executable) {
    char marker[128];
#ifdef _WIN32
    std::snprintf(marker,sizeof(marker),"target_walking_abort_%d.txt",_getpid());
    std::remove(marker);
    const std::string childArg0=std::string("\"")+executable+"\"";
    const int result=static_cast<int>(_spawnl(_P_WAIT,executable,childArg0.c_str(),"--missing-desc-child",marker,static_cast<const char*>(0)));
#else
    std::snprintf(marker,sizeof(marker),"target_walking_abort_%ld.txt",static_cast<long>(getpid()));
    const std::string command=QuoteShell(executable)+" --missing-desc-child "+QuoteShell(marker);
    std::remove(marker);
    const int result=std::system(command.c_str());
#endif
    assert(result==87||result==(87<<8));
    FILE* log=std::fopen(marker,"rb");assert(log);char text[128]={0};
    const size_t n=std::fread(text,1,sizeof(text)-1,log);std::fclose(log);
    assert(std::string(text,n)=="BroadcastTargetPacket:4801:missing-target\n");
    assert(std::remove(marker)==0);
}
int main(int argc,char** argv) {
    assert(sizeof(int)==4&&sizeof(long)==4&&sizeof(DWORD)==4);
    assert(sizeof(TPacketGCTarget)==6&&sizeof(TPacketGCWalkMode)==6&&sizeof(TPacketGCChat)==9);
    if(argc==3&&!std::strcmp(argv[1],"--missing-desc-child"))return MissingDescriptorChild(argv[2]);
    assert(argc==1);
    void (CHARACTER::*targetSignature)()=&CHARACTER::BroadcastTargetPacket;
    void (CHARACTER::*walkingSignature)(bool)=&CHARACTER::SetNowWalking;
    (void)targetSignature;(void)walkingSignature;
    desc_test_allow_clock();test_server=0;
    {Fixture fixture;CheckTargets(fixture);CheckWalking(fixture);}
    CheckFatalChild(argv[0]);
    std::puts("target/walking passed: real pointer-set target fanout and fatal path, all raw types, clock/view behavior and CP949 monitored logs");
}
