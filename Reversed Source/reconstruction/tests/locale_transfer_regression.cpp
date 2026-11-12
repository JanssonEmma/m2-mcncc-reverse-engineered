// Complete real TransferDefaultSetting and real CTableBySkill::Check.
// TEST-only compiler entry hooks observe actual Check: no appdata/return mutation.
#include <string>
#define private public
#include "../../game/src/constants.h"
#undef private
#include "../../game/src/config.h"
#include "../../game/src/locale_service.h"
#include "../../game/src/locale.hpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#ifdef _WIN32
#include <io.h>
#include <process.h>
#define LT_DUP _dup
#define LT_DUP2 _dup2
#define LT_FILENO _fileno
#define LT_CLOSE _close
#else
#include <unistd.h>
#include <sys/wait.h>
#define LT_DUP dup
#define LT_DUP2 dup2
#define LT_FILENO fileno
#define LT_CLOSE close
#endif
#if defined(__GNUC__)
#define LT_NO_INSTRUMENT __attribute__((no_instrument_function))
#else
#error This test uses GCC object-specific function instrumentation
#endif
extern std::string g_stServiceName,g_stServiceBasePath,g_stServiceMapPath,g_stLocal;
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" {
void locale_transfer_set_check_target(void*);
void locale_transfer_reset_check_entries(void);
unsigned locale_transfer_check_entry_count(void);
}
typedef int(*NameCallback)(const char*);
static struct Context {
    bool active;unsigned seen;int missing;
    NameCallback name,two;
    const DWORD* exp;
    const int* bossBefore;const int* normalBefore;const int* chainBefore;
    const CTableBySkill* table;int* powers[8];int* damage;
    Context():active(false),seen(0),missing(-1),name(0),two(0),exp(0),bossBefore(0),
        normalBefore(0),chainBefore(0),table(0),damage(0) {
        for(int i=0;i<8;++i) powers[i]=0;
    }
} context;
static void AssertEarly() {
    assert(check_name==context.name && is_twobyte==context.two && exp_table==context.exp);
}
static void AssertLateBefore() {
    assert(aiPercentByDeltaLevForBoss==context.bossBefore);
    assert(aiPercentByDeltaLev==context.normalBefore);
    assert(aiChainLightningCountBySkillLevel==context.chainBefore);
}
static void AssertTables() {
    for(int i=0;i<8;++i) assert(context.table->m_aiSkillPowerByLevelFromType[i]==context.powers[i]);
    assert(context.table->m_aiSkillDamageByLevel==context.damage);
}
extern "C" void LT_NO_INSTRUMENT locale_transfer_observe_check(unsigned entries) {
    assert(context.active && entries==context.seen+1 && entries<=8);
    ++context.seen;
    AssertEarly();AssertLateBefore();AssertTables();
    // Observation only: no application field, callback, return or table mutation.
}
static void LT_NO_INSTRUMENT RegisterActualCheck() {
    bool(CTableBySkill::*method)()=&CTableBySkill::Check;
    void* code=0;
    assert(sizeof(method)>=sizeof(code));
    // GCC x86 nonvirtual member-function pointer ABI: first word is code pointer.
    // Byte copy is test-only address identification, not a call through a fake signature.
    std::memcpy(&code,&method,sizeof(code));assert(code);
    locale_transfer_set_check_target(code);
}
static void Configure(unsigned mask) {
    check_name=(mask&1)?check_name_alphabet:0;
    is_twobyte=(mask&2)?is_twobyte_big5:0;
    exp_table=(mask&4)?exp_table_newcibn:0;
    aiPercentByDeltaLevForBoss=(mask&8)?aiPercentByDeltaLev_gb2312:0;
    aiPercentByDeltaLev=(mask&16)?aiPercentByDeltaLev_sjis:0;
    aiChainLightningCountBySkillLevel=(mask&32)?aiChainLightningCountBySkillLevel_gb2312:0;
}
static void Begin(CTableBySkill& table,int missing) {
    context.active=false;context.seen=0;context.missing=missing;context.table=&table;
    context.name=check_name?check_name:check_name_euckr;
    context.two=is_twobyte?is_twobyte:is_twobyte_euckr;
    context.exp=exp_table?exp_table:exp_table_euckr;
    context.bossBefore=aiPercentByDeltaLevForBoss;
    context.normalBefore=aiPercentByDeltaLev;
    context.chainBefore=aiChainLightningCountBySkillLevel;
    for(int i=0;i<8;++i) context.powers[i]=table.m_aiSkillPowerByLevelFromType[i];
    context.damage=table.m_aiSkillDamageByLevel;
    locale_transfer_reset_check_entries();context.active=true;
}
static void AssertLateAfter() {
    assert(aiPercentByDeltaLevForBoss==(context.bossBefore?context.bossBefore:aiPercentByDeltaLevForBoss_euckr));
    assert(aiPercentByDeltaLev==(context.normalBefore?context.normalBefore:aiPercentByDeltaLev_euckr));
    assert(aiChainLightningCountBySkillLevel==(context.chainBefore?context.chainBefore:aiChainLightningCountBySkillLevel_euckr));
}
static void OtherState() {
    assert(g_stServiceName=="transfer-service" && g_stServiceBasePath=="transfer/base" &&
           g_stServiceMapPath=="transfer/map" && g_stLocaleFilename=="transfer-file");
    assert(g_stLocale=="transfer-encoding" && g_stLocal=="transfer-local");
    assert(g_eLocalType==LC_CANADA && g_iUseLocale==7 && PK_PROTECT_LEVEL==17);
    assert(g_bAuthServer && test_server==0);
}
static void SetOtherState() {
    g_stServiceName="transfer-service";g_stServiceBasePath="transfer/base";g_stServiceMapPath="transfer/map";
    g_stLocaleFilename="transfer-file";g_stLocale="transfer-encoding";g_stLocal="transfer-local";
    g_eLocalType=LC_CANADA;g_iUseLocale=7;PK_PROTECT_LEVEL=17;g_bAuthServer=true;test_server=0;
}
class Capture {
    FILE* file;int saved;
public:
    Capture():file(std::tmpfile()),saved(-1) {
        assert(file);std::fflush(stderr);saved=LT_DUP(LT_FILENO(stderr));assert(saved>=0);
        assert(LT_DUP2(LT_FILENO(file),LT_FILENO(stderr))>=0);
    }
    ~Capture() {
        std::fflush(stderr);assert(LT_DUP2(saved,LT_FILENO(stderr))>=0);
        LT_CLOSE(saved);std::fclose(file);
    }
    long Mark() {
        std::fflush(stderr);assert(std::fseek(file,0,SEEK_END)==0);
        long n=std::ftell(file);assert(n>=0);return n;
    }
    void ExpectEmpty(long start) {assert(Mark()==start);}
};
static void Fill(CTableBySkill& table,int missing,int** owned) {
    for(int slot=0;slot<8;++slot) {
        owned[slot]=0;
        if(slot==missing) continue;
        int data[41];for(int i=0;i<41;++i) data[i]=slot*1000-i*17;
        table.SetSkillPowerByLevelFromType(slot,data);
        owned[slot]=table.m_aiSkillPowerByLevelFromType[slot];assert(owned[slot]);
    }
}
static void Success(Capture& capture) {
    int* owned[8];
    {
        CTableBySkill table;Fill(table,-1,owned);
        int damage[99];for(int i=0;i<99;++i) damage[i]=i*31;
        table.SetSkillDamageByLevelTable(damage);
        for(unsigned mask=0;mask<64;++mask) {
            Configure(mask);
            for(int repeat=0;repeat<2;++repeat) {
                Begin(table,-1);const long start=capture.Mark();
                LocaleService_TransferDefaultSetting();
                context.active=false;
                assert(context.seen==8 && locale_transfer_check_entry_count()==8);
                AssertEarly();AssertLateAfter();AssertTables();OtherState();
                for(int slot=0;slot<8;++slot)
                    for(int i=0;i<41;++i) assert(table.GetSkillPowerByLevelFromType(slot/2,slot%2+1,i,false)==slot*1000-i*17);
                for(int i=0;i<99;++i) assert(table.GetSkillDamageByLevel(i)==damage[i]);
                capture.ExpectEmpty(start);
            }
        }
    }
    assert(singleton<CTableBySkill>::instance_ptr()==0);
    // Original destructor leaked power tables. Test cleanup AFTER actual destruction.
    for(int i=0;i<8;++i) delete[] owned[i];
}
static std::string witness;
static void LT_NO_INSTRUMENT VerifyFailureAtExit() {
    assert(context.active && context.missing>=0 && context.missing<8);
    assert(context.seen==1 && locale_transfer_check_entry_count()==1);
    AssertEarly();AssertLateBefore();AssertTables();OtherState();
    assert(context.table->m_aiSkillPowerByLevelFromType[context.missing]==0);
    assert(!aiPercentByDeltaLevForBoss && !aiPercentByDeltaLev && !aiChainLightningCountBySkillLevel);
    assert(std::fflush(stderr)==0);
    FILE* out=std::fopen(witness.c_str(),"wb");assert(out);
    const char text[]="actual exit1 state verified";
    assert(std::fwrite(text,1,sizeof(text)-1,out)==sizeof(text)-1);
    assert(std::fclose(out)==0);
    // Return normally to REAL std::exit(1); no exit replacement or status override.
}
static int MissingChild(int slot,const char* filename) {
    assert(slot>=0 && slot<8);
    assert(std::freopen(filename,"wb",stderr));
    SetOtherState();CTableBySkill table;int* owned[8];Fill(table,slot,owned);
    // Remove setup diagnostics; subsequent bytes come only from actual Check and Transfer.
    assert(std::freopen(filename,"wb",stderr));
    Configure(0);Begin(table,slot);
    witness=std::string(filename)+".state";
    assert(std::atexit(VerifyFailureAtExit)==0);
    LocaleService_TransferDefaultSetting();
    std::abort(); // Real missing-table path must exit1 and never return.
    return 99;
}
static std::string Read(const std::string& name) {
    FILE* file=std::fopen(name.c_str(),"rb");assert(file);
    std::string data;char block[256];std::size_t n;
    while((n=std::fread(block,1,sizeof block,file))!=0) data.append(block,n);
    assert(!std::ferror(file));assert(std::fclose(file)==0);return data;
}
static void Children(const char* executable) {
    for(int slot=0;slot<8;++slot) {
        char number[16],filename[96];std::sprintf(number,"%d",slot);
        std::sprintf(filename,"locale_transfer_missing_%d.tmp",slot);
        const std::string state=std::string(filename)+".state";
        std::remove(filename);std::remove(state.c_str());
#ifdef _WIN32
        const std::string argv0=std::string("\"")+executable+"\"";
        errno=0;
        const int status=static_cast<int>(_spawnl(_P_WAIT,executable,argv0.c_str(),"--missing",number,filename,static_cast<char*>(0)));
        const int spawn_errno=errno;
        if(status!=1) {
            std::fprintf(stderr,"locale transfer spawn failure: status=%d errno=%d slot=%d executable=[%s] argv0=[%s] file=[%s]\n",status,spawn_errno,slot,executable,argv0.c_str(),filename);
            std::fflush(stderr);
        }
        assert(status==1);
#else
        pid_t pid=fork();assert(pid>=0);
        if(pid==0) {execl(executable,executable,"--missing",number,filename,static_cast<char*>(0));_exit(99);}
        int status;assert(waitpid(pid,&status,0)==pid);
        assert(WIFEXITED(status) && WEXITSTATUS(status)==1);
#endif
        char diagnostic[256];
        std::sprintf(diagnostic,"[NO SETTING SKILL] aiSkillPowerByLevelFromType[%d][NO SETTING SKILL] aiSkillPowerByLevelFromType[0]",slot);
        assert(Read(filename)==diagnostic); // No newline; outer job remains0 on first failed Check.
        assert(Read(state)=="actual exit1 state verified");
        assert(std::remove(filename)==0 && std::remove(state.c_str())==0);
    }
}
int main(int argc,char** argv) {
    assert(sizeof(int)==4);RegisterActualCheck();
    if(argc==4 && !std::strcmp(argv[1],"--missing"))
        return MissingChild(std::atoi(argv[2]),argv[3]);
    assert(argc==1);
    SetOtherState();
    {Capture capture;Success(capture);}
    Children(argv[0]);
    context.active=false;locale_transfer_set_check_target(0);
    check_name=0;is_twobyte=0;exp_table=0;aiPercentByDeltaLevForBoss=0;
    aiPercentByDeltaLev=0;aiChainLightningCountBySkillLevel=0;
    std::puts("locale transfer passed:64 combinations/repeat calls,8 actual Check entries each, ordered defaults/state isolation and8 real exit1 diagnostics/atexit-state children");
}
