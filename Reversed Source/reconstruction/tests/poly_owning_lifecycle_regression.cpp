#include <string>
#include <vector>
#include <list>
#include <map>
#include <new>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <limits>
#define private public
#define protected public
#include "../../game/src/skill.h"
#undef protected
#undef private
#include "../../libpoly/Constants.h"
static void* tracked[2048];static unsigned released[2048],count;static bool observing;
void* operator new(std::size_t n) throw(std::bad_alloc){void* p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();return p;}
void operator delete(void* p) throw(){if(observing&&p)for(unsigned i=0;i<count;++i)if(tracked[i]==p){assert(!released[i]);++released[i];break;}std::free(p);}
static void Track(void* p){assert(count<2048);for(unsigned i=0;i<count;++i)assert(tracked[i]!=p);tracked[count]=p;released[count++]=0;}
static void BeginTrack(){count=0;observing=true;}
static void AllReleased(){for(unsigned i=0;i<count;++i)assert(released[i]==1);observing=false;}
static bool Close(double a,double b){return std::fabs(a-b)<1e-10;}
static void TrackSymbols(CPoly& p){for(unsigned i=0;i<p.lSymbol.size();++i)Track(p.lSymbol[i]);}
struct ReentrantSymbol:CSymTable {
 CPoly* owner;int index;bool shrink;bool* destroyed;
 ReentrantSymbol(CPoly* p,int i,bool s,bool* d):CSymTable(ID,"observed"),owner(p),index(i),shrink(s),destroyed(d){}
 virtual ~ReentrantSymbol(){assert(!*destroyed);*destroyed=true;if(shrink){assert(owner->tokenBase.empty()&&owner->numBase.empty());assert(owner->lSymbol[index]==this);owner->STSize=1;}}
};
static void Proto(CSkillProto* s){
 CPoly* fields[]={&s->kPointPoly,&s->kSPCostPoly,&s->kDurationPoly,&s->kDurationSPCostPoly,&s->kCooldownPoly,&s->kMasterBonusPoly,&s->kSplashAroundDamageAdjustPoly,&s->kPointPoly2,&s->kDurationPoly2,&s->kPointPoly3,&s->kDurationPoly3,&s->kGrandMasterAddSPCostPoly};
 Track(s);for(unsigned i=0;i<12;++i){assert(fields[i]->ErrorOccur&&fields[i]->STSize==25);TrackSymbols(*fields[i]);}
}
int main(){
 union Storage{long double align;unsigned char bytes[sizeof(CPoly)];} storage;std::memset(storage.bytes,0xa5,sizeof storage.bytes);CPoly* placed=new(storage.bytes) CPoly;
 const unsigned char* untouched[]={reinterpret_cast<unsigned char*>(&placed->iToken),reinterpret_cast<unsigned char*>(&placed->iNumToken),reinterpret_cast<unsigned char*>(&placed->iLookAhead),reinterpret_cast<unsigned char*>(&placed->iErrorPos)};const unsigned sizes[]={sizeof(int),sizeof(double),sizeof(int),sizeof(int)};
 for(unsigned i=0;i<4;++i)for(unsigned j=0;j<sizes[i];++j)assert(untouched[i][j]==0xa5);
 assert(placed->ErrorOccur&&placed->uiLookPos==0&&placed->STSize==25&&placed->lSymbol.capacity()>=50&&placed->strData.empty());
 const char* names[]={"min","max","number","irandom","irand","frandom","frand","rt","sqrt","cos","sin","tan","cot","csc","cosec","sec","pi","e","log","ln","log10","abs","mod","floor","sign"};const int tokens[]={22,23,24,24,24,25,25,1,1,12,13,14,17,15,15,16,10,10,18,19,20,21,26,27,28};
 for(int i=0;i<25;++i){assert(placed->lSymbol[i]->strlex==names[i]&&placed->lSymbol[i]->token==tokens[i]&&placed->lSymbol[i]->dVal==0);assert(placed->find(names[i])==i);}
 for(int i=1;i<25;++i)assert(placed->lSymbol[placed->SymbolIndex[i-1]]->strlex<placed->lSymbol[placed->SymbolIndex[i]]->strlex);
 assert(placed->SetVar("pi",3.0)==0&&placed->lSymbol[placed->find("pi")]->dVal==0&&placed->lSymbol[placed->find("e")]->dVal==0);
 // Error check precedes unsafe lookup; no actual Analyze implementation or fake.
 placed->STSize=2147483647;assert(!placed->SetVar("missing",1));placed->STSize=25;
 placed->ErrorOccur=false;assert(placed->SetVar("pi",3.0)==1&&placed->lSymbol[placed->find("pi")]->dVal==3);assert(!placed->SetVar("missing",1));
 placed->iToken=11;placed->iNumToken=1.25;placed->iLookAhead=7;placed->iErrorPos=99;placed->uiLookPos=88;placed->strData="persistent";placed->tokenBase.push_back(9);placed->numBase.push_back(5);
 const std::size_t reserved=placed->lSymbol.capacity();BeginTrack();TrackSymbols(*placed);placed->Clear();AllReleased();assert(placed->lSymbol.capacity()==reserved);assert(placed->tokenBase.empty()&&placed->numBase.empty()&&placed->lSymbol.empty()&&placed->SymbolIndex.empty()&&placed->STSize==0);
 assert(!placed->ErrorOccur&&placed->iToken==11&&placed->iNumToken==1.25&&placed->iLookAhead==7&&placed->iErrorPos==99&&placed->uiLookPos==88&&placed->strData=="persistent");assert(placed->find("pi")==-1);placed->Clear();
 // init sees CURRENT false error state, so its real SetVar(pi/e) calls succeed.
 placed->init();assert(placed->STSize==25&&Close(placed->lSymbol[placed->find("pi")]->dVal,3.141592653589793)&&Close(placed->lSymbol[placed->find("e")]->dVal,2.718281828459045));
 const unsigned char piBits[]={0x18,0x2d,0x44,0x54,0xfb,0x21,0x09,0x40};const unsigned char eBits[]={0x69,0x57,0x14,0x8b,0x0a,0xbf,0x05,0x40};
 assert(!std::memcmp(&placed->lSymbol[placed->find("pi")]->dVal,piBits,8)&&!std::memcmp(&placed->lSymbol[placed->find("e")]->dVal,eBits,8));assert(placed->lSymbol.capacity()==reserved);
 const int a=placed->insert("duplicate",10),b=placed->insert("duplicate",10);assert(a==25&&b==26&&placed->STSize==27&&placed->lSymbol[a]!=placed->lSymbol[b]);
 placed->insert("",-17);placed->insert(std::string("a\0b",3),123);assert(placed->find("")==27&&placed->find(std::string("a\0b",3))==28);assert(placed->SetVar(std::string("a\0b",3),-5.5)==1&&placed->lSymbol[28]->dVal==-5.5);
 placed->insert(std::string(1,(char)0x80),10);placed->insert(std::string(1,(char)0xff),10);placed->insert(std::string(1,(char)0x7f),10);assert(placed->find(std::string(1,(char)0x80))==29&&placed->find(std::string(1,(char)0xff))==30&&placed->find(std::string(1,(char)0x7f))==31);
 for(int i=1;i<placed->STSize;++i)assert(placed->lSymbol[placed->SymbolIndex[i-1]]->strlex<=placed->lSymbol[placed->SymbolIndex[i]]->strlex);
 placed->init();assert(placed->STSize==57); // init appends complete25 even when already populated.
 BeginTrack();TrackSymbols(*placed);placed->~CPoly();AllReleased();
 {CSymTable symbol(-19,std::string("a\0b",3));assert(symbol.dVal==0&&symbol.token==-19&&symbol.strlex==std::string("a\0b",3));}
 {CPoly p;p.Clear();bool first=false,second=false;ReentrantSymbol* a=new ReentrantSymbol(&p,0,true,&first);ReentrantSymbol* b=new ReentrantSymbol(&p,1,false,&second);p.lSymbol.push_back(a);p.lSymbol.push_back(b);p.SymbolIndex.push_back(0);p.SymbolIndex.push_back(1);p.STSize=2;p.tokenBase.push_back(9);p.numBase.push_back(1);p.Clear();assert(first&&!second&&p.STSize==0&&p.lSymbol.empty());delete b;assert(second);} // Explicit cleanup of original live-bound skipped pointer.
 BeginTrack();{CSkillManager manager;CSkillProto* a=new CSkillProto;CSkillProto* b=new CSkillProto;Proto(a);Proto(b);manager.m_map_pkSkillProto[0]=a;manager.m_map_pkSkillProto[0xffffffffUL]=b;assert(manager.Get(0UL)==a&&manager.Get(0xffffffffUL)==b&&!manager.Get(9UL));}AllReleased();assert(!CSkillManager::instance_ptr()&&count==602);
 std::puts("real poly owning lifecycle and skill prototype destruction passed; parser/eval APIs deliberately uncalled");
}
