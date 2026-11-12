#include <map>
#include <set>
#include <list>
#include <string>
#include <vector>
#define private public
#include "../../game/src/mob_manager.h"
#undef private
#include <cassert>
#include <climits>
#include <cstdarg>
#include <cstdio>
#include <cstring>
static unsigned logs=0;
static int last_index=-1;
static DWORD last_timing=0,last_distance=0;
extern "C" void sys_log(int level,const char* format,...) {
    assert(level==0 && !std::strcmp(format,"SkillIndex %d timing %lu hit_distance %lu"));
    va_list args; va_start(args,format);
    last_index=va_arg(args,int); last_timing=va_arg(args,DWORD); last_distance=va_arg(args,DWORD);
    va_end(args); ++logs;
}
static unsigned clock_reads=0;
extern "C" DWORD get_dword_time() { return ++clock_reads==1?0xffffffffUL:7; }
int main() {
    CMob source;
    //The old default constructor leaves scalars indeterminate: supply explicit test data.
    std::memset(&source.m_table,0xa5,sizeof source.m_table);
    for(unsigned i=0;i<5;++i) {
        assert(source.m_mobSkillInfo[i].vecSplashAttack.empty());
        source.m_mobSkillInfo[i].dwSkillVnum=100+i;
        source.m_mobSkillInfo[i].bSkillLevel=static_cast<BYTE>(9+i);
    }
    source.AddSkillSplash(-1,1,2); source.AddSkillSplash(INT_MIN,1,2);
    source.AddSkillSplash(5,1,2); source.AddSkillSplash(INT_MAX,1,2); assert(logs==0);
    for(int i=0;i<5;++i) {
        source.AddSkillSplash(i,static_cast<DWORD>(i),0xffffffffUL);
        assert(last_index==i && last_timing==static_cast<DWORD>(i) && last_distance==0xffffffffUL);
    }
    for(unsigned i=0;i<128;++i) source.AddSkillSplash(2,i,0x80000000UL+i);
    assert(logs==133 && source.m_mobSkillInfo[2].vecSplashAttack.size()==129);
    TMobTable expected; std::memcpy(&expected,&source.m_table,sizeof expected);
    for(unsigned i=2;i<5;++i) { expected.Skills[i].dwVnum=256; expected.Skills[i].bLevel=25; }
    const CMob* original=&source; CMob clone(original);
    assert(!std::memcmp(&clone.m_table,&expected,sizeof expected));
    assert(source.m_table.Skills[2].bLevel==0xa5);
    for(unsigned i=0;i<5;++i) {
        assert(clone.m_mobSkillInfo[i].dwSkillVnum==source.m_mobSkillInfo[i].dwSkillVnum);
        assert(clone.m_mobSkillInfo[i].bSkillLevel==source.m_mobSkillInfo[i].bSkillLevel);
        assert(clone.m_mobSkillInfo[i].vecSplashAttack.size()==source.m_mobSkillInfo[i].vecSplashAttack.size());
        for(unsigned j=0;j<source.m_mobSkillInfo[i].vecSplashAttack.size();++j) {
            assert(clone.m_mobSkillInfo[i].vecSplashAttack[j].dwTiming==source.m_mobSkillInfo[i].vecSplashAttack[j].dwTiming);
            assert(clone.m_mobSkillInfo[i].vecSplashAttack[j].dwHitDistance==source.m_mobSkillInfo[i].vecSplashAttack[j].dwHitDistance);
        }
    }
    source.m_mobSkillInfo[2].vecSplashAttack[0].dwTiming=999;
    source.m_mobSkillInfo[2].vecSplashAttack.clear();
    assert(clone.m_mobSkillInfo[2].vecSplashAttack.size()==129 && clone.m_mobSkillInfo[2].vecSplashAttack[0].dwTiming==2);
    clone.AddSkillSplash(0,7,8); assert(source.m_mobSkillInfo[0].vecSplashAttack.size()==1);
    CMob empty_source;
    std::memset(&empty_source.m_table,0,sizeof empty_source.m_table);
    for(unsigned i=0;i<5;++i) { empty_source.m_mobSkillInfo[i].dwSkillVnum=0; empty_source.m_mobSkillInfo[i].bSkillLevel=0; }
    CMob empty_clone(&empty_source); assert(empty_clone.m_mobSkillInfo[4].vecSplashAttack.empty());
    assert(empty_clone.m_table.Skills[4].dwVnum==256 && empty_clone.m_table.Skills[4].bLevel==25);
    CMobInstance instance;
    assert(clock_reads==2 && instance.m_dwLastAttackedTime==0xffffffffUL && instance.m_dwLastWarpTime==7);
    assert(!instance.m_IsBerserk && !instance.m_IsGodSpeed && !instance.m_IsRevive);
    assert(!CMobManager::instance_ptr());
    {
        CMobManager manager; assert(CMobManager::instance_ptr()==&manager);
        assert(!manager.Get(1UL) && !manager.Get("",true) && !manager.Get("none",false) && !manager.GetGroup(1));
        manager.m_map_pkMobByVnum[0xffffffffUL]=&source;
        manager.m_map_pkMobByName["AlphaBeta"]=&source;
        manager.m_map_pkMobByName["Alpha"]=&clone;
        manager.m_map_pkMobByName["alpha"]=&empty_source;
        assert(manager.Get(0xffffffffUL)==&source && !manager.Get(0UL));
        assert(manager.Get("AlphaBeta",false)==&source && !manager.Get("ALPHA",false));
        assert(manager.Get("Al",true)==&clone && manager.Get("",true)==&clone);
        assert(manager.Get("alpha",true)==&empty_source && !manager.Get("AlphaBetaLonger",true));
        assert(!manager.Get("Al",false) && !manager.Get("Z",true));
        int opaque=0; CMobGroup* group=reinterpret_cast<CMobGroup*>(&opaque);
        manager.m_map_pkMobGroup[0x80000000UL]=group;
        assert(manager.GetGroup(0x80000000UL)==group && !manager.GetGroup(0));
        assert(manager.m_map_pkMobByVnum.size()==1 && manager.m_map_pkMobByName.size()==3 && manager.m_map_pkMobGroup.size()==1);
    }
    assert(!CMobManager::instance_ptr());
    //Stack mobs remain usable: manager destruction destroys registries, not pointees.
    assert(clone.m_mobSkillInfo[2].vecSplashAttack.size()==129);
    std::puts("mob lifecycle passed: clone table overrides, independent splash vectors, all slots and invalid indices");
}
