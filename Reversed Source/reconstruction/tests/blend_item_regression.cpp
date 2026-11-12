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

extern "C" {
FILE* blend_test_fopen(const char*,const char*);
int blend_test_fclose(FILE*);
}
namespace std { using ::blend_test_fopen;using ::blend_test_fclose; }
#define fopen blend_test_fopen
#define fclose blend_test_fclose
#include "../../game/src/blend_item.cpp"
#undef fclose
#undef fopen

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
        // External SQL callback mutates the full blend table after socket0.
        // Both sampled scalar locals must already have been captured.
        if(ctx.reenter&&ctx.stage==1) {
            long values[3]; for(int i=0;i<3;++i) values[i]=ctx.item->GetSockets()[i];
            (void)values;
            s_blend_info[0]->apply_type=999;
            for(int i=0;i<5;++i) {s_blend_info[0]->apply_value[i]=999;s_blend_info[0]->apply_duration[i]=999;}
            Step("table-mutate");
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
    if(ctx.reenter&&ctx.ops.size()==1)ctx.expected.push_back("table-mutate");
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


extern std::string g_stServiceBasePath;
static std::string fileText,filePath;
static bool failOpen,expectCleared;
static int opens,closes,diagnostics;
static std::vector<FILE*> files;
static std::vector<long> randomValues;
static unsigned randomAt;
extern "C" FILE* blend_test_fopen(const char* file,const char* mode) {
    assert(file&&mode&&!std::strcmp(mode,"r")&&filePath==file);
    assert(files.empty());++opens;
    if(expectCleared)assert(s_blend_info.empty());
    if(failOpen)return 0;
    FILE* fp=std::tmpfile();assert(fp);
    assert(std::fwrite(fileText.data(),1,fileText.size(),fp)==fileText.size());
    std::rewind(fp);files.push_back(fp);return fp;
}
extern "C" int blend_test_fclose(FILE* fp) {
    assert(files.size()==1&&files[0]==fp);files.clear();++closes;return std::fclose(fp);
}
extern "C" long blend_test_random() {
    assert(randomAt<randomValues.size());return randomValues[randomAt++];
}
extern "C" void blend_test_init_error(const char* function,int line,const char* format) {
    assert(!std::strcmp(function,"Blend_Item_init")&&line==156&&!std::strcmp(format,"<Blend_Item_init> fail"));++diagnostics;
}
static void File(const std::string& text,bool fail=false,bool cleared=false) {
    assert(files.empty());fileText=text;failOpen=fail;expectCleared=cleared;opens=closes=diagnostics=0;
}
static void LeakedFileCleanup() {
    // The original parser did not close this FILE. Cleanup only at external test seam.
    assert(files.size()==1&&closes==0);std::fclose(files[0]);files.clear();
}
static void Reset(const std::string& text) {
    File(text,false,true);filePath=g_stServiceBasePath+"/blend.txt";
    assert(Blend_Item_init());assert(opens==1&&closes==1&&!diagnostics&&files.empty());
}
static bool Load(const std::string& text) {
    File(text);filePath="fixture";char name[]="fixture";return Blend_Item_load(name);
}
static void Percent(int a,int b) {
    assert(randomAt==randomValues.size());randomValues.clear();randomAt=0;
    randomValues.push_back(a-1);randomValues.push_back(b-1);
}
static int Index(int p) { return p<=10?0:p<=30?1:p<=70?2:p<=90?3:4; }
static std::string Section(DWORD vnum) {
    char buffer[128];std::snprintf(buffer,sizeof(buffer),"section\nitem_vnum %lu\n",vnum);
    return std::string(buffer)+"apply_type MAX_HP\napply_value -2147483648 -1 0 123 2147483647 extra ignored\napply_duration 7 19 31 43 55\nend\n";
}
static void MissingTokens() {
    const char* fields[]={"item_vnum","apply_type","apply_value","apply_duration"};
    for(int field=0;field<4;++field)for(int supplied=0;supplied<((field<2)?1:5);++supplied) {
        Reset("");std::string text="section\nend\n";text+=fields[field];
        for(int i=0;i<supplied;++i)text+=" 17";
        text+="\n";assert(!Load(text));assert(opens==1&&!closes&&!diagnostics);
        assert(s_blend_info.size()==1);
        if(field==2)for(int i=0;i<5;++i)assert(s_blend_info[0]->apply_value[i]==(i<supplied?17:0));
        if(field==3)for(int i=0;i<5;++i)assert(s_blend_info[0]->apply_duration[i]==(i<supplied?17:0));
        LeakedFileCleanup(); // Published record is deleted by next real init.
    }
}
#include "blend_table_golden.h"
int main() {
    assert(sizeof(DWORD)==4&&sizeof(long)==4&&sizeof(BLEND_ITEM_INFO)==48);
    assert(s_blend_info.empty());
    desc_test_allow_clock();test_server=0;
    Runtime runtime;sql_test_runtime=&runtime;
    LogManager logger;ctx.logger=&logger;runtime.db=logger.m_sql.GetSQLHandle();assert(logger.Connect("host","user","password","database"));g_table_postfix="_socket";
    quest::CQuestManager quests;ITEM_MANAGER manager;CHARACTER owner;owner.m_dwPlayerID=777;owner.m_stName="blend-owner";owner.m_vid=VID(7000,55);
    SECTREE sector;owner.SetSectree(&sector);
    LPBUFFER output=buffer_new(0,8192);LPFDWATCH watch=fdwatch_new(16);DescOutputProbe descriptor;descriptor.Attach(watch,5,output);descriptor.SetPacketState(5);owner.BindDesc(&descriptor);
    thecore_memcpy=Copy;std::string oldBase=g_stServiceBasePath;g_stServiceBasePath="blend-fixture";
    for(int i=0;i<77;++i) {
        assert(c_aApplyTypeNames[i].lValue==blendGoldenValues[i]);
        if(i==76) {assert(!c_aApplyTypeNames[i].c_pszName);continue;}
        assert(!std::strcmp(c_aApplyTypeNames[i].c_pszName,blendGoldenNames[i]));
        char lower[128];std::strcpy(lower,blendGoldenNames[i]);for(char* c=lower;*c;++c)if(*c>='A'&&*c<='Z')*c=static_cast<char>(*c-'A'+'a');
        assert(FN_get_apply_type(lower)==blendGoldenValues[i]);
    }
    char unknown[]="ENERGY";assert(FN_get_apply_type(unknown)==0); // Newer reference has this; old table does not.
    File("");assert(!Blend_Item_load(0));char empty[]="";assert(!Blend_Item_load(empty));assert(!opens&&!closes);
    File("",true);filePath="fixture";char name[]="fixture";assert(!Blend_Item_load(name));assert(opens==1&&!closes);
    Reset("");assert(s_blend_info.empty()&&!Blend_Item_find(0));
    assert(Blend_Item_set_value(0)==false); // Empty vector returns before dereferencing item.
    // Unknown tokens/comments/blank lines, mixed-case grammar and names, numeric prefix/wrap.
    assert(Load("#comment\n\n unknown ignored\nSeCtIoN ignored\nitem_vnum -1junk\napply_type mAx_hP\napply_value 1junk -2 +3 4 5\napply_duration 6 7 8 9 10\nEnD\n"));
    assert(s_blend_info.size()==1&&Blend_Item_find(0xffffffffUL));
    assert(s_blend_info[0]->apply_type==1&&s_blend_info[0]->apply_value[0]==1&&s_blend_info[0]->apply_value[1]==-2);
    // Direct load appends; unknown apply accepted as0, no modern error path.
    assert(Load("section\nitem_vnum 77\napply_type NOT_AN_APPLY\nend\n"));
    assert(s_blend_info.size()==2&&s_blend_info[1]->apply_type==0);
    MissingTokens();Reset("");
    // Init deletes/clears prior vector even when open/load fails.
    assert(Load(Section(1)));File("",true,true);filePath=g_stServiceBasePath+"/blend.txt";
    assert(!Blend_Item_init()&&s_blend_info.empty()&&diagnostics==1&&opens==1&&!closes);
    const long values[5]={(-2147483647L-1),-1,0,123,2147483647};
    const long durations[5]={7,19,31,43,55};
    // All100x100 independent percentile combinations; same distribution even51002.
    for(int special=0;special<2;++special) {
        DWORD vnum=special?51002:50000;Reset(Section(vnum)+Section(vnum));assert(s_blend_info.size()==2);
        // Duplicate records: first wins; second deliberately differs.
        s_blend_info[1]->apply_type=888;
        for(int a=1;a<=100;++a)for(int b=1;b<=100;++b) {
            CItem item;item.Initialize();TItemTable proto;std::memset(&proto,0,sizeof(proto));proto.dwVnum=vnum;std::strcpy(proto.szLocaleName,"socketed");item.SetProto(&proto);item.SetID(1234);item.SetCell(&owner,107);item.m_dwCount=300;
            Percent(a,b);Start(item,manager,logger,output,false,true,true,false,false);Expect(0,1);Expect(1,values[Index(a)]);Expect(2,durations[Index(b)]);
            assert(Blend_Item_set_value(&item));assert(randomAt==2);Finish();item.SetCell(0,0);
        }
        // Failed lookup consumes no RNG, updates no packet/save/SQL.
        CItem absent;absent.Initialize();TItemTable proto;std::memset(&proto,0,sizeof(proto));proto.dwVnum=9;absent.SetProto(&proto);
        Start(absent,manager,logger,output,false,false,false,false,false);assert(!Blend_Item_set_value(&absent));Finish();assert(randomAt==2);
    }
    // Reentrant external SQL callback mutates table after socket0: both sampled
    // values were already loaded, so later sockets must retain the original locals.
    Reset(Section(50000));
    {
        CItem item;item.Initialize();TItemTable proto;std::memset(&proto,0,sizeof(proto));proto.dwVnum=50000;std::strcpy(proto.szLocaleName,"socketed");item.SetProto(&proto);item.SetID(1234);item.SetCell(&owner,107);item.m_dwCount=300;
        Percent(1,100);Start(item,manager,logger,output,false,true,true,true,false);Expect(0,1);Expect(1,values[0]);Expect(2,durations[4]);
        assert(Blend_Item_set_value(&item));Finish();assert(s_blend_info[0]->apply_type==999);item.SetCell(0,0);
    }
    Reset("section\nend\n"); // vnum0 and every fieldzero are genuine defaults.
    {
        CItem item;item.Initialize();item.SetID(1234);item.SetSkipSave(true);
        Percent(50,50);Start(item,manager,logger,output,false,false,false,false,false);Expect(0,0);Expect(1,0);Expect(2,0);
        assert(Blend_Item_set_value(&item));Finish();assert(Blend_Item_find(0));
    }
    Reset("");g_stServiceBasePath=oldBase;thecore_memcpy=std::memcpy;owner.BindDesc(0);owner.SetSectree(0);descriptor.Attach(0,-1,0);buffer_delete(output);fdwatch_delete(watch);event_destroy();
    std::puts("blend module passed: complete parser/table/reload, original error FILE lifetime, all percent pairs and real packet/save/SQL");
}
