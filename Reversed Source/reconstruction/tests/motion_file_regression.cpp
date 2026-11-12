#include <map>
#include <string>
#include <vector>
#define protected public
#include "../../game/src/motion.h"
#undef protected
#include "../../game/src/mob_manager.h"
#include "../../game/src/text_file_loader.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
static unsigned errors,logs;
static int last_error;
extern "C" void sys_log(int level,const char* format,...) { assert(level>=0 && level<=2 && format); ++logs; }
extern "C" void _sys_err(const char* function,int line,const char* format,...) {
    assert((std::string(function)=="LoadFromFile" && line==515) || std::string(function)=="LoadMobSkillFromFile");
    assert(format); ++errors; last_error=line;
}
// CMobInstance's unused clock boundary. Motion/skill loading does not call this.
extern "C" DWORD get_dword_time() { std::abort(); }
static const char* filename="./_mcncc_motion_asset.tmp";
static void Cleanup() { std::remove(filename); }
static void Write(const std::string& text) { std::ofstream out(filename,std::ios::binary); out<<text; out.close(); assert(out); }
static std::string Skill(const std::string& events,int count) {
    std::ostringstream text;
    text<<"motionduration 2\naccumulation 0 -999 0\nGroup MotionEventData\n{\nMotionEventDataCount "<<count<<"\n"<<events<<"}\n";
    return text.str();
}
int main() {
    std::atexit(Cleanup);
    {
        CMotion motion;
        assert(!motion.LoadFromFile("./_mcncc_motion_missing.tmp") && motion.IsEmpty() && motion.m_stFileName=="./_mcncc_motion_missing.tmp");
        Write("MotionDuration 2\nAccumulation 1 -500 3\n");
        assert(motion.LoadFromFile(filename) && !motion.IsEmpty() && motion.GetDuration()==2 && motion.m_isAccumulation && motion.GetAccumVector().y==-500 && motion.m_stFileName==filename);
        Write("MotionDuration 3\n");
        assert(motion.LoadFromFile(filename) && motion.GetDuration()==3 && motion.m_isAccumulation && motion.GetAccumVector().y==-500);
        Write("Accumulation 9 -10 11\n");
        unsigned before=errors;
        assert(!motion.LoadFromFile(filename) && errors==before+1 && last_error==515 && motion.GetDuration()==3 && !motion.IsEmpty() && motion.GetAccumVector().y==-500);
        CMotion fresh; Write("MotionDuration 0\n"); assert(fresh.LoadFromFile(filename) && !fresh.m_isAccumulation && fresh.GetDuration()==0 && !fresh.IsEmpty());
        Write("MotionDuration 2\nAccumulation 0 -600 0\n");
        CMotionSet set; assert(set.Load(filename,3,2));
        const CMotion* loaded=set.GetMotion(0x03000200); assert(loaded && loaded->GetDuration()==2 && loaded->GetAccumVector().y==-600);
        assert(!set.Load("./_mcncc_motion_missing.tmp",1,2) && !set.GetMotion(0x01000200));
        CMob mob; std::memset(&mob.m_table,0,sizeof(mob.m_table));
        for(int i=0;i<5;++i) { mob.m_table.Skills[i].dwVnum=100+i; mob.m_table.Skills[i].bLevel=static_cast<BYTE>(10+i); mob.m_mobSkillInfo[i].dwSkillVnum=999; mob.m_mobSkillInfo[i].bSkillLevel=99; }
        CMotion skill;
        std::string events=
            "Group Event00\n{\nMotionEventType 2\n}\n"
            "Group Event01\n{\nMotionEventType 4\nStartingTime 0.3759\nGroup SphereData00\n{\nPosition 1 -200.75 3\n}\n}\n"
            "Group Event02\n{\nMotionEventType 4\nStartingTime -0.15\nGroup SphereData00\n{\nPosition 0 4294967296 0\n}\n}\n";
        Write(Skill(events,3));
        assert(skill.LoadMobSkillFromFile(filename,&mob,2));
        assert(!skill.IsEmpty() && skill.GetDuration()==2 && skill.m_stFileName.empty() && !skill.m_isAccumulation && !skill.GetAccumVector().y);
        assert(mob.m_mobSkillInfo[2].dwSkillVnum==102 && mob.m_mobSkillInfo[2].bSkillLevel==12);
        const std::vector<TMobSplashAttackInfo>& splash=mob.m_mobSkillInfo[2].vecSplashAttack;
        assert(splash.size()==2 && splash[0].dwTiming==475 && splash[0].dwHitDistance==200);
        assert(splash[1].dwTiming==0xffffffceUL && splash[1].dwHitDistance==0);
        assert(skill.LoadMobSkillFromFile(filename,&mob,2) && splash.size()==4); // Loads append rather than clear prior splash entries.
        CMotion failed;
        mob.m_mobSkillInfo[2].dwSkillVnum=12345;
        Write(Skill("Group Event00\n{\nMotionEventType 4\nStartingTime 1\nGroup SphereData00\n{\nPosition 0 -50 0\n}\n}\nGroup Event01\n{\nMotionEventType 4\n}\n",2));
        before=errors; assert(!failed.LoadMobSkillFromFile(filename,&mob,2) && last_error==463 && errors==before+1);
        assert(failed.IsEmpty() && failed.GetDuration()==2 && splash.size()==5 && splash[4].dwTiming==1100 && splash[4].dwHitDistance==50 && mob.m_mobSkillInfo[2].dwSkillVnum==12345);
        Write(Skill("",1)); assert(!failed.LoadMobSkillFromFile(filename,&mob,2) && last_error==433);
        Write(Skill("Group Event00\n{\nStartingTime 0\n}\n",1)); assert(!failed.LoadMobSkillFromFile(filename,&mob,2) && last_error==440);
        Write(Skill("Group Event00\n{\nMotionEventType 4\nGroup SphereData00\n{\nOther 1\n}\n}\n",1)); assert(!failed.LoadMobSkillFromFile(filename,&mob,2) && last_error==471);
        Write(Skill("Group Event00\n{\nMotionEventType 4\nGroup SphereData00\n{\nPosition 0 -10 0\n}\n}\n",1)); assert(!failed.LoadMobSkillFromFile(filename,&mob,2) && last_error==486);
        Write("Accumulation 0 0 0\n"); assert(!failed.LoadMobSkillFromFile(filename,&mob,2) && last_error==407);
        Write("MotionDuration 7\nGroup MotionEventData\n{\nOther 1\n}\n");
        assert(failed.LoadMobSkillFromFile(filename,&mob,2) && !failed.IsEmpty() && failed.GetDuration()==7 && splash.size()==5); // Missing count keeps the old cursor/early-success behavior.
        assert(mob.m_mobSkillInfo[2].dwSkillVnum==102);
    }
    CTextFileLoader::DestroySystem(); assert(logs && errors==7);
    std::puts("motion files passed: actual parser, retained motion fields, keyed loads and monster splash/error semantics");
}
