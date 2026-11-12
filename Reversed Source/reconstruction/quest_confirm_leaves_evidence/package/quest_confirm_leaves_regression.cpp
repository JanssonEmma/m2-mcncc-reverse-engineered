// Actual full CHAR/DESC/CQM/script/PC/event graph; no confirmation dispatcher/timer substitute.
#include <map>
#include <set>
#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/questmanager.h"
#include "../../game/src/char.h"
#undef private
#undef protected
#include "../../game/src/packet.h"
#include "../../game/src/config.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static LPBUFFER outA,outB;static bool active;static int mode,copies,destination;static CHARACTER* sender;static DESC* otherDesc;static std::string* liveInput;static std::vector<BYTE> frame;
static unsigned Read2(const BYTE* b){return static_cast<unsigned>(b[0])|(static_cast<unsigned>(b[1])<<8);}
static unsigned Read4(const BYTE* b){return static_cast<unsigned>(b[0])|(static_cast<unsigned>(b[1])<<8)|(static_cast<unsigned>(b[2])<<16)|(static_cast<unsigned>(b[3])<<24);}
static void* Copy(void* output,const void* input,size_t count){
 if(active&&(output==outA->write_point||output==outB->write_point)){
  assert(!copies);const BYTE* bytes=static_cast<const BYTE*>(input);frame.assign(bytes,bytes+count);++copies;destination=output==outA->write_point?0:1;void* result=std::memcpy(output,input,count);
  if(mode==1){assert(liveInput&&sender&&otherDesc);liveInput->assign("changed-after-copy");sender->BindDesc(otherDesc);}
  return result;
 }
 return std::memcpy(output,input,count);
}
static void Begin(){active=false;mode=copies=0;destination=-1;frame.clear();buffer_reset(outA);buffer_reset(outB);liveInput=0;test_server=0;}
static void Packet(const std::string& message,int timeout,DWORD requestPID,int dest){
 assert(copies==1&&destination==dest&&frame.size()==74&&frame[0]==46&&frame[65]==0&&Read4(&frame[66])==static_cast<unsigned>(timeout)&&Read4(&frame[70])==requestPID);
 const size_t prefix=std::strlen(message.c_str());for(unsigned i=0;i<64;++i)assert(frame[1+i]==(i<prefix?static_cast<BYTE>(message[i]):0));
 assert(buffer_size(dest?outB:outA)==74&&!buffer_size(dest?outA:outB));
}
int main(){
 desc_test_allow_clock();assert(sizeof(long)==4&&sizeof(DWORD)==4&&sizeof(TPacketGCQuestConfirm)==74);quest::CQuestManager qm;CHARACTER character;sender=&character;DescOutputProbe first,second;otherDesc=&second;LPFDWATCH watch=fdwatch_new(16);outA=buffer_new(0,32768);outB=buffer_new(0,32768);first.Attach(watch,5,outA);first.SetPacketState(5);second.Attach(watch,6,outB);second.SetPacketState(5);thecore_memcpy=Copy;
 // Guard precedes ALLmessage use: NULL input is safe when no descriptor is attached.
 Begin();character.BindDesc(0);active=true;character.ConfirmWithMsg(0,INT_MIN,0xffffffffUL);active=false;assert(!copies&&frame.empty()&&!buffer_size(outA)&&!buffer_size(outB));
 character.m_bCharType=0;assert(character.GetCharType()==0);character.BindDesc(&first); // NPC/monster type with validDESC muststillreceive.
 const std::string messages[]={"",std::string(1,'a'),std::string(63,'b'),std::string(64,'c'),std::string(65,'d'),std::string(100,'e'),std::string("left\0right",10)};const int timeouts[]={INT_MIN,-1,0,1,INT_MAX};const DWORD ids[]={0,1,0x7fffffffUL,0x80000000UL,0xffffffffUL};
 for(unsigned m=0;m<7;++m)for(unsigned t=0;t<5;++t)for(unsigned p=0;p<5;++p){Begin();active=true;character.ConfirmWithMsg(messages[m].c_str(),timeouts[t],ids[p]);active=false;Packet(messages[m],timeouts[t],ids[p],0);assert(character.GetDesc()==&first);}
 // Exactly64 accessible non-NUL bytes are a VALIDstrncpy input, no strlen/read-beyond allowed in production.
 char raw[64];std::memset(raw,'R',sizeof(raw));Begin();active=true;character.ConfirmWithMsg(raw,-17,0xaabbccddUL);active=false;assert(frame.size()==74&&frame[0]==46&&frame[65]==0&&Read4(&frame[66])==static_cast<unsigned>(-17)&&Read4(&frame[70])==0xaabbccddUL);for(unsigned i=1;i<=64;++i)assert(frame[i]=='R');
 // Payload alreadyownedstackpacket beforeactualDESCcopy callback; receiver remains original for thiscall.
 Begin();std::string changing="original";liveInput=&changing;mode=1;active=true;character.ConfirmWithMsg(changing.c_str(),-23,0x80000000UL);active=false;Packet("original",-23,0x80000000UL,0);assert(changing=="changed-after-copy"&&character.GetDesc()==&second);
 Begin();active=true;character.ConfirmWithMsg(changing.c_str(),29,31);active=false;Packet(changing,29,31,1); // NEXTinvocationusesfreshdescriptor.
 // SetSkinStyle is unsigned0..5 selection; negatives and everyotherinteger fallback1, no otherownerwrites.
 const int styles[]={INT_MIN,-100,-1,0,1,2,3,4,5,6,7,65535,INT_MAX};
 for(unsigned i=0;i<sizeof(styles)/sizeof(styles[0]);++i){int style=styles[i];int expected=style>=0&&style<=5?style:1;qm.m_iCurrentSkin=-777;qm.m_strScript=std::string("s\0x",3);qm.m_bNoSend=true;qm.m_bError=true;qm.m_pCurrentCharacter=0;qm.m_pCurrentPC=0;qm.SetSkinStyle(style);assert(qm.m_iCurrentSkin==expected&&qm.m_strScript==std::string("s\0x",3)&&qm.m_bNoSend&&qm.m_bError&&!qm.m_pCurrentCharacter&&!qm.m_pCurrentPC&&!qm.L);
  // Observe selectedstyle through complete actualSendScript/network, not a test-only serializer.
  Begin();qm.m_strScript="style-check";qm.m_bNoSend=false;qm.m_bError=false;qm.m_pCurrentCharacter=&character;active=true;qm.SendScript();active=false;assert(copies==1&&destination==1&&frame.size()==17&&frame[0]==45&&Read2(&frame[1])==17&&frame[3]==expected&&Read2(&frame[4])==11&&!std::memcmp(&frame[6],"style-check",11));assert(qm.m_strScript.empty()&&qm.m_iCurrentSkin==1);
 }
 active=false;thecore_memcpy=std::memcpy;character.BindDesc(0);first.Attach(0,-1,0);second.Attach(0,-1,0);buffer_delete(outA);buffer_delete(outB);fdwatch_delete(watch);qm.m_pCurrentCharacter=0;qm.m_pCurrentPC=0;test_server=0;std::puts("whole CHARConfirmWithMsg/CQMSetSkinStyle actual network/script passed; Confirm/RunState cycle remains unintegrated");return 0;
}
