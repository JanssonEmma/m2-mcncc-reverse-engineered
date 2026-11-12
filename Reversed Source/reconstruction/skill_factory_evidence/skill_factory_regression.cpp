#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <ext/hash_map>
#include <new>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <climits>
#define private public
#define protected public
#include "../../game/src/skill.h"
#include "../../common/tables.h"
#include "../../game/src/questmanager.h"
#include "../../game/src/char.h"
#include "../../game/src/desc_manager.h"
#undef protected
#undef private
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
extern "C" void desc_test_allow_clock(void);
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
struct SPointOnType{const char* c_pszName;int iPointOn;};extern SPointOnType kPointOnTypes[32];extern int FindPointType(const char*);
struct Allocation{CSkillProto* p;bool freed;};static Allocation allocations[256];static unsigned count;static bool allocating;
static CSkillManager* skills;static CSkillProto* eraseOnDelete;static DWORD eraseKey;static bool erasedCurrent;
void* operator new(std::size_t n) throw(std::bad_alloc){void* p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();std::memset(p,0xa5,n);if(allocating&&n==sizeof(CSkillProto)){assert(count<256);allocations[count].p=static_cast<CSkillProto*>(p);allocations[count++].freed=false;}return p;}
void operator delete(void* p) throw(){if(p)for(unsigned i=0;i<count;++i)if(allocations[i].p==p&&!allocations[i].freed){allocations[i].freed=true;break;}if(p&&p==eraseOnDelete){eraseOnDelete=0;erasedCurrent=true;assert(skills->m_map_pkSkillProto[eraseKey]==p);skills->m_map_pkSkillProto.erase(eraseKey);}std::free(p);}
static std::vector<std::string> events;static std::vector<int> errors;static std::string lastError;static LPBUFFER gmOutput;static bool packetsActive;
static TSkillTable* callbackRow;static bool fixMaster,changeID;static DWORD newID;
static std::vector<std::pair<std::string,std::string> > comparisons;
extern "C" int skill_factory_strcasecmp(const char* a,const char* b){assert(a&&b);comparisons.push_back(std::make_pair(std::string(a),std::string(b)));for(;;++a,++b){unsigned x=(unsigned char)*a,y=(unsigned char)*b;if(x>='A'&&x<='Z')x+='a'-'A';if(y>='A'&&y<='Z')y+='a'-'A';if(x!=y)return x<y?-1:1;if(!x)return 0;}}
extern "C" void skill_factory_log(int kind,const char*,const char* text,const char* name){events.push_back(std::string(kind?"row:":"master:")+text);if(!kind&&fixMaster){assert(callbackRow);std::strcpy(callbackRow->szMasterBonusPoly,"x+2");std::strcpy(callbackRow->szPointPoly,"late");}if(kind&&changeID){bool found=false;for(unsigned i=0;i<count;++i)if(!allocations[i].freed&&allocations[i].p->szName==name){allocations[i].p->dwVnum=newID;found=true;break;}assert(found);}}
extern "C" void skill_factory_error(const char* function,int line,const char* format,const char* text){assert(!std::strcmp(function,"Initialize")&&!std::strcmp(format,"%s"));errors.push_back(line);lastError=text;events.push_back("error:"+lastError);}
static void* Copy(void* out,const void* in,size_t bytes){if(packetsActive&&out==gmOutput->write_point){const unsigned char* b=static_cast<const unsigned char*>(in);assert(bytes>=10&&b[0]==4&&b[3]==2&&bytes==(unsigned(b[1])|(unsigned(b[2])<<8)));assert(b[4]==0&&b[5]==0&&b[6]==0&&b[7]==0&&b[8]==1&&b[bytes-1]==0);events.push_back(std::string("send:")+reinterpret_cast<const char*>(b+9));}return std::memcpy(out,in,bytes);}
struct Fixture{
 quest::CQuestManager quests;DESC_MANAGER descriptors;CHARACTER gm,normal;DescOutputProbe desc[3];LPBUFFER outputs[3];LPFDWATCH watch;
 Fixture():watch(fdwatch_new(16)){for(int i=0;i<3;++i){outputs[i]=buffer_new(0,8192);desc[i].Attach(watch,5+i,outputs[i]);desc[i].SetPacketState(5);desc[i].GetAccountTable().bEmpire=1;descriptors.m_set_pkDesc.insert(&desc[i]);}gm.BindDesc(&desc[0]);desc[0].BindCharacter(&gm);gm.m_pointsInstant.gm_level=1;gm.m_bCharType=0;normal.BindDesc(&desc[1]);desc[1].BindCharacter(&normal);normal.m_pointsInstant.gm_level=0;normal.m_bCharType=0;gmOutput=outputs[0];thecore_memcpy=Copy;}
 ~Fixture(){packetsActive=false;thecore_memcpy=std::memcpy;descriptors.m_set_pkDesc.clear();gm.BindDesc(0);normal.BindDesc(0);for(int i=0;i<3;++i){desc[i].BindCharacter(0);desc[i].Attach(0,-1,0);buffer_delete(outputs[i]);}fdwatch_delete(watch);}
 void Begin(){for(int i=0;i<3;++i)buffer_reset(outputs[i]);events.clear();errors.clear();lastError.clear();packetsActive=true;}
 void End(){packetsActive=false;assert(!buffer_size(outputs[1])&&!buffer_size(outputs[2]));}
};
static void Base(TSkillTable& t,DWORD id,const char* name){std::memset(&t,0,sizeof t);t.dwVnum=id;std::strncpy(t.szName,name,32);t.bType=250;t.bMaxLevel=255;t.bLevelLimit=254;t.bLevelStep=253;t.dwSplashRange=0x80000001UL;t.dwTargetRange=0xffffffffUL;t.dwFlag=0xfedcba98UL;t.dwAffectFlag=0x80000000UL;t.dwAffectFlag2=0xffffffffUL;t.preSkillVnum=0xffffffffUL;t.preSkillLevel=255;t.lMaxHit=-2147483647L;t.bSkillAttrType=252;std::strcpy(t.szPointOn,"HP");std::strcpy(t.szPointOn2,"SP");std::strcpy(t.szPointOn3,"");char* expressions[]={t.szSplashAroundDamageAdjustPoly,t.szPointPoly,t.szPointPoly2,t.szPointPoly3,t.szSPCostPoly,t.szGrandMasterAddSPCostPoly,t.szDurationPoly,t.szDurationPoly2,t.szDurationPoly3,t.szDurationSPCostPoly,t.szCooldownPoly,t.szMasterBonusPoly};for(unsigned i=0;i<12;++i)std::strcpy(expressions[i],"x+1");}
static CSkillProto* Old(CSkillManager& m,DWORD id,const char* name){allocating=true;CSkillProto* p=new CSkillProto;allocating=false;p->dwVnum=id;std::strcpy(p->szName,name);m.m_map_pkSkillProto[id]=p;return p;}
static bool Owned(CSkillManager& m,CSkillProto* p){for(std::map<DWORD,CSkillProto*>::iterator it=m.m_map_pkSkillProto.begin();it!=m.m_map_pkSkillProto.end();++it)if(it->second==p)return true;return false;}
static void CleanupOrphans(CSkillManager& m){for(unsigned i=0;i<count;++i)if(!allocations[i].freed&&!Owned(m,allocations[i].p))delete allocations[i].p;}
static bool Initialize(Fixture& f,CSkillManager& m,TSkillTable* t,int n){f.Begin();allocating=true;bool ok=m.Initialize(t,n);allocating=false;f.End();return ok;}
static void ErrorOrder(int site){assert(lastError.size()<CHAT_MAX_LEN);assert(errors.size()==1&&errors[0]==site);unsigned at=0;for(;at<events.size()&&events[at]!=("error:"+lastError);++at){}if(!(at+1<events.size()&&events[at+1]=="send:"+lastError)){std::fprintf(stderr,"factory observer failure site=%d events=%u error=[%s]\n",site,(unsigned)events.size(),lastError.c_str());for(unsigned z=0;z<events.size();++z)std::fprintf(stderr,"event[%u]=[%s]\n",z,events[z].c_str());std::fflush(stderr);}assert(at+1<events.size()&&events[at+1]=="send:"+lastError);assert(events.back()=="send:There were erros when loading skill table");}
int main(){
 assert(sizeof(TSkillTable)==1475&&sizeof(DWORD)==4&&sizeof(long)==4);desc_test_allow_clock();Fixture f;CSkillManager manager;skills=&manager;
 assert(!manager.Get(static_cast<const char*>(0)));assert(FindPointType("hp")==5&&FindPointType("SP")==7&&FindPointType("")==-1&&FindPointType("\n")==-1);
 const char* saved=kPointOnTypes[0].c_pszName;kPointOnTypes[0].c_pszName="\n";comparisons.clear();assert(FindPointType(0)==-1&&comparisons.empty());kPointOnTypes[0].c_pszName=saved;
 CSkillProto* old=Old(manager,100,"old");
 const int sites[]={122,135,154,166,175,184,193,202,211,220,229,238,247,256,266};
 for(unsigned which=0;which<15;++which){TSkillTable row;Base(row,1,"e");char* fields[]={row.szPointOn,row.szPointOn2,row.szPointOn3,row.szSplashAroundDamageAdjustPoly,row.szPointPoly,row.szPointPoly2,row.szPointPoly3,row.szSPCostPoly,row.szGrandMasterAddSPCostPoly,row.szDurationPoly,row.szDurationPoly2,row.szDurationPoly3,row.szDurationSPCostPoly,row.szCooldownPoly,row.szMasterBonusPoly};std::strcpy(fields[which],which<3?"?":"@");unsigned start=count;assert(!Initialize(f,manager,&row,1)&&count==start+1&&allocations[start].freed&&manager.Get(100UL)==old&&manager.m_map_pkSkillProto.size()==1);ErrorOrder(sites[which]);}
 // On mixed failure the VALID temporary prototype leaks while old map survives.
 {TSkillTable rows[2];Base(rows[0],1,"staged");Base(rows[1],2,"bad");std::strcpy(rows[1].szPointOn,"unknown");unsigned start=count;assert(!Initialize(f,manager,rows,2));assert(count==start+2&&!allocations[start].freed&&allocations[start+1].freed&&!Owned(manager,allocations[start].p)&&manager.Get(100UL)==old);CleanupOrphans(manager);}
 // Legal external allocator callback erases current old mapnode during deletion:
 // original increment-before-delete keeps the next iterator valid.
 {TSkillTable rows[2];Base(rows[0],7,"first");Base(rows[1],7,"second");unsigned start=count;eraseOnDelete=old;eraseKey=100;erasedCurrent=false;assert(Initialize(f,manager,rows,2)&&erasedCurrent);assert(count==start+2&&manager.Get(7UL)==allocations[start].p&&!allocations[start+1].freed&&!Owned(manager,allocations[start+1].p));assert(events.back()=="send:Skill Prototype reloaded!");CleanupOrphans(manager);}
 // Nonpositive counts intentionally perform an empty successful replacement.
 assert(Initialize(f,manager,0,0)&&manager.m_map_pkSkillProto.empty());Old(manager,1,"Case");Old(manager,0x80000000UL,"cASE");Old(manager,0xffffffffUL,"other");comparisons.clear();assert(manager.Get("CASE")==manager.Get(1UL)&&comparisons.size()==1&&comparisons[0].first=="Case"&&comparisons[0].second=="CASE");comparisons.clear();assert(!manager.Get("missing")&&comparisons.size()==3&&comparisons[1].first=="cASE"&&comparisons[2].first=="other");assert(Initialize(f,manager,0,-7)&&manager.m_map_pkSkillProto.empty());Old(manager,3,"old-negative");assert(Initialize(f,manager,0,INT_MIN)&&manager.m_map_pkSkillProto.empty());
 // Master log callback fixes upcoming formula, changes already-parsed table text;
 // summary callback changes proto ID, which local insert must reload afterward.
 TSkillTable row;Base(row,21,"callback");std::strcpy(row.szMasterBonusPoly,"@");callbackRow=&row;fixMaster=true;changeID=true;newID=77;assert(Initialize(f,manager,&row,1));fixMaster=changeID=false;CSkillProto* p=manager.Get(77UL);assert(p&&!manager.Get(21UL)&&p->dwVnum==77&&row.dwVnum==21&&p->kPointPoly.strData=="x+1"&&p->kMasterBonusPoly.strData=="x+2");
 assert(p->bType==250&&p->bMaxLevel==255&&p->bLevelLimit==254&&p->bLevelStep==253&&p->iSplashRange==-2147483647&&p->dwTargetRange==0xffffffffUL&&p->dwFlag==0xfedcba98UL&&p->dwAffectFlag==0x80000000UL&&p->dwAffectFlag2==0xffffffffUL&&p->preSkillVnum==0xffffffffUL&&p->preSkillLevel==255&&p->lMaxHit==-2147483647L&&p->bSkillAttrType==252&&p->bPointOn==5&&p->bPointOn2==7&&p->bPointOn3==0);
 const unsigned char* untouched=reinterpret_cast<const unsigned char*>(&p->dwFlag2);for(unsigned i=0;i<sizeof p->dwFlag2;++i)assert(untouched[i]==0xa5); // bytes only, no indeterminate DWORD read.
 p->SetPointVar("x",3);assert(p->kPointPoly.Eval()==4&&p->kPointPoly2.Eval()==4&&p->kPointPoly3.Eval()==4&&p->kMasterBonusPoly.Eval()==5);p->SetDurationVar("x",4);assert(p->kDurationPoly.Eval()==5&&p->kDurationPoly2.Eval()==5&&p->kDurationPoly3.Eval()==5);p->SetSPCostVar("x",5);assert(p->kSPCostPoly.Eval()==6&&p->kGrandMasterAddSPCostPoly.Eval()==6&&p->kDurationSPCostPoly.Eval()==1&&p->kCooldownPoly.Eval()==1&&p->kSplashAroundDamageAdjustPoly.Eval()==1);
 assert(events[0]=="master:Master @"&&events[1].find("point_poly: late")!=std::string::npos&&events.back()=="send:Skill Prototype reloaded!");

// Fold into future full skill_factory_regression before its final empty reload.
// Uses its ACTUAL full live prototype, restores manager object invariant afterward.
{
    const DWORD saved=p->dwVnum;
    for(DWORD n=0;n<513;++n) { p->dwVnum=n;assert(p->IsChargeSkill()==(n==5||n==138)); }
    const DWORD raw[]={0x80000000UL,0xffffffffUL,0x10000005UL,0x1000008aUL};
    for(unsigned i=0;i<4;++i) { p->dwVnum=raw[i];assert(!p->IsChargeSkill()); }
    p->dwVnum=saved;
}

 assert(Initialize(f,manager,0,0));CleanupOrphans(manager);for(unsigned i=0;i<count;++i)assert(allocations[i].freed);skills=0;
 std::puts("real skill factory/parser/SendLog passed: all15 failure sites, transactional reload, original leaks, callbacks and owned destruction");
}
