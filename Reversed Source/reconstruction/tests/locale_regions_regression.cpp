// Include the complete actual original TU to call its ORIGINAL static helpers.
#include "../../game/src/locale_service.cpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <set>
#include <string>
#ifdef _WIN32
#include <io.h>
#include <process.h>
#define LR_DUP _dup
#define LR_DUP2 _dup2
#define LR_FILENO _fileno
#define LR_CLOSE _close
#else
#include <unistd.h>
#include <sys/wait.h>
#define LR_DUP dup
#define LR_DUP2 dup2
#define LR_FILENO fileno
#define LR_CLOSE close
#endif
extern "C" unsigned locale_regions_already_errors(void);
int max_bytes_written,current_bytes_written,total_bytes_written;
typedef int(*NameFn)(const char*);
struct Region {
 const char* name;void(*fn)();const char* encoding;const char* base;
 const char* quest;const char* map;const char* file;
 NameFn namefn;NameFn bytefn;const DWORD* exp;const int* delta;
 const int* boss;const int* chain;int pk;const char* message;
};
#define COMMON(N,F,E,CB,PK) {N,F,E,"locale/" N,"locale/" N "/quest","locale/" N "/map","locale/" N "/locale_string.txt",CB,0,0,0,0,0,PK,"Init " N "\n"}
static const Region regions[]={
 {"cibn",__LocaleService_Init_CIBN,"gb2312","locale/cibn","locale/cibn/quest","locale/cibn/map","locale/cibn/gb2312_string.txt",check_name_gb2312,is_twobyte_gb2312,exp_table_gb2312,aiPercentByDeltaLev_gb2312,aiPercentByDeltaLev_gb2312,aiChainLightningCountBySkillLevel_gb2312,10,"Init cinb\n"},
 {"cibn_old",__LocaleService_Init_CIBN_OLD,"gb2312",".","quest","data/map","gb2312_string.txt",check_name_gb2312,is_twobyte_gb2312,exp_table_gb2312,aiPercentByDeltaLev_gb2312,aiPercentByDeltaLev_gb2312,aiChainLightningCountBySkillLevel_gb2312,10,"Init cibn_old\n"},
 {"japan",__LocaleService_Init_JAPAN,"sjis","locale/japan","locale/japan/quest","locale/japan/map","locale/japan/sjis_string.txt",check_name_sjis,0,exp_table_sjis,aiPercentByDeltaLev_sjis,0,0,-1,"Init japan\n"},
 {"english",__LocaleService_Init_English,"","locale/english","locale/english/quest","locale/english/map","locale/english/eng_string.txt",0,0,0,0,0,0,-1,"Init english\n"},
 {"hongkong",__LocaleService_Init_HongKong,"big5","locale/hongkong","locale/hongkong/quest","locale/hongkong/map","locale/hongkong/big5_string.txt",check_name_big5,is_twobyte_big5,exp_table_hk,0,0,0,-1,"Init Hongkong\n"},
 {"newcibn",__LocaleService_Init_NewCIBN,"gb2312","locale/newcibn","locale/newcibn/quest","locale/newcibn/map","locale/newcibn/gb2312_string.txt",check_name_gb2312,is_twobyte_gb2312,exp_table_newcibn,0,0,0,-1,"Init NewCIBN\n"},
 {"germany",__LocaleService_Init_Germany,"latin1","locale/germany","locale/germany/quest","locale/germany/map","locale/germany/locale_string.txt",check_name_latin1,0,0,0,0,0,15,"Init getmany\n"},
 {"korea",__LocaleService_Init_Korea,"euckr","locale/korea","locale/korea/quest","locale/korea/map",0,0,0,0,0,0,0,-1,"Init korea\n"},
 COMMON("france",__LocaleService_Init_France,"latin1",check_name_latin1,15),
 COMMON("italy",__LocaleService_Init_Italy,"latin1",check_name_latin1,15),
 COMMON("spain",__LocaleService_Init_spain,"latin1",check_name_latin1,15),
 COMMON("uk",__LocaleService_Init_UK,"latin1",check_name_latin1,15),
 COMMON("turkey",__LocaleService_Init_Turkey,"latin5",check_name_latin1,15),
 COMMON("poland",__LocaleService_Init_Poland,"latin2",check_name_latin1,15),
 COMMON("portugal",__LocaleService_Init_Portugal,"latin1",check_name_latin1,15),
 {"canada",__LocaleService_Init_Canada,"latin1","locale/canada","locale/canada/quest","locale/canada/map","locale/canada/locale_string.txt",check_name_alphabet,0,0,0,0,0,-1,"Init canada"},
 {"brazil",__LocaleService_Init_Brazil,"latin1","locale/brazil","locale/brazil/quest","locale/brazil/map","locale/brazil/locale_string.txt",check_name_alphabet,0,0,0,0,0,-1,"Init brazil"}
};
#undef COMMON
struct State {
 std::string service,local,encoding,base,quest,map,file,defaultObject;
 std::set<std::string> objects;NameFn namefn,bytefn;const DWORD* exp;
 const int *delta,*boss,*chain;int use,pk,server;eLocalization localization;bool auth;
 State():service(g_stServiceName),local(g_stLocal),encoding(g_stLocale),base(g_stServiceBasePath),quest(g_stQuestDir),map(g_stServiceMapPath),file(g_stLocaleFilename),defaultObject(g_stDefaultQuestObjectDir),objects(g_setQuestObjectDir),namefn(check_name),bytefn(is_twobyte),exp(exp_table),delta(aiPercentByDeltaLev),boss(aiPercentByDeltaLevForBoss),chain(aiChainLightningCountBySkillLevel),use(g_iUseLocale),pk(PK_PROTECT_LEVEL),server(test_server),localization(g_eLocalType),auth(g_bAuthServer) {}
 void Expect() const {
  assert(g_stServiceName==service && g_stLocal==local && g_stLocale==encoding);
  assert(g_stServiceBasePath==base && g_stQuestDir==quest && g_stServiceMapPath==map && g_stLocaleFilename==file);
  assert(g_stDefaultQuestObjectDir==defaultObject && g_setQuestObjectDir==objects);
  assert(check_name==namefn && is_twobyte==bytefn && exp_table==exp);
  assert(aiPercentByDeltaLev==delta && aiPercentByDeltaLevForBoss==boss && aiChainLightningCountBySkillLevel==chain);
  assert(g_iUseLocale==use && PK_PROTECT_LEVEL==pk && test_server==server && g_eLocalType==localization && g_bAuthServer==auth);
 }
 void RegionResult(const Region& r) {
  encoding=r.encoding;base=r.base;quest=r.quest;map=r.map;if(r.file)file=r.file;
  objects.clear();objects.insert(quest+"/object");use=1;
  if(r.namefn)namefn=r.namefn;if(r.bytefn)bytefn=r.bytefn;if(r.exp)exp=r.exp;
  if(r.delta)delta=r.delta;if(r.boss)boss=r.boss;if(r.chain)chain=r.chain;if(r.pk>=0)pk=r.pk;
 }
 void DefaultResult() {base="locale/"+service;map=base+"/map";quest=base+"/quest";file="";objects.clear();objects.insert(quest+"/object");}
};
static void Seed(unsigned mode,unsigned setmode) {
 g_stServiceName="unrelated-service";g_stLocal="unrelated-localization";g_stLocale="unrelated-encoding";
 g_stServiceBasePath="obsolete-base";g_stServiceMapPath="obsolete-map";g_stQuestDir="obsolete-quest";g_stLocaleFilename="keep-translation";
 g_stDefaultQuestObjectDir="unrelated-default-object";g_eLocalType=LC_BRAZIL;g_iUseLocale=19;PK_PROTECT_LEVEL=29;g_bAuthServer=true;test_server=-1;
 check_name=mode?check_name_independent:0;is_twobyte=mode?is_twobyte_euckr:0;exp_table=mode?exp_table_euckr:0;
 aiPercentByDeltaLev=mode?aiPercentByDeltaLev_euckr:0;aiPercentByDeltaLevForBoss=mode?aiPercentByDeltaLevForBoss_euckr:0;
 aiChainLightningCountBySkillLevel=mode?aiChainLightningCountBySkillLevel_euckr:0;
 g_setQuestObjectDir.clear();if(setmode)g_setQuestObjectDir.insert("obsolete");
 if(setmode>1){g_setQuestObjectDir.insert("another");g_setQuestObjectDir.insert(std::string("x\0y",3));}
}
static std::string Read(const std::string& name) {
 FILE* f=std::fopen(name.c_str(),"rb");assert(f);std::string s;char b[256];std::size_t n;
 while((n=std::fread(b,1,sizeof b,f))!=0)s.append(b,n);
 assert(!std::ferror(f));assert(std::fclose(f)==0);return s;
}
class Capture {
 int saved;std::string filename;
 public:Capture(const char* f):filename(f){assert(std::fflush(stdout)==0);saved=LR_DUP(LR_FILENO(stdout));assert(saved>=0);assert(std::freopen(f,"wb",stdout));}
 std::string Bytes(){assert(std::fflush(stdout)==0);return Read(filename);}
 ~Capture(){assert(std::fflush(stdout)==0);assert(LR_DUP2(saved,LR_FILENO(stdout))>=0);assert(LR_CLOSE(saved)==0);std::clearerr(stdout);assert(std::remove(filename.c_str())==0);}
};
static void DirectRegions() {
 for(unsigned r=0;r<17;++r)for(unsigned mode=0;mode<2;++mode)for(unsigned setmode=0;setmode<3;++setmode){
  Seed(mode,setmode);State expected;expected.RegionResult(regions[r]);
  for(unsigned repeat=0;repeat<2;++repeat){g_setQuestObjectDir.insert("reintroduced");regions[r].fn();expected.Expect();}
 }
 // Consecutive real region transitions prove missing defaults stay inherited.
 Seed(1,2);State expected;
 for(unsigned r=0;r<17;++r){expected.RegionResult(regions[r]);regions[r].fn();expected.Expect();}
}
static void SuccessfulDispatch() {
 for(unsigned r=2;r<17;++r)for(unsigned mode=0;mode<2;++mode){
  Seed(mode,2);g_stServiceName="";State expected;expected.service=regions[r].name;expected.RegionResult(regions[r]);
  Capture capture("locale_regions_stdout.tmp");assert(LocaleService_Init(regions[r].name));expected.Expect();
  const std::string output=std::string(regions[r].message)+"Setting Locale =\""+regions[r].name+"\"\n"+regions[r].base+"\n";
  assert(capture.Bytes()==output);
  const unsigned errors=locale_regions_already_errors();assert(!LocaleService_Init("different"));assert(locale_regions_already_errors()==errors+1);
  expected.Expect();assert(capture.Bytes()==output);
  assert(!LocaleService_Init(g_stServiceName));expected.Expect();assert(locale_regions_already_errors()==errors+2);
 }
 Seed(1,2);g_stServiceName="";State expected;expected.service="ymir";expected.DefaultResult();
 {Capture capture("locale_regions_stdout.tmp");assert(LocaleService_Init("ymir"));expected.Expect();assert(capture.Bytes()=="Init else\nSetting Locale =\"ymir\"\nlocale/ymir\n");}
 // Existing nonempty service rejects every input before any initializer writes.
 const char* rejected[]={"","ymir","cibn","cibn_old","custom","japan"};
 for(unsigned i=0;i<sizeof rejected/sizeof rejected[0];++i){State unchanged;unsigned errors=locale_regions_already_errors();Capture capture("locale_regions_stdout.tmp");assert(!LocaleService_Init(rejected[i]));unchanged.Expect();assert(capture.Bytes().empty());assert(locale_regions_already_errors()==errors+1);}
 // Direct helper accepts all16 complete strings and mutates no state.
 const char* accepted[]={"ymir","korea","japan","newcibn","brazil","hongkong","canada","english","germany","italy","poland","portugal","turkey","uk","france","spain"};
 State unchanged;for(unsigned i=0;i<16;++i){__CheckPlayerSlot(accepted[i]);unchanged.Expect();}
}
static bool completed_or_expected_exit;
static void RejectUnexpectedExit() {assert(completed_or_expected_exit);}
static State* exitExpected;static std::string witness;
static void VerifyActualExit() {
 assert(exitExpected);exitExpected->Expect();assert(std::fflush(stdout)==0);
 FILE* f=std::fopen(witness.c_str(),"wb");assert(f);const char text[]="actual exit0 after complete initialization";
 assert(std::fwrite(text,1,sizeof text-1,f)==sizeof text-1);assert(std::fclose(f)==0);completed_or_expected_exit=true;
}
static std::string Invalid(unsigned n) {
 const char* names[]={"cibn","cibn_old","custom","","JAPAN","lets_game","japan/","japan "};
 if(n<8)return names[n];return std::string("japan\0suffix",12);
}
static int InvalidChild(unsigned n,const char* file) {
 Seed(1,2);g_stServiceName="";const std::string name=Invalid(n);State expected;expected.service=name;
 if(n<2)expected.RegionResult(regions[n]);else expected.DefaultResult();
 exitExpected=&expected;witness=std::string(file)+".state";
 assert(std::freopen(file,"wb",stdout));assert(std::atexit(VerifyActualExit)==0);
 LocaleService_Init(name);std::abort();return 99;
}
static void InvalidChildren(const char* executable) {
 for(unsigned n=0;n<9;++n){char num[16],file[96];std::sprintf(num,"%u",n);std::sprintf(file,"locale_regions_invalid_%u.tmp",n);const std::string state=std::string(file)+".state";std::remove(file);std::remove(state.c_str());
#ifdef _WIN32
  const std::string argv0=std::string("\"")+executable+"\"";
  errno=0;
  const int status=static_cast<int>(_spawnl(_P_WAIT,executable,argv0.c_str(),"--invalid",num,file,static_cast<char*>(0)));
  const int spawn_errno=errno;
  if(status!=0) {
   std::fprintf(stderr,"locale regions spawn failure: status=%d errno=%d n=%u executable=[%s] argv0=[%s] file=[%s]\n",status,spawn_errno,n,executable,argv0.c_str(),file);
   std::fflush(stderr);
  }
  assert(status==0);
#else
  pid_t pid=fork();assert(pid>=0);if(pid==0){execl(executable,executable,"--invalid",num,file,static_cast<char*>(0));_exit(99);}int status;assert(waitpid(pid,&status,0)==pid);assert(WIFEXITED(status)&&WEXITSTATUS(status)==0);
#endif
  const std::string name=Invalid(n);const std::string printable=name.c_str();const std::string base=n<2?regions[n].base:std::string("locale/")+printable;
  const std::string output=std::string(n<2?regions[n].message:"Init else\n")+"Setting Locale =\""+printable+"\"\n"+base+"\n<ERROR> PLAYER_PER_ACCOUNT = 4\n";
  assert(Read(file)==output);assert(Read(state)=="actual exit0 after complete initialization");assert(std::remove(file)==0 && std::remove(state.c_str())==0);
 }
}
int main(int argc,char** argv) {
 assert(PLAYER_PER_ACCOUNT==4);assert(std::atexit(RejectUnexpectedExit)==0);
 if(argc==4 && !std::strcmp(argv[1],"--invalid"))return InvalidChild(static_cast<unsigned>(std::atoi(argv[2])),argv[3]);
 assert(argc==1);DirectRegions();SuccessfulDispatch();InvalidChildren(argv[0]);completed_or_expected_exit=true;
 std::puts("locale regions passed:17 full static bodies,204 poisoned/repeated cases+17 inherited transitions,31 successful dispatches, exact errors/output and9 actual exit0 children");
}
