#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
#include "../../game/src/item.h"
#include "../../game/src/motion.h"
#undef protected
#undef private
#include "../../game/src/utils.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
extern "C" void desc_test_set_millis(unsigned long);
extern "C" int desc_test_motion_errors(void);
static CMotion* Seed(CMotionManager& manager,DWORD race,DWORD key,float duration,float y) {
    CMotionSet* set=0;
    if(manager.m_map_pkMotionSet.count(race)) set=manager.m_map_pkMotionSet[race];
    else { set=new CMotionSet; manager.m_map_pkMotionSet[race]=set; }
    CMotion* motion=new CMotion;
    motion->m_stFileName="fixture/data/motion.msa"; motion->m_isEmpty=false; motion->m_isAccumulation=true;
    motion->m_fDuration=duration; motion->m_vec3Accumulation.y=y;
    set->Insert(key,motion); return motion;
}
int main() {
    desc_test_allow_clock(); desc_test_set_millis(1000);
    quest::CQuestManager quests;
    assert(!CMotionManager::instance_ptr());
    {
        CMotion empty; assert(empty.IsEmpty() && empty.m_stFileName.empty() && empty.GetDuration()==0);
        const D3DVECTOR& accumulation=empty.GetAccumVector(); assert(!accumulation.x && !accumulation.y && !accumulation.z);
        CMotionManager motions; assert(CMotionManager::instance_ptr()==&motions && motions.m_map_pkMotionSet.empty());
        assert(!motions.GetMotionSet(1) && !motions.GetMotion(1,2) && !motions.GetMotionDuration(1,2) && motions.m_map_pkMotionSet.empty());
        CMotion* run=Seed(motions,0,0x300,2,-400);
        CMotion* walk=Seed(motions,0,0x200,2,-200);
        assert(motions.GetMotion(0,0x300)==run && motions.GetMotionDuration(0,0x200)==2);
        CMotion* duplicate=new CMotion; motions.m_map_pkMotionSet[0]->Insert(0x300,duplicate);
        assert(motions.GetMotion(0,0x300)==run); delete duplicate;
        assert(!motions.GetMotion(0,0x999) && motions.m_map_pkMotionSet[0]->m_map_pkMotion.size()==2);
        CHARACTER character; character.m_points.job=0;
        assert(character.GetMotionMode()==0 && character.GetMoveMotionSpeed()==200);
        DescOutputProbe descriptor; character.BindDesc(&descriptor); character.m_points.stamina=1;
        character.m_bNowWalking=false; assert(character.GetMoveMotionSpeed()==200);
        character.m_bNowWalking=true; assert(character.GetMoveMotionSpeed()==100);
        character.m_bNowWalking=false; character.m_points.stamina=0; assert(character.GetMoveMotionSpeed()==100);
        character.BindDesc(0); character.m_bNowWalking=true; assert(character.GetMoveMotionSpeed()==200);
        TItemTable prototype; std::memset(&prototype,0,sizeof(prototype));
        CItem weapon; weapon.Initialize(); weapon.SetProto(&prototype); character.m_pointsInstant.pItems[94]=&weapon;
        const DWORD modes[]={1,3,4,2,5,6,0,0};
        for(unsigned i=0;i<8;++i) { prototype.bSubType=static_cast<BYTE>(i); assert(character.GetMotionMode()==modes[i]); }
        prototype.bSubType=3; Seed(motions,0,0x02000300,1,-400); assert(character.GetMoveMotionSpeed()==400);
        character.m_dwPolymorphRace=5000; assert(character.GetMotionMode()==0);
        Seed(motions,5000,0x300,1,-175); assert(character.GetMoveMotionSpeed()==175);
        character.m_dwPolymorphRace=0; character.m_pointsInstant.pItems[94]=0;
        Seed(motions,20101,0x300,2,-600); Seed(motions,20101,0x200,2,-120);
        character.m_dwMountVnum=20101; assert(character.GetMoveMotionSpeed()==300);
        character.BindDesc(&descriptor); character.m_bNowWalking=true; assert(character.GetMoveMotionSpeed()==60);
        Seed(motions,0,0x07000300,1,-123); Seed(motions,0,0x07000200,1,-70);
        character.m_dwMountVnum=20102; assert(character.GetMoveMotionSpeed()==70);
        character.m_bNowWalking=false; character.m_points.stamina=10; assert(character.GetMoveMotionSpeed()==123);
        character.m_dwMountVnum=0; character.m_dwPolymorphRace=60000;
        int errors=desc_test_motion_errors(); assert(character.GetMoveMotionSpeed()==300 && desc_test_motion_errors()==errors+1);
        character.m_dwPolymorphRace=0; character.BindDesc(0);
        struct DurationCase { int speed,duration,expected; } cases[]={
            {0,1000,2000},{50,1000,1500},{100,1000,1000},{150,1000,660},{170,1000,580},
            {200,1001,500},{250,1000,400},{101,1000,990},{-100,1000,3000},
            {100,-101,-101},{200,-101,-50},{2147483647,1000,0},{(-2147483647-1),1000,0},
            {100,2147483647,-1},{100,(-2147483647-1),0}
        };
        for(unsigned i=0;i<sizeof(cases)/sizeof(cases[0]);++i) assert(CalculateDuration(cases[i].speed,cases[i].duration)==cases[i].expected);
        character.m_pointsInstant.points[19]=100; character.SetXYZ(10,20,30);
        character.m_posDest.x=310; character.m_posDest.y=420; character.m_posDest.z=999; character.m_posStart.z=77;
        character.m_bCharType=6; character.CalculateMoveDuration();
        assert(character.m_dwMoveDuration==2500 && character.m_dwMoveStartTime==1000 && character.m_posStart.x==10 && character.m_posStart.y==20 && character.m_posStart.z==77);
        character.SetPoint(7,-99); assert(character.GetPoint(7)==-99 && character.m_dwMoveDuration==2500 && character.m_dwMoveStartTime==1000);
        desc_test_set_millis(1500); character.SetPoint(19,200);
        assert(character.m_dwMoveDuration==1250 && character.m_dwMoveStartTime==1500 && character.GetPoint(19)==200);
        desc_test_set_millis(2750); character.SetPoint(19,150);
        assert(character.m_dwMoveDuration==1250 && character.m_dwMoveStartTime==1500); // Equality is already finished.
        character.m_dwMoveStartTime=0xfffffff0UL; character.m_dwMoveDuration=32;
        desc_test_set_millis(20); character.SetPoint(19,100); assert(character.m_dwMoveDuration==32 && character.m_dwMoveStartTime==0xfffffff0UL);
        desc_test_set_millis(5); character.SetPoint(19,100); assert(character.m_dwMoveStartTime==5 && character.m_dwMoveDuration==2500);
        // Float-to-int conversion truncates rather than rounding16.66ms to17.
        character.m_posDest.x=13; character.m_posDest.y=24; character.m_bCharType=0; run->m_fDuration=1; run->m_vec3Accumulation.y=-300;
        character.CalculateMoveDuration(); assert(character.m_dwMoveDuration==16);
        // x87 integer-indefinite conversion plus wrapped duration math is retained for zero motion speed.
        run->m_vec3Accumulation.y=0; character.CalculateMoveDuration(); assert(character.m_dwMoveDuration==0);
        run->m_vec3Accumulation.y=-400; run->m_fDuration=0;
        assert(character.GetMoveMotionSpeed()==std::numeric_limits<float>::infinity());
        character.CalculateMoveDuration(); assert(character.m_dwMoveDuration==0);
        (void)walk;
    }
    assert(!CMotionManager::instance_ptr());
    std::puts("movement passed: motion ownership/lookups, mode/walk/mount selection, duration truncation/wrap and speed updates");
}
