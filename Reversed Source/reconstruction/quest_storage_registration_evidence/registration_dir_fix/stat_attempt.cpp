#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#undef private
#include "../../game/src/config.h"
#include "../../libthecore/include/main.h"
#include <cassert>
#include <climits>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <fstream>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError() { std::abort(); }
extern "C" void desc_test_allow_clock(void);
static const std::string root="quest_registration_inputs";
static quest::CQuestManager* active;
static int loadLogs,registerLogs,mode;
static std::string lastName;
static unsigned int lastIndex;
extern "C" void quest_registration_log(int kind,const char* name,unsigned int index) {
    lastName=name;lastIndex=index;
    if(kind==1) {
        ++loadLogs;
        if(mode==1) { // Real external diagnostic occurs before script slot access.
            mode=0;assert(active->GetQuestIndexByName("callback")==31);
            assert(!active->m_mapQuestNameByIndex.count(31));
            active->RegisterQuest("callback",99); // Already inserted -> complete early-return branch.
            active->m_hmQuestStartScript[31].push_back('!');
            active->m_mapQuestNameByIndex.insert(std::make_pair(31U,std::string("diagnostic-wins")));
            g_setQuestObjectDir.insert(root+"/b"); // Live set iteration visits new next path.
        }
    } else {assert(kind==2);++registerLogs;}
}
static void Directory(const std::string& path) {
#ifdef _WIN32
    int result=_mkdir(path.c_str());
#else
    int result=mkdir(path.c_str(),0700);
#endif
    if(result!=0) {
#ifdef _WIN32
        struct _stat info;
        assert(_stat(path.c_str(),&info)==0 && (info.st_mode&_S_IFDIR));
#else
        struct stat info;
        assert(stat(path.c_str(),&info)==0 && S_ISDIR(info.st_mode));
#endif
    }
}
static void File(const std::string& dir,const std::string& name,const char* bytes,unsigned int count) {
    std::ofstream output((root+"/"+dir+"/begin_condition/"+name).c_str(),std::ios::binary|std::ios::trunc);
    assert(output.is_open());output.write(bytes,count);output.close();assert(output.good());
}
static void Inputs() {
    Directory(root);
    const char* dirs[]={"a","b","z"};
    for(unsigned i=0;i<3;++i) {Directory(root+"/"+dirs[i]);Directory(root+"/"+dirs[i]+"/begin_condition");}
    File("a","append","A\0\200\n",4);File("z","append","Z\377\n",3);
    File("a","empty","",0);File("z","empty","",0);
    File("a","registered","R",1);File("z","registered","S",1);
    File("a","alias","x",1);File("z","alias","y",1);
    File("a","callback","a",1);File("b","callback","b",1);File("z","callback","z",1);
}
static std::string Script(quest::CQuestManager& manager,unsigned int index) {
    std::vector<char>& data=manager.m_hmQuestStartScript.find(index)->second;
    return std::string(data.begin(),data.end());
}
static void ResetLogs() {loadLogs=registerLogs=0;lastName="";lastIndex=0;}
int main(int argc,char** argv) {
    desc_test_allow_clock();assert(sizeof(int)==4&&sizeof(unsigned int)==4);
    quest::CQuestManager manager;active=&manager;
    if(argc>1&&!std::strcmp(argv[1],"--fatal-zero")) {std::fputs("REGISTER-ZERO\n",stderr);manager.RegisterQuest("zero",0);return 9;}
    Inputs();std::set<std::string> saved=g_setQuestObjectDir;g_setQuestObjectDir.clear();
    ResetLogs();manager.LoadStartQuest("append",7);assert(manager.m_hmQuestStartScript.empty()&&!loadLogs);
    manager.RegisterQuest("without-path",1);
    assert(manager.GetQuestIndexByName("without-path")==1&&manager.GetQuestNameByIndex(1)=="without-path"&&!manager.m_hmQuestStartScript.count(1)&&registerLogs==1);
    g_setQuestObjectDir.insert(root+"/z");g_setQuestObjectDir.insert(root+"/a"); // Set traversal is lexical, not insertion order.
    ResetLogs();manager.LoadStartQuest("missing",8);assert(!manager.m_hmQuestStartScript.count(8)&&!loadLogs);
    manager.LoadStartQuest("empty",9);assert(manager.m_hmQuestStartScript.count(9)&&Script(manager,9).empty()&&loadLogs==2);
    const std::string payload=std::string("A\0\200\nZ\377\n",7);
    manager.m_hmQuestStartScript[7].push_back('P');ResetLogs();manager.LoadStartQuest("append",7);
    assert(Script(manager,7)=="P"+payload&&loadLogs==2);
    manager.LoadStartQuest("append",7);assert(Script(manager,7)=="P"+payload+payload&&loadLogs==4);
    manager.LoadStartQuest("append",0);assert(Script(manager,0)==payload); // Loader itself has no nonzero assertion.
    ResetLogs();manager.RegisterQuest("registered",17);
    assert(manager.GetQuestIndexByName("registered")==17&&manager.GetQuestNameByIndex(17)=="registered"&&Script(manager,17)=="RS");
    assert(loadLogs==2&&registerLogs==1&&lastName=="registered"&&lastIndex==17);
    manager.RegisterQuest("registered",18);assert(loadLogs==2&&registerLogs==1&&!manager.m_mapQuestNameByIndex.count(18)&&!manager.m_hmQuestStartScript.count(18));
    manager.RegisterQuest("alias",17);assert(manager.GetQuestIndexByName("alias")==17&&manager.GetQuestNameByIndex(17)=="registered"&&Script(manager,17)=="RSxy");
    std::string embedded("registered\0tail",15);manager.RegisterQuest(embedded,19);
    assert(manager.GetQuestIndexByName(embedded)==19&&manager.GetQuestNameByIndex(19)==embedded&&Script(manager,19)=="RS"); // c_str filesystem path truncates at NUL.
    ResetLogs();mode=1;manager.RegisterQuest("callback",31);
    assert(mode==0&&loadLogs==3&&registerLogs==1&&Script(manager,31)=="!abz");
    assert(manager.GetQuestIndexByName("callback")==31&&manager.GetQuestNameByIndex(31)=="diagnostic-wins"&&!manager.m_mapQuestNameByIndex.count(99));
    ResetLogs();manager.RegisterQuest("registered",UINT_MAX);assert(!loadLogs&&!registerLogs&&manager.GetQuestIndexByName("registered")==17);
    ResetLogs();manager.RegisterQuest("maximum",UINT_MAX);
    assert(!loadLogs&&registerLogs==1&&manager.GetQuestIndexByName("maximum")==UINT_MAX&&manager.GetQuestNameByIndex(UINT_MAX)=="maximum");
    g_setQuestObjectDir=saved;active=0;assert(!event_count());event_destroy();
    std::puts("quest registration passed: actual file/STL loading and config set, append/duplicate/callback semantics; no VM substitution");
}
// Fixtures use binary writes only to create exact TEST bytes. Production loader
// retains original default text mode. CRLF/CTRL-Z host translations are excluded
// from these byte assertions and documented as Windows-vs-FreeBSD boundaries.
