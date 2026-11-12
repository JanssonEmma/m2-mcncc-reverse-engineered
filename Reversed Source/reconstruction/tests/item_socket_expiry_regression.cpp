#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/item.h"
#include "../../game/src/item_manager.h"
#include "../../game/src/char.h"
#include "../../game/src/sectree.h"
#include "../../game/src/questmanager.h"
#include "../../game/src/log.h"
#undef protected
#undef private
#include "../../game/src/dev_log.h"
#include "../../game/src/config.h"
#include "../../game/src/packet.h"
#include "../../libthecore/include/main.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include "item_socket_support.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
extern "C" void (*desc_test_event_force_error)(void);
void ShutdownOnFatalError() { std::abort(); }
struct Context {
    bool active,dirty,packet,alignment,timer,debug;
    int index,serial;
    long value,previous;
    DWORD wire;
    CItem* item; ITEM_MANAGER* manager; LogManager* logger; CHARACTER* owner;
    LPBUFFER output;
    std::vector<std::string> steps;
    std::string developer;
} static ctx;
static DWORD Read4(const BYTE* b) { return static_cast<DWORD>(b[0]) | static_cast<DWORD>(b[1])<<8 | static_cast<DWORD>(b[2])<<16 | static_cast<DWORD>(b[3])<<24; }
static void AfterSave() {
    assert(ctx.active && ctx.item->GetSockets()[ctx.index]==ctx.value);
    if(ctx.manager) assert((ctx.manager->m_set_pkItemForDelayedSave.count(ctx.item)!=0)==ctx.dirty);
    else assert(!ITEM_MANAGER::instance_ptr() && ctx.item->GetSkipSave());
    if(ctx.output) assert(buffer_size(ctx.output)==(ctx.packet?36:0)+(ctx.alignment?35:0));
    assert((ctx.item->m_pkUniqueExpireEvent!=0)==ctx.timer);
}
static void* Copy(void* out,const void* in,size_t count) {
    if(ctx.active) {
        const BYTE* b=static_cast<const BYTE*>(in);
        if(count==35 && b[0]==19) {
            assert(ctx.alignment && ctx.owner->m_iAlignment==ctx.owner->m_iRealAlignment && ctx.item->GetSockets()[ctx.index]==ctx.previous);
            TPacketGCCharacterUpdate p; std::memcpy(&p,in,sizeof(p)); assert(p.sAlignment==-1234 && p.dwVID==7000UL); ctx.steps.push_back("alignment");
        } else {
            assert(ctx.packet && count==36 && b[0]==25 && b[1]==107 && b[2]==44);
            assert(Read4(b+3+ctx.index*4)==ctx.wire && ctx.item->GetSockets()[ctx.index]==ctx.value);
            if(ctx.manager) assert(!ctx.manager->m_set_pkItemForDelayedSave.count(ctx.item));
            assert(ctx.logger->m_sql.m_queue_query.empty()); ctx.steps.push_back("packet");
        }
    }
    return std::memcpy(out,in,count);
}
struct Runtime : SqlRuntime {
    MYSQL* db;
    int depth;
    Runtime() : db(0),depth(0) {}
    virtual int SemInit(sem_t* sem,int shared,unsigned int value) { assert(!shared && !value); *sem=reinterpret_cast<sem_t>(1); return 0; }
    virtual int SemDestroy(sem_t* sem) { assert(sem && *sem); return 0; }
    virtual int MutexInit(pthread_mutex_t* mutex,const pthread_mutexattr_t* attr) { assert(mutex && !attr); std::memset(mutex,0,sizeof(*mutex)); return 0; }
    virtual int Create(pthread_t*,const pthread_attr_t* attr,void*(*entry)(void*),void* argument) { assert(!attr && entry==AsyncSQLThread && argument==&ctx.logger->m_sql); return 11; }
    virtual void Close(MYSQL* value) { assert(value==db); }
    virtual int MutexDestroy(pthread_mutex_t* mutex) { assert(mutex && !depth); return 0; }
    virtual unsigned long Escape(MYSQL* value,char* to,const char* from,unsigned long count) {
        assert(value==db && !count && !*from); AfterSave(); ctx.steps.push_back("escape"); to[0]=0; return 0;
    }
    virtual int MutexLock(pthread_mutex_t* mutex) {
        assert(ctx.active && mutex==ctx.logger->m_sql.m_mtxQuery && !depth); AfterSave(); ++depth;
        assert(ctx.logger->m_sql.m_queue_query.empty()); ctx.steps.push_back("lock"); return 0;
    }
    virtual int SemPost(sem_t* sem) {
        assert(sem && *sem && depth==1 && ctx.logger->m_sql.m_queue_query.size()==1); AfterSave(); ctx.steps.push_back("post"); return 0;
    }
    virtual int MutexUnlock(pthread_mutex_t* mutex) {
        assert(mutex==ctx.logger->m_sql.m_mtxQuery && depth==1); AfterSave(); --depth; ctx.steps.push_back("unlock"); return 0;
    }
};
struct DevIo : ItemDevIo {
    timeval* stamp;
    virtual int Open(const char* file,int flags,int mode) { AfterSave(); assert(ctx.debug && std::string(file)=="DEV_LOG.log" && flags==0x209 && mode==0666); ctx.steps.push_back("dev-open"); return 7; }
    virtual int Time(timeval* value,void* zone) { assert(!zone); stamp=value; value->tv_sec=123; value->tv_usec=123456; return 0; }
    virtual tm* Local(const time_t* epoch,tm* value) {
        assert(epoch==reinterpret_cast<const time_t*>(&stamp->tv_sec)); std::memset(value,0,sizeof(*value)); value->tm_mon=0; value->tm_mday=2; value->tm_hour=3; value->tm_min=4; value->tm_sec=5; return value;
    }
    virtual long Write(int fd,const void* data,std::size_t size) { assert(fd==7); AfterSave(); ctx.developer.assign(static_cast<const char*>(data),size); ctx.steps.push_back("dev-write"); return static_cast<long>(size); }
    virtual int Close(int fd) { assert(fd==7); AfterSave(); ctx.steps.push_back("dev-close"); return 0; }
};
static void ClearAudit(LogManager& logger) { while(!logger.m_sql.m_queue_query.empty()) { delete logger.m_sql.m_queue_query.front(); logger.m_sql.m_queue_query.pop(); } }
static void Start(CItem& item,ITEM_MANAGER* manager,LogManager& logger,CHARACTER* owner,LPBUFFER output,int index,long value,DWORD wire,bool dirty,bool packet,bool alignment,bool timer,bool debug) {
    assert(!ctx.active); ClearAudit(logger); if(manager) manager->m_set_pkItemForDelayedSave.clear(); if(output) buffer_reset(output);
    ctx.item=&item; ctx.manager=manager; ctx.logger=&logger; ctx.owner=owner; ctx.output=output;
    ctx.index=index; ctx.value=value; ctx.wire=wire; ctx.previous=item.GetSockets()[index]; ctx.dirty=dirty; ctx.packet=packet; ctx.alignment=alignment; ctx.timer=timer; ctx.debug=debug;
    ctx.steps.clear(); ctx.developer.clear(); ctx.serial=logger.m_sql.m_iMsgCount; test_server=debug?1:0; ctx.active=true;
}
static void Finish() {
    assert(ctx.logger->m_sql.m_queue_query.size()==1 && ctx.logger->m_sql.m_iMsgCount==ctx.serial+1);
    SQLMsg* message=ctx.logger->m_sql.m_queue_query.front(); char expected[512];
    std::snprintf(expected,sizeof(expected),"INSERT DELAYED INTO log_socket (type, time, who, x, y, what, how, hint) VALUES('ITEM', NOW(), %lu, %lu, 0, %lu, 'SET_SOCKET', '')",static_cast<DWORD>(ctx.index),ctx.wire,ctx.item->GetID());
    assert(message->stQuery==expected && message->m_pkSQL==ctx.logger->m_sql.GetSQLHandle() && !message->bReturn);
    std::vector<std::string> steps; if(ctx.alignment) steps.push_back("alignment"); if(ctx.packet) steps.push_back("packet");
    if(ctx.debug) { steps.push_back("dev-open"); steps.push_back("dev-write"); steps.push_back("dev-close"); }
    steps.push_back("escape"); steps.push_back("lock"); steps.push_back("post"); steps.push_back("unlock"); assert(ctx.steps==steps);
    if(ctx.debug) { char suffix[160]; std::snprintf(suffix,sizeof(suffix),"ITEM SetSocket (name,apply_type,apply_value) = (socketed, %d, %d)\n",ctx.index,static_cast<int>(ctx.value)); assert(ctx.developer.find("SetSocket")!=std::string::npos && ctx.developer.find(" 853,")!=std::string::npos && ctx.developer.substr(ctx.developer.size()-std::strlen(suffix))==suffix); }
    ctx.active=false; test_server=0;
}
static long UnreachableTimer(LPEVENT) { assert(false); return 0; }
struct TimerInfo { CItem* item; };
static bool callback_observed;
static int forced_diagnostics;
static DWORD diagnostic_info;
static void ForcedDiagnostic() {
    assert(ctx.active && callback_observed && !ctx.item->m_pkUniqueExpireEvent && FindEventInfo(diagnostic_info) && ctx.logger->m_sql.m_queue_query.size()==1);
    ++forced_diagnostics;
}
static long StopInCallback(LPEVENT event) {
    TimerInfo* info=static_cast<TimerInfo*>(event_info(event)); DWORD id=event->dwInfoID;
    assert(event->is_processing && !event->q_el && info->item->m_pkUniqueExpireEvent==event);
    info->item->StopUniqueExpireEvent(); assert(!info->item->m_pkUniqueExpireEvent && event->is_force_to_end && FindEventInfo(id)); callback_observed=true; return 99;
}
static LPEVENT Timer(CItem& item,long when,TEVENTFUNC callback=UnreachableTimer) {
    TimerInfo* info; DWORD id=__AllocEventInfo(sizeof(TimerInfo),reinterpret_cast<void**>(&info)); info->item=&item;
    LPEVENT timer=event_create_ex(callback,id,when,const_cast<char*>("socket-test"),1); item.m_pkUniqueExpireEvent=timer; return timer;
}
static void Proto(TItemTable& proto,DWORD vnum) { std::memset(&proto,0,sizeof(proto)); proto.dwVnum=vnum; std::strcpy(proto.szLocaleName,"socketed"); }
int main() {
    desc_test_allow_clock(); test_server=0;
    { CItem empty; empty.Initialize(); empty.StopUniqueExpireEvent(); } // No proto or singleton lookup on the empty path.
    Runtime runtime; DevIo io; sql_test_runtime=&runtime; item_dev_io=&io;
    LogManager logger; ctx.logger=&logger; runtime.db=logger.m_sql.GetSQLHandle(); assert(logger.Connect("host","user","password","database"));
    g_table_postfix="_socket"; dev_log_set_level(L_DEB1);
    {
        CItem item; item.Initialize(); TItemTable proto; Proto(proto,1); item.SetProto(&proto); item.SetID(7); item.SetSkipSave(true);
        Start(item,0,logger,0,0,0,99,99,false,false,false,false,false); item.SetSocket(0,99); Finish(); ClearAudit(logger);
    }
    quest::CQuestManager quests; ITEM_MANAGER manager; CHARACTER owner; owner.m_dwPlayerID=777; owner.m_stName="socket-owner"; owner.m_vid=VID(7000,55);
    SECTREE sector; owner.SetSectree(&sector);
    LPBUFFER output=buffer_new(0,8192); LPFDWATCH watch=fdwatch_new(16); DescOutputProbe descriptor; descriptor.Attach(watch,5,output); descriptor.SetPacketState(5); owner.BindDesc(&descriptor);
    thecore_memcpy=Copy;
    struct SocketCase { int index; long value; DWORD wire,id; bool skip,dirty; } sockets[]={
        {0,2147483647L,2147483647UL,1234,false,true},{1,-1,4294967295UL,0,false,false},{2,(-2147483647L-1),2147483648UL,1234,true,false}
    };
    for(unsigned i=0;i<sizeof(sockets)/sizeof(*sockets);++i) {
        CItem item; item.Initialize(); TItemTable proto; Proto(proto,1); item.SetProto(&proto); item.SetID(sockets[i].id); item.SetCell(&owner,107); item.m_dwCount=300; item.SetSkipSave(sockets[i].skip);
        Start(item,&manager,logger,&owner,output,sockets[i].index,sockets[i].value,sockets[i].wire,sockets[i].dirty,true,false,false,true);
        item.SetSocket(sockets[i].index,sockets[i].value); Finish(); item.SetCell(0,0); manager.m_set_pkItemForDelayedSave.clear(); ClearAudit(logger);
    }
    struct ExpiryCase { DWORD vnum; int when,elapsed,scale; long value; DWORD wire; bool align; } expiry[]={
        {70048,274,0,25,10,10,true},{70047,10,61,25,-2,4294967294UL,false},{70048,274,0,50,5,5,true}
    };
    for(unsigned i=0;i<sizeof(expiry)/sizeof(*expiry);++i) {
        assert(!event_count()); thecore_heart->pulse=100; passes_per_sec=expiry[i].scale;
        CItem item; item.Initialize(); TItemTable proto; Proto(proto,expiry[i].vnum); item.SetProto(&proto); item.SetID(1234); item.SetCell(&owner,107); item.m_dwCount=300; item.m_alSockets[1]=-77;
        owner.m_iAlignment=0; owner.m_iRealAlignment=-12345; LPEVENT timer=Timer(item,expiry[i].when); DWORD id=timer->dwInfoID;
        thecore_heart->pulse=100+expiry[i].elapsed;
        Start(item,&manager,logger,&owner,output,1,expiry[i].value,expiry[i].wire,true,true,expiry[i].align,true,true);
        item.StopUniqueExpireEvent(); Finish(); assert(!item.m_pkUniqueExpireEvent && !FindEventInfo(id));
        assert(owner.m_iAlignment==(expiry[i].align?-12345:0));
        thecore_heart->pulse=100+expiry[i].when+expiry[i].elapsed; assert(!event_process(thecore_heart->pulse) && !event_count());
        item.SetCell(0,0); manager.m_set_pkItemForDelayedSave.clear(); ClearAudit(logger);
    }
    // Sparse and negative sockets: count the occupied prefix, not all occupied cells.
    {
        CItem item; item.Initialize();
        for(unsigned mask=0;mask<8;++mask) {
            for(int index=0;index<3;++index) item.m_alSockets[index]=(mask&(1u<<index))?-7:0;
            int expected=0; while(expected<3 && (mask&(1u<<expected))) ++expected;
            assert(item.GetSocketCount()==expected);
        }
        TItemTable proto; Proto(proto,1); item.SetProto(&proto);
        for(unsigned type=0;type<256;++type) for(unsigned subtype=0;subtype<256;++subtype) {
            proto.bType=static_cast<BYTE>(type); proto.bSubType=static_cast<BYTE>(subtype);
            assert(item.IsAccessoryForSocket()==(type==2 && (subtype==3 || subtype==5 || subtype==6)));
        }
    }
    // Max-grade mutation has no accessory predicate guard and still emits on repeats.
    const int grades[]={(-2147483647-1),-1,0,1,2,3,4,2147483647,3};
    for(unsigned i=0;i<sizeof(grades)/sizeof(*grades);++i) {
        CItem item; item.Initialize(); TItemTable proto; Proto(proto,1); item.SetProto(&proto);
        item.SetID(1234); item.SetCell(&owner,107); item.m_dwCount=300;
        int value=grades[i]<0?0:(grades[i]>3?3:grades[i]); item.m_alSockets[1]=value;
        Start(item,&manager,logger,&owner,output,1,value,static_cast<DWORD>(value),true,true,false,false,true);
        item.SetAccessorySocketMaxGrade(grades[i]); Finish(); assert(item.m_alSockets[1]==value);
        item.SetCell(0,0); manager.m_set_pkItemForDelayedSave.clear(); ClearAudit(logger);
    }
    passes_per_sec=25;
    {
        thecore_heart->pulse=100; CItem item; item.Initialize(); TItemTable proto; Proto(proto,70048); proto.alValues[2]=1; item.SetProto(&proto); item.SetID(1234); item.SetCell(&owner,107); item.m_alSockets[1]=-77;
        owner.m_iAlignment=0; owner.m_iRealAlignment=-12345; LPEVENT timer=Timer(item,25); DWORD id=timer->dwInfoID;
        Start(item,&manager,logger,&owner,output,1,-77,0,false,false,false,true,true); item.StopUniqueExpireEvent();
        assert(item.m_pkUniqueExpireEvent==timer && item.m_alSockets[1]==-77 && owner.m_iAlignment==0 && ctx.steps.empty() && !buffer_size(output) && manager.m_set_pkItemForDelayedSave.empty() && logger.m_sql.m_queue_query.empty());
        ctx.active=false; test_server=0; event_cancel(&item.m_pkUniqueExpireEvent); assert(!FindEventInfo(id)); thecore_heart->pulse=125; assert(!event_process(125) && !event_count()); item.SetCell(0,0);
    }
    {
        thecore_heart->pulse=200; CItem item; item.Initialize(); TItemTable proto; Proto(proto,1); item.SetProto(&proto); item.SetID(1234); item.SetCell(&owner,107); item.m_dwCount=300;
        LPEVENT timer=Timer(item,25,StopInCallback); DWORD id=timer->dwInfoID; diagnostic_info=id; desc_test_event_force_error=ForcedDiagnostic;
        Start(item,&manager,logger,&owner,output,1,0,0,true,true,false,true,true); thecore_heart->pulse=225;
        assert(event_process(225)==1); desc_test_event_force_error=0; Finish(); assert(forced_diagnostics==1 && callback_observed && !item.m_pkUniqueExpireEvent && !FindEventInfo(id) && !event_count()); item.SetCell(0,0); manager.m_set_pkItemForDelayedSave.clear(); ClearAudit(logger);
    }
    thecore_memcpy=std::memcpy; owner.BindDesc(0); owner.SetSectree(0); descriptor.Attach(0,-1,0); buffer_delete(output); fdwatch_delete(watch); event_destroy();
    std::puts("item socket/unique expiry passed: real packets/save/devlog/SQL ordering, signed audit values, alignment and queued/processing event cancellation");
}
