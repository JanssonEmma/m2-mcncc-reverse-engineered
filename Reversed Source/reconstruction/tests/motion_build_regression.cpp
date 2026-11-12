#include <map>
#include <string>
#include <vector>
#include <set>
#define private public
#define protected public
#include "../../game/src/motion.h"
#include "../../game/src/mob_manager.h"
#undef protected
#undef private
#include "../../game/src/text_file_loader.h"
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <direct.h>
#include <process.h>
#define MKDIR(p) _mkdir(p)
#define CHDIR _chdir
#define RMDIR _rmdir
#define GETCWD _getcwd
#define GETPID _getpid
#else
#include <sys/stat.h>
#include <unistd.h>
#define MKDIR(p) mkdir(p,0700)
#define CHDIR chdir
#define RMDIR rmdir
#define GETCWD getcwd
#define GETPID getpid
#endif
static std::string original,fixture;
static std::vector<std::string> files,folders;
static std::set<std::string> written;
static int build_errors,last_line;
static DWORD last_mob,last_skill;
extern "C" DWORD get_dword_time() { std::abort(); } // Unused CMobInstance clock boundary.
extern "C" void sys_log(int level,const char* format,...) { assert(level>=0 && level<=2 && format); }
extern "C" void _sys_err(const char* function,int line,const char* format,...) {
    if(!std::strcmp(function,"Build")) {
        assert(line==330 || line==286 || line==299 || line==310 || line==321);
        va_list args; va_start(args,format); last_mob=va_arg(args,DWORD); last_skill=va_arg(args,DWORD); va_end(args);
        last_line=line; ++build_errors;
    } else assert((!std::strcmp(function,"LoadFromFile") && line==515) || !std::strcmp(function,"LoadMobSkillFromFile"));
}
static void Cleanup() {
    for(std::vector<std::string>::reverse_iterator it=files.rbegin();it!=files.rend();++it) std::remove(it->c_str());
    for(std::vector<std::string>::reverse_iterator it=folders.rbegin();it!=folders.rend();++it) RMDIR(it->c_str());
    CHDIR(original.c_str()); RMDIR(fixture.c_str());
}
static void Make(const std::string& path) {
    std::string::size_type slash=0;
    while((slash=path.find('/',slash))!=std::string::npos) {
        std::string folder=path.substr(0,slash++);
        if(MKDIR(folder.c_str())==0) folders.push_back(folder); else assert(errno==EEXIST);
    }
}
static void Write(const std::string& path,const std::string& content) {
    Make(path); std::ofstream out(path.c_str(),std::ios::binary); out<<content; out.close(); assert(out);
    if(written.insert(path).second) files.push_back(path);
}
static std::string Motion(int duration,int y) { std::ostringstream out; out<<"MotionDuration "<<duration<<"\nAccumulation 0 "<<y<<" 0\n"; return out.str(); }
static std::string Skill() { return "MotionDuration 1\nGroup MotionEventData\n{\nMotionEventDataCount 0\n}\n"; }
static void Init(CMob& mob,DWORD vnum,const char* folder) {
    std::memset(&mob.m_table,0,sizeof(mob.m_table)); mob.m_table.dwVnum=vnum;
    std::strcpy(mob.m_table.szFolder,folder); std::strcpy(mob.m_table.szLocaleName,"fixture");
    for(int i=0;i<5;++i) { mob.m_table.Skills[i].dwVnum=500+i; mob.m_table.Skills[i].bLevel=static_cast<BYTE>(10+i); }
}
static void Modern(const std::string& folder,int missing) {
    std::string base="data/monster/"+folder+"/";
    Write(base+"motlist.txt","IGNORED wAlK walk.msa\nGENERAL RUN run.msa 100\nX NORMAL_ATTACK attack.msa\nX SPECIAL skill0.msa\nX SPECIAL1 skill1.msa 100\nX SPECIAL2 skill2.msa 100\nX SPECIAL3 skill3.msa 100\nX SPECIAL4 skill4.msa 100\n");
    Write(base+"walk.msa",Motion(2,-200)); Write(base+"run.msa",Motion(2,-600)); Write(base+"attack.msa",Motion(3,0));
    for(int i=0;i<5;++i) if(i!=missing) { std::ostringstream name; name<<base<<"skill"<<i<<".msa"; Write(name.str(),Skill()); }
}
int main() {
    char cwd[4096]; assert(GETCWD(cwd,sizeof(cwd))); original=cwd;
    for(int attempt=0;;++attempt) {
        std::ostringstream name; name<<"_mcncc_motion_build_"<<GETPID()<<"_"<<attempt; fixture=name.str();
        if(MKDIR(fixture.c_str())==0) break; assert(errno==EEXIST);
    }
    assert(!CHDIR(fixture.c_str())); std::atexit(Cleanup);
    const char* pc[8]={"data/pc/warrior","data/pc/assassin","data/pc/sura","data/pc2/shaman","data/pc2/warrior","data/pc2/assassin","data/pc2/sura","data/pc2/shaman"};
    const char* modes[8]={"general","onehand_sword","twohand_sword","dualhand_sword","bow","bell","fan","horse"};
    for(int race=0;race<8;++race) for(int mode=0;mode<8;++mode) {
        Write(std::string(pc[race])+"/"+modes[mode]+"/run.msa",Motion(race+1,-800));
        Write(std::string(pc[race])+"/"+modes[mode]+"/walk.msa",Motion(race+1,-400));
    }
    Write("data/monster/old/02.msa",Motion(2,-100)); Write("data/monster/old/03.msa",Motion(3,-300)); Write("data/monster/old/20.msa",Motion(1,0));
    Write("data/monster/old/24.msa",Skill()); Write("data/monster/old/25.msa",Skill());
    Modern("new",-1); Modern("special",-1);
    Write("data/monster/broken/motlist.txt","\nX WALK named.msa 100\n"); Write("data/monster/broken/02.msa",Motion(8,-100));
    {
        CMobManager mobs;
        CMob skipped,old,modern,broken,special; Init(skipped,999,""); Init(old,1000,"old"); Init(modern,1001,"new"); Init(broken,1002,"broken"); Init(special,2493,"special");
        broken.m_table.Skills[0].dwVnum=0; broken.m_table.Skills[1].dwVnum=900;
        mobs.m_map_pkMobByVnum[999]=&skipped; mobs.m_map_pkMobByVnum[1000]=&old; mobs.m_map_pkMobByVnum[1001]=&modern; mobs.m_map_pkMobByVnum[1002]=&broken; mobs.m_map_pkMobByVnum[2493]=&special;
        CMotionManager motions; assert(motions.Build() && !build_errors && motions.m_map_pkMotionSet.size()==12);
        for(DWORD race=0;race<8;++race) for(DWORD mode=0;mode<8;++mode) {
            assert(motions.GetMotion(race,MAKE_MOTION_KEY(mode,MOTION_RUN)) && motions.GetMotion(race,MAKE_MOTION_KEY(mode,MOTION_WALK)));
            assert(motions.GetMotionDuration(race,MAKE_MOTION_KEY(mode,MOTION_RUN))==(race==3 ? 8 : race+1));
        }
        assert(motions.GetMotionSet(3)!=motions.GetMotionSet(7) && motions.GetMotionDuration(3,0x300)==motions.GetMotionDuration(7,0x300));
        assert(!motions.GetMotionSet(999));
        assert(motions.GetMotionDuration(1000,0x200)==2 && motions.GetMotionDuration(1000,0x300)==3 && motions.GetMotionDuration(1000,0xd00)==1);
        assert(motions.GetMotion(1000,0x1d00) && motions.GetMotion(1000,0x1e00) && !motions.GetMotion(1000,0x1f00));
        assert(motions.GetMotion(1001,0xd00) && !motions.GetMotion(1001,0x1f00)); // Three tokens suffice; extra skill files are ignored for ordinary mobs.
        assert(motions.GetMotionSet(1002) && !motions.GetMotion(1002,0x200) && !motions.GetMotion(1002,0x1e00)); // Blank motlist terminates search; missing skill0 suppresses skill1.
        for(int i=0;i<5;++i) { assert(motions.GetMotion(2493,(29+i)<<8)); assert(special.m_mobSkillInfo[i].dwSkillVnum==500+static_cast<DWORD>(i) && special.m_mobSkillInfo[i].bSkillLevel==10+i); }
        mobs.m_map_pkMobByVnum.clear();
    }
    CTextFileLoader::DestroySystem();
    const int lines[5]={330,286,299,310,321};
    for(int missing=0;missing<5;++missing) {
        std::ostringstream folder; folder<<"fail"<<missing; Modern(folder.str(),missing);
        CMobManager mobs; CMob mob; DWORD id=missing>=2 ? 2493 : static_cast<DWORD>(3000+missing); Init(mob,id,folder.str().c_str()); mobs.m_map_pkMobByVnum[id]=&mob;
        CMotionManager motions; int before=build_errors;
        assert(!motions.Build() && build_errors==before+1 && last_line==lines[missing] && last_mob==id);
        assert(last_skill==(missing==0 ? 500UL : 501UL)); // Old slots2..4 report slot1, not their own skill.
        assert(motions.GetMotionSet(id) && motions.GetMotion(id,0x200)); // Failure retains partially constructed tables.
        mobs.m_map_pkMobByVnum.clear();
    }
    CTextFileLoader::DestroySystem();
    assert(!std::remove("data/pc/warrior/general/run.msa"));
    { CMobManager mobs; CMotionManager motions; assert(motions.Build() && motions.m_map_pkMotionSet.size()==8 && !motions.GetMotion(0,0x300) && motions.GetMotion(0,0x200)); }
    CTextFileLoader::DestroySystem();
    assert(MOTION_SPECIAL_1==29 && MOTION_SPECIAL_5==33 && MOTION_INTRO_WAIT==18);
    std::puts("motion build passed: real PC/monster assets, legacy/motlist lookup, mob2493 rules and retained error paths");
}
