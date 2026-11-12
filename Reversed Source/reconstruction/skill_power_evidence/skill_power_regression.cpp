// Full actual CTableBySkill; external stderr and new[]/delete[] observation only.
#define private public
#include "../../game/src/constants.h"
#undef private
#include "skill_power_api_checks.h"
#include "skill_power_alloc_observer.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <climits>
#ifdef _WIN32
#include <io.h>
#define SP_DUP _dup
#define SP_DUP2 _dup2
#define SP_FILENO _fileno
#define SP_CLOSE _close
#else
#include <unistd.h>
#define SP_DUP dup
#define SP_DUP2 dup2
#define SP_FILENO fileno
#define SP_CLOSE close
#endif
class Capture {
    FILE* file;int saved;
public:
    Capture():file(std::tmpfile()),saved(-1) {
        assert(file);std::fflush(stderr);saved=SP_DUP(SP_FILENO(stderr));assert(saved>=0);
        assert(SP_DUP2(SP_FILENO(file),SP_FILENO(stderr))>=0);
    }
    ~Capture() {
        std::fflush(stderr);assert(SP_DUP2(saved,SP_FILENO(stderr))>=0);
        SP_CLOSE(saved);std::fclose(file);
    }
    long Mark() {
        std::fflush(stderr);assert(std::fseek(file,0,SEEK_END)==0);
        long n=std::ftell(file);assert(n>=0);return n;
    }
    void Expect(long start,const char* expected) {
        long end=Mark();char text[256];
        assert(end-start>=0 && end-start<static_cast<long>(sizeof(text)));
        assert(std::fseek(file,start,SEEK_SET)==0);
        std::size_t n=std::fread(text,1,static_cast<std::size_t>(end-start),file);
        assert(n==static_cast<std::size_t>(end-start));text[n]=0;
        assert(std::strcmp(text,expected)==0);assert(std::fseek(file,0,SEEK_END)==0);
    }
};
static int Pattern(int slot,int level) {
    if(level==0) return INT_MIN+slot;
    if(level==40) return INT_MAX-slot;
    return slot*10003-level*97;
}
static void FillPower(int* a,int slot) {
    for(int i=0;i<41;++i) a[i]=Pattern(slot,i);
}
static void Presence(Capture& log) {
    int sentinel=7;
    for(unsigned mask=0;mask<256;++mask) {
        CTableBySkill t;
        assert(singleton<CTableBySkill>::instance_ptr()==&t);
        assert(&singleton<CTableBySkill>::instance()==&t);
        assert(&singleton<CTableBySkill>::Instance()==&t);
        assert(t.m_aiSkillDamageByLevel==0);
        for(int i=0;i<8;++i) {
            assert(t.m_aiSkillPowerByLevelFromType[i]==0);
            if(mask&(1U<<i)) t.m_aiSkillPowerByLevelFromType[i]=&sentinel;
        }
        const std::size_t allocations=skill_test_allocations();
        const std::size_t deletes=skill_test_deallocations();
        long start=log.Mark();bool ok=t.Check();
        assert(ok==(mask==255));
        char expected[128]="";
        if(mask!=255) {
            int first=0;while(mask&(1U<<first)) ++first;
            std::sprintf(expected,"[NO SETTING SKILL] aiSkillPowerByLevelFromType[%d]",first);
        }
        log.Expect(start,expected);
        for(int i=0;i<8;++i) {
            int* before=t.m_aiSkillPowerByLevelFromType[i];
            t.DeleteSkillPowerByLevelFromType(i);
            assert(t.m_aiSkillPowerByLevelFromType[i]==before);
            t.m_aiSkillPowerByLevelFromType[i]=0; // test storage cleanup only
        }
        assert(skill_test_allocations()==allocations && skill_test_deallocations()==deletes);
    }
    assert(singleton<CTableBySkill>::instance_ptr()==0);
}
static void Power(Capture& log) {
    int* leaked[16];std::size_t leakCount=0;
    {
        CTableBySkill t;
        // Guard paths must work before any table exists.
        assert(t.GetSkillPowerByLevelFromType(4,1,INT_MIN,false)==0);
        assert(t.GetSkillPowerByLevelFromType(INT_MAX,INT_MIN,INT_MAX,false)==0);
        assert(t.GetSkillPowerByLevelFromType(INT_MIN,0,INT_MIN,false)==0);
        for(int slot=0;slot<8;++slot) {
            int input[41];FillPower(input,slot);
            std::size_t a=skill_test_allocations(),d=skill_test_deallocations();
            long start=log.Mark();t.SetSkillPowerByLevelFromType(slot,input);
            char expected[64];std::sprintf(expected,"AddedSkill[%d][%d]\n",slot/2,slot%2);
            log.Expect(start,expected);
            int* installed=t.m_aiSkillPowerByLevelFromType[slot];
            assert(installed!=input && skill_test_size(installed)==164);
            assert(skill_test_allocations()==a+1 && skill_test_deallocations()==d);
            leaked[leakCount++]=installed;
            for(int level=0;level<41;++level) input[level]=0;
            for(int level=0;level<41;++level)
                assert(t.GetSkillPowerByLevelFromType(slot/2,slot%2+1,level,false)==Pattern(slot,level));
        }
        long start=log.Mark();assert(t.Check());log.Expect(start,"");
        for(int level=0;level<41;++level)
            assert(t.GetSkillPowerByLevelFromType(INT_MAX,INT_MIN,level,true)==Pattern(0,level));
        // Original has no group upper/lower check: test unusual but valid resulting indexes.
        assert(t.GetSkillPowerByLevelFromType(0,8,1,false)==Pattern(7,1));
        assert(t.GetSkillPowerByLevelFromType(-1,3,2,false)==Pattern(0,2));
        assert(t.GetSkillPowerByLevelFromType(1,-1,3,false)==Pattern(0,3));
        assert(t.GetSkillPowerByLevelFromType(INT_MIN,1,4,false)==Pattern(0,4));
        assert(t.GetSkillPowerByLevelFromType(INT_MIN+3,2,5,false)==Pattern(7,5));
        for(int slot=0;slot<8;++slot) {
            int* prior=t.m_aiSkillPowerByLevelFromType[slot];
            std::size_t deletes=skill_test_deallocations();
            t.DeleteSkillPowerByLevelFromType(slot);
            assert(t.m_aiSkillPowerByLevelFromType[slot]==prior && skill_test_live(prior));
            assert(skill_test_deallocations()==deletes);
            int input[41];FillPower(input,slot+10);
            long start=log.Mark();t.SetSkillPowerByLevelFromType(slot,input);
            char expected[64];std::sprintf(expected,"AddedSkill[%d][%d]\n",slot/2,slot%2);
            log.Expect(start,expected);
            assert(t.m_aiSkillPowerByLevelFromType[slot]!=prior && skill_test_live(prior));
            assert(skill_test_deallocations()==deletes);
            leaked[leakCount++]=t.m_aiSkillPowerByLevelFromType[slot];
            for(int level=0;level<41;++level)
                assert(t.GetSkillPowerByLevelFromType(slot/2,slot%2+1,level,false)==Pattern(slot+10,level));
        }
        const std::size_t deletes=skill_test_deallocations();
        t.DeleteAll();assert(skill_test_deallocations()==deletes);
        for(int i=0;i<8;++i) assert(t.m_aiSkillPowerByLevelFromType[i]!=0);
    }
    assert(singleton<CTableBySkill>::instance_ptr()==0);
    for(std::size_t i=0;i<leakCount;++i) {
        assert(skill_test_live(leaked[i]));delete[] leaked[i]; // external cleanup AFTER real destructor
    }
}
static void Damage(Capture& log) {
    int* power[8];
    std::size_t deletesBeforeDestructor=0;
    {
        CTableBySkill t;
        const int invalid[]={INT_MIN,-1,99,100,INT_MAX};
        for(unsigned i=0;i<sizeof(invalid)/sizeof(invalid[0]);++i)
            assert(t.GetSkillDamageByLevel(invalid[i])==0); // safe even when pointerNULL
        t.DeleteSkillDamageByLevelTable();t.DeleteAll();
        assert(t.m_aiSkillDamageByLevel==0);
        int input[99];
        for(int i=0;i<99;++i) input[i]=i==0?INT_MIN:(i==98?INT_MAX:-i*101);
        std::size_t a=skill_test_allocations(),d=skill_test_deallocations();
        long start=log.Mark();t.SetSkillDamageByLevelTable(input);log.Expect(start,"");
        assert(skill_test_allocations()==a+1 && skill_test_deallocations()==d);
        assert(skill_test_size(t.m_aiSkillDamageByLevel)==396);
        for(int i=0;i<99;++i) {
            int expected=input[i];input[i]=0;assert(t.GetSkillDamageByLevel(i)==expected);
        }
        for(unsigned i=0;i<sizeof(invalid)/sizeof(invalid[0]);++i)
            assert(t.GetSkillDamageByLevel(invalid[i])==0);
        for(int i=0;i<99;++i) input[i]=10000-i;
        d=skill_test_deallocations();a=skill_test_allocations();
        t.SetSkillDamageByLevelTable(input);
        assert(skill_test_deallocations()==d+1 && skill_test_allocations()==a+1);
        for(int i=0;i<99;++i) assert(t.GetSkillDamageByLevel(i)==input[i]);
        d=skill_test_deallocations();t.DeleteSkillDamageByLevelTable();
        assert(t.m_aiSkillDamageByLevel==0 && skill_test_deallocations()==d+1);
        t.DeleteSkillDamageByLevelTable();assert(skill_test_deallocations()==d+1);
        t.SetSkillDamageByLevelTable(input);
        d=skill_test_deallocations();t.DeleteAll();
        assert(t.m_aiSkillDamageByLevel==0 && skill_test_deallocations()==d+1);
        // Exercise destructor with all8 live power slots plus live damage storage.
        for(int i=0;i<8;++i) {
            int data[41];FillPower(data,i);
            t.SetSkillPowerByLevelFromType(i,data);power[i]=t.m_aiSkillPowerByLevelFromType[i];
        }
        t.SetSkillDamageByLevelTable(input);
        deletesBeforeDestructor=skill_test_deallocations();
    }
    assert(singleton<CTableBySkill>::instance_ptr()==0);
    assert(skill_test_deallocations()==deletesBeforeDestructor+1);
    for(int i=0;i<8;++i) { assert(skill_test_live(power[i]));delete[] power[i]; }
}
int main() {
    SkillPowerApiChecks();assert(sizeof(int)==4);
    assert(singleton<CTableBySkill>::instance_ptr()==0);
    { Capture log;Presence(log);Power(log);Damage(log); }
    assert(skill_test_allocations()==skill_test_deallocations());
    // Real virtual deleting destructor and singleton reset; no arrays are present here.
    singleton<CTableBySkill>* base=new CTableBySkill;delete base;
    assert(singleton<CTableBySkill>::instance_ptr()==0);
    std::puts("skill power: complete class, all256 presence masks,8x41 tables, damage99, exact diagnostics and original deletion behavior passed");
}
