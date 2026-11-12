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
#include "../../game/src/locale.hpp"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
extern "C" void (*desc_test_event_force_error)(void);
void ShutdownOnFatalError() { std::abort(); }
// This fixture observes external I/O seams only; application methods are real.
struct Operation { int index; long value; Operation(int i,long v):index(i),value(v){} };
struct Context {
    bool active,debug,packet,dirty,reenter,translate;
    unsigned stage; int serial,localeErrors; long sockets[3];
    CItem* item; ITEM_MANAGER* manager; LogManager* logger;
    LPBUFFER output; std::vector<Operation> ops;
    std::vector<std::string> steps,expected;
    std::string chat,developer;
    size_t packetBytes;
} static ctx;
static DWORD Read4(const BYTE* b) { return static_cast<DWORD>(b[0]) | static_cast<DWORD>(b[1])<<8 | static_cast<DWORD>(b[2])<<16 | static_cast<DWORD>(b[3])<<24; }
static Operation& Current() { assert(ctx.active && ctx.stage<ctx.ops.size()); return ctx.ops[ctx.stage]; }
static void AfterSave() {
    Operation& op=Current(); assert(ctx.item->GetSockets()[op.index]==op.value);
    assert((ctx.manager->m_set_pkItemForDelayedSave.count(ctx.item)!=0)==ctx.dirty);
    assert(buffer_size(ctx.output)==ctx.packetBytes);
}
static void Step(const char* name) { ctx.steps.push_back(name); }
static void* Copy(void* out,const void* in,size_t count) {
    if(ctx.active && count) {
        const BYTE* b=static_cast<const BYTE*>(in);
        if(count==36 && b[0]==25) {
            Operation& op=Current(); assert(ctx.packet && b[1]==107 && b[2]==44);
            ctx.sockets[op.index]=op.value;
            for(int i=0;i<3;++i) assert(Read4(b+3+4*i)==static_cast<DWORD>(ctx.sockets[i]));
            assert(ctx.item->GetSockets()[op.index]==op.value);
            assert(ctx.logger->m_sql.m_queue_query.size()==ctx.stage);
            assert((ctx.manager->m_set_pkItemForDelayedSave.count(ctx.item)!=0)==(ctx.stage!=0 && ctx.dirty));
            Step("packet"); ctx.packetBytes+=36;
        } else if(count==ctx.chat.size()+10 && b[0]==4) {
            // TEMP_BUFFER's header and payload copies are not DESC output.
            assert(ctx.debug && ctx.packet && ctx.stage==ctx.ops.size());
            assert(ctx.logger->m_sql.m_queue_query.size()==ctx.ops.size());
            assert(b[1]==count%256 && b[2]==count/256 && b[3]==1 && Read4(b+4)==0);
            assert(std::memcmp(b+9,ctx.chat.c_str(),ctx.chat.size()+1)==0);
            Step("chat"); ctx.packetBytes+=count;
        }
    }
    return std::memcpy(out,in,count);
}
struct Runtime : SqlRuntime {
    MYSQL* db; int depth;
    Runtime():db(0),depth(0){}
    virtual int SemInit(sem_t* s,int shared,unsigned int value) { assert(!shared&&!value); *s=reinterpret_cast<sem_t>(1);return 0; }
    virtual int SemDestroy(sem_t* s) { assert(s&&*s);return 0; }
    virtual int MutexInit(pthread_mutex_t* m,const pthread_mutexattr_t* a) { assert(m&&!a);std::memset(m,0,sizeof(*m));return 0; }
    virtual int Create(pthread_t*,const pthread_attr_t* a,void*(*entry)(void*),void* arg) { assert(!a&&entry==AsyncSQLThread&&arg==&ctx.logger->m_sql);return 11; }
    virtual void Close(MYSQL* p) { assert(p==db); }
    virtual int MutexDestroy(pthread_mutex_t* m) { assert(m&&!depth);return 0; }
    virtual unsigned long Escape(MYSQL* p,char* to,const char* from,unsigned long count) { assert(p==db&&!count&&!*from);AfterSave();Step("escape");to[0]=0;return 0; }
    virtual int MutexLock(pthread_mutex_t* m) { assert(m==ctx.logger->m_sql.m_mtxQuery&&!depth);AfterSave();assert(ctx.logger->m_sql.m_queue_query.size()==ctx.stage);++depth;Step("lock");return 0; }
    virtual int SemPost(sem_t* s) { assert(s&&*s&&depth==1);AfterSave();assert(ctx.logger->m_sql.m_queue_query.size()==ctx.stage+1);Step("post");return 0; }
    virtual int MutexUnlock(pthread_mutex_t* m) {
        assert(m==ctx.logger->m_sql.m_mtxQuery&&depth==1);AfterSave();--depth;Step("unlock");++ctx.stage;
        // A real bulk setter at an external callback seam proves reload after
        // SetSocket(0). It adds a real Save but no fake application behavior.
        if(ctx.reenter&&ctx.stage==1) {
            long values[3]; for(int i=0;i<3;++i) values[i]=ctx.item->GetSockets()[i];
            values[0]=2; ctx.item->SetSockets(values); ctx.sockets[0]=2; Step("reenter-save");
        }
        return 0;
    }
};
struct DevIo : ItemDevIo {
    timeval* stamp;
    virtual int Open(const char* file,int flags,int mode) { AfterSave();assert(ctx.debug&&std::string(file)=="DEV_LOG.log"&&flags==0x209&&mode==0666);Step("dev-open");return 7; }
    virtual int Time(timeval* t,void* zone) { assert(!zone);stamp=t;t->tv_sec=123;t->tv_usec=123456;return 0; }
    virtual tm* Local(const time_t* epoch,tm* t) { assert(epoch==reinterpret_cast<const time_t*>(&stamp->tv_sec));std::memset(t,0,sizeof(*t));t->tm_mon=0;t->tm_mday=2;t->tm_hour=3;t->tm_min=4;t->tm_sec=5;return t; }
    virtual long Write(int fd,const void* data,std::size_t size) {
        assert(fd==7);AfterSave();ctx.developer.assign(static_cast<const char*>(data),size);
        Operation& op=Current();char suffix[160];std::snprintf(suffix,sizeof(suffix),"ITEM SetSocket (name,apply_type,apply_value) = (socketed, %d, %d)\n",op.index,static_cast<int>(op.value));
        assert(ctx.developer.find("SetSocket")!=std::string::npos&&ctx.developer.find(" 853,")!=std::string::npos);
        assert(ctx.developer.size()>=std::strlen(suffix)&&ctx.developer.substr(ctx.developer.size()-std::strlen(suffix))==suffix);
        Step("dev-write");return static_cast<long>(size);
    }
    virtual int Close(int fd) { assert(fd==7);AfterSave();Step("dev-close");return 0; }
};
static void ClearAudit(LogManager& l) { while(!l.m_sql.m_queue_query.empty()){delete l.m_sql.m_queue_query.front();l.m_sql.m_queue_query.pop();} }
static void Start(CItem& item,ITEM_MANAGER& manager,LogManager& logger,LPBUFFER output,bool debug,bool packet,bool dirty,bool reenter,bool translate) {
    assert(!ctx.active);ClearAudit(logger);manager.m_set_pkItemForDelayedSave.clear();buffer_reset(output);
    ctx.item=&item;ctx.manager=&manager;ctx.logger=&logger;ctx.output=output;
    ctx.debug=debug;ctx.packet=packet;ctx.dirty=dirty;ctx.reenter=reenter;ctx.translate=translate;
    ctx.stage=0;ctx.localeErrors=0;ctx.serial=logger.m_sql.m_iMsgCount;ctx.packetBytes=0;ctx.chat.clear();ctx.ops.clear();ctx.steps.clear();ctx.expected.clear();
    for(int i=0;i<3;++i)ctx.sockets[i]=item.GetSockets()[i];
    test_server=debug?1:0;g_iUseLocale=translate?1:0;ctx.active=true;
}
static void Expect(int index,long value) {
    ctx.ops.push_back(Operation(index,value));
    if(ctx.packet)ctx.expected.push_back("packet");
    if(ctx.debug){ctx.expected.push_back("dev-open");ctx.expected.push_back("dev-write");ctx.expected.push_back("dev-close");}
    ctx.expected.push_back("escape");ctx.expected.push_back("lock");ctx.expected.push_back("post");ctx.expected.push_back("unlock");
    if(ctx.reenter&&ctx.ops.size()==1)ctx.expected.push_back("reenter-save");
}
static void Finish() {
    assert(ctx.stage==ctx.ops.size()&&ctx.logger->m_sql.m_queue_query.size()==ctx.ops.size());
    assert(ctx.logger->m_sql.m_iMsgCount==ctx.serial+static_cast<int>(ctx.ops.size()));
    for(unsigned i=0;i<ctx.ops.size();++i){
        SQLMsg* m=ctx.logger->m_sql.m_queue_query.front();ctx.logger->m_sql.m_queue_query.pop();
        char expected[512];Operation& op=ctx.ops[i];
        std::snprintf(expected,sizeof(expected),"INSERT DELAYED INTO log_socket (type, time, who, x, y, what, how, hint) VALUES('ITEM', NOW(), %lu, %lu, 0, %lu, 'SET_SOCKET', '')",static_cast<DWORD>(op.index),static_cast<DWORD>(op.value),ctx.item->GetID());
        assert(m->stQuery==expected&&m->m_pkSQL==ctx.logger->m_sql.GetSQLHandle()&&!m->bReturn);delete m;
    }
    if(!ctx.chat.empty()){
        if(!ctx.translate)ctx.expected.push_back("locale-diagnostic");
        ctx.expected.push_back("chat");
    }
    assert(ctx.localeErrors==(!ctx.chat.empty()&&!ctx.translate?1:0));
    assert(ctx.steps==ctx.expected&&buffer_size(ctx.output)==ctx.packetBytes);
    ctx.active=false;test_server=0;g_iUseLocale=0;ctx.manager->m_set_pkItemForDelayedSave.clear();
}

static const char localeKey[]="\045\163\040\277\241\274\255\040\274\322\304\317\040\272\374\301\372\266\247\261\356\301\366\040\263\262\300\272\040\275\303\260\243\040\045\144";
extern "C" void accessory_test_locale_error(const char* function,int line,const char* format,const char* key) {
    assert(ctx.active&&ctx.debug&&!ctx.translate&&ctx.packet&&ctx.stage==ctx.ops.size());
    assert(std::strcmp(function,"locale_find")==0&&line==51);
    assert(std::strcmp(format,"LOCALE_ERROR: locae == 0  \"%s\";")==0);
    assert(std::strcmp(key,localeKey)==0&&ctx.logger->m_sql.m_queue_query.size()==ctx.ops.size());
    ++ctx.localeErrors;Step("locale-diagnostic");
}
static void Chat(long value) {
    if(ctx.debug&&ctx.packet){char b[256];std::snprintf(b,sizeof(b),ctx.translate?"translated:%s:%d":localeKey,"socketed",static_cast<int>(value));ctx.chat=b;}
}
static void Proto(TItemTable& p) { std::memset(&p,0,sizeof(p));p.dwVnum=1;p.bType=0;p.bSubType=0;std::strcpy(p.szLocaleName,"socketed"); }
int main() {
    assert(sizeof(DWORD)==4 && sizeof(long)==4);
    desc_test_allow_clock();test_server=0;
    Runtime runtime;DevIo io;sql_test_runtime=&runtime;item_dev_io=&io;
    LogManager logger;ctx.logger=&logger;runtime.db=logger.m_sql.GetSQLHandle();assert(logger.Connect("host","user","password","database"));
    g_table_postfix="_socket";dev_log_set_level(L_DEB1);
    const char* translation[]={localeKey,"translated:%s:%d"};locale_add(translation);
    assert(std::strcmp(locale_find(localeKey,1),translation[1])==0);
    quest::CQuestManager quests;ITEM_MANAGER manager;CHARACTER owner;owner.m_dwPlayerID=777;owner.m_stName="socket-owner";owner.m_vid=VID(7000,55);
    SECTREE sector;owner.SetSectree(&sector);
    LPBUFFER output=buffer_new(0,8192);LPFDWATCH watch=fdwatch_new(16);DescOutputProbe descriptor;descriptor.Attach(watch,5,output);descriptor.SetPacketState(5);owner.BindDesc(&descriptor);
    thecore_memcpy=Copy;
    const int requests[]={(-2147483647-1),-1,0,1,2,3,4,2147483647};
    const int times[]={0,86400,43200,21600};
    // Non-accessory prototype proves that setters have no accessory guard.
    for(int debug=0;debug<2;++debug)for(int translated=0;translated<2;++translated)for(int maximum=0;maximum<=3;++maximum)for(unsigned n=0;n<sizeof(requests)/sizeof(*requests);++n){
        CItem item;item.Initialize();TItemTable p;Proto(p);item.SetProto(&p);item.SetID(1234);item.SetCell(&owner,107);item.m_dwCount=300;
        int grade=requests[n]<0?0:(requests[n]>maximum?maximum:requests[n]);int time=times[grade]/(debug?60:1);
        // Repeating the stored values must still execute both complete paths.
        item.m_alSockets[0]=grade;item.m_alSockets[1]=maximum;item.m_alSockets[2]=time;
        Start(item,manager,logger,output,debug!=0,true,true,false,translated!=0);Expect(0,grade);Expect(2,time);Chat(time);
        item.SetAccessorySocketGrade(requests[n]);Finish();assert(item.GetAccessorySocketGrade()==grade&&item.GetAccessorySocketMaxGrade()==maximum&&item.GetAccessorySocketDownGradeTime()==time);
        item.SetCell(0,0);
    }
    // Raw maximum >3 with safe index; callback changes grade1 to2 through real
    // SetSockets after the first SQL enqueue, before the table read.
    for(int reenter=0;reenter<2;++reenter){
        CItem item;item.Initialize();TItemTable p;Proto(p);item.SetProto(&p);item.SetID(1234);item.SetCell(&owner,107);item.m_dwCount=300;item.m_alSockets[1]=7;
        int time=times[reenter?2:1]/60;
        Start(item,manager,logger,output,true,true,true,reenter!=0,true);Expect(0,1);Expect(2,time);Chat(time);
        item.SetAccessorySocketGrade(1);Finish();assert(item.GetAccessorySocketGrade()==(reenter?2:1)&&item.GetAccessorySocketMaxGrade()==7);item.SetCell(0,0);
    }
    // Raw getter observations include invalid table indices; do not pass these
    // values into SetGrade, which intentionally retains the original precondition.
    {
        CItem item;item.Initialize();long values[3]={-7,7,(-2147483647L-1)};item.SetSkipSave(true);item.SetSockets(values);
        assert(item.GetAccessorySocketGrade()==-7&&item.GetAccessorySocketMaxGrade()==7&&item.GetAccessorySocketDownGradeTime()==(-2147483647-1));
        values[0]=2147483647;values[1]=-1;values[2]=-1;item.SetSockets(values);
        assert(item.GetAccessorySocketGrade()==2147483647&&item.GetAccessorySocketMaxGrade()==-1&&item.GetAccessorySocketDownGradeTime()==-1);
    }
    const DWORD extremes[]={0UL,2147483647UL,2147483648UL,4294967295UL};
    for(int debug=0;debug<2;++debug)for(int owned=0;owned<2;++owned)for(unsigned n=0;n<sizeof(extremes)/sizeof(*extremes);++n){
        CItem item;item.Initialize();TItemTable p;Proto(p);item.SetProto(&p);item.SetID(1234);if(owned)item.SetCell(&owner,107);item.m_dwCount=300;
        // The target and this integration fixture require 32-bit DWORD/long.
        long expected=static_cast<long>(static_cast<int>(static_cast<unsigned int>(extremes[n])));
        Start(item,manager,logger,output,debug!=0,owned!=0,true,false,true);Expect(2,expected);Chat(expected);
        item.SetAccessorySocketDownGradeTime(extremes[n]);Finish();assert(item.GetAccessorySocketDownGradeTime()==static_cast<int>(expected));item.SetCell(0,0);
    }
    thecore_memcpy=std::memcpy;owner.BindDesc(0);owner.SetSectree(0);descriptor.Attach(0,-1,0);buffer_delete(output);fdwatch_delete(watch);event_destroy();
    std::puts("accessory setters passed: real packet/save/devlog/SQL/chat/locale paths, raw getters and callback reload");
}
