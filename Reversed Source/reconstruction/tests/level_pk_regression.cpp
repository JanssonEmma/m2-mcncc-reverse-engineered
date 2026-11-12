#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
#include "../../game/src/guild.h"
#undef protected
#undef private
#include "../../game/src/sectree.h"
#include "../../game/src/packet.h"
#include "../../game/src/config.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <climits>
#include <cstdlib>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static DWORD expectedGuildId;
struct Context {bool active,changed;int copies,logs,expectedCopies;BYTE mode,level;CHARACTER* ch;LPBUFFER own,other;} static ctx;
static void* Copy(void* out,const void* in,size_t bytes) {
    if(ctx.active&&bytes==35&&static_cast<const BYTE*>(in)[0]==19){
        assert(ctx.changed&&ctx.logs==0&&ctx.ch->m_bPKMode==ctx.mode&&ctx.ch->m_points.level==ctx.level);
        TPacketGCCharacterUpdate p;std::memcpy(&p,in,35);assert(p.dwVID==1234&&p.bPKMode==ctx.mode&&p.dwGuildID==expectedGuildId);++ctx.copies;
    }
    return std::memcpy(out,in,bytes);
}
extern "C" void level_pk_test_log(int level,const char* format,const char* text) {
    if(!ctx.active||std::strcmp(format,"PK_MODE: %s %d"))return;
    assert(ctx.changed&&level==0&&ctx.copies==ctx.expectedCopies&&ctx.logs==0&&ctx.ch->m_bPKMode==ctx.mode&&ctx.ch->m_points.level==ctx.level);
    char expected[64];std::snprintf(expected,sizeof(expected),"PK_MODE: subject %u",static_cast<unsigned int>(ctx.mode));assert(text==std::string(expected));++ctx.logs;
}
static void Start(CHARACTER& ch,LPBUFFER own,LPBUFFER other,BYTE level,BYTE mode,bool changed){
    buffer_reset(own);buffer_reset(other);ctx.ch=&ch;ctx.own=own;ctx.other=other;ctx.level=level;ctx.mode=mode;ctx.changed=changed;ctx.copies=ctx.logs=0;ctx.expectedCopies=changed?(ch.GetDesc()?2:1):0;ctx.active=true;
}
static void Finish(){ctx.active=false;assert(ctx.ch->m_points.level==ctx.level&&ctx.ch->m_bPKMode==ctx.mode);assert(ctx.copies==ctx.expectedCopies&&ctx.logs==(ctx.changed?1:0));assert(buffer_size(ctx.own)==(ctx.changed&&ctx.ch->GetDesc()?35UL:0UL)&&buffer_size(ctx.other)==(ctx.changed?35UL:0UL));}
int main(){
    desc_test_allow_clock();quest::CQuestManager quests;CHARACTER ch,viewer;SECTREE sector;
    assert(PK_PROTECT_LEVEL==30);BYTE oldThreshold=PK_PROTECT_LEVEL;int oldServer=test_server;
    const int servers[]={0,1,-1,INT_MIN};
    for(unsigned i=0;i<4;++i){test_server=servers[i];for(unsigned gm=0;gm<256;++gm){ch.m_pointsInstant.gm_level=static_cast<BYTE>(gm);assert(ch.GetGMLevel()==(servers[i]?5:gm));}}
    LPBUFFER own=buffer_new(0,8192),other=buffer_new(0,8192);LPFDWATCH watch=fdwatch_new(16);DescOutputProbe self,recipient;
    self.Attach(watch,5,own);self.SetPacketState(5);recipient.Attach(watch,6,other);recipient.SetPacketState(5);
    ch.BindDesc(&self);viewer.BindDesc(&recipient);ch.SetSectree(&sector);ch.m_map_view[&viewer]=1;ch.m_vid=VID(1234,55);ch.m_stName="subject";ch.m_pGuild=0;
    ch.m_pointsInstant.points[19]=100;ch.m_pointsInstant.points[17]=100;
    thecore_memcpy=Copy;const BYTE previous[]={0,1,2,3,4,255};
    // Unsigned requested-byte guard, equality before guild fallback, and raw
    // existing invalid modes. Real UpdatePacket fanout + external logging only.
    for(unsigned n=0;n<6;++n)for(unsigned requested=0;requested<256;++requested){
        BYTE old=previous[n];ch.m_bPKMode=old;ch.m_points.level=42;
        bool changed=requested<5&&old!=requested;BYTE final=changed?static_cast<BYTE>(requested==4?2:requested):old;
        Start(ch,own,other,42,final,changed);ch.SetPKMode(static_cast<BYTE>(requested));Finish();
    }
    // Use actual complete CGuild data and real GetID/update packet paths.
    CGuild guild(0xa1b2c3d4UL);ch.m_pGuild=&guild;expectedGuildId=0xa1b2c3d4UL;
    for(unsigned n=0;n<6;++n)for(unsigned requested=0;requested<256;++requested){
        BYTE old=previous[n];ch.m_bPKMode=old;ch.m_points.level=42;
        bool changed=requested<5&&old!=requested;BYTE final=changed?static_cast<BYTE>(requested):old;
        Start(ch,own,other,42,final,changed);ch.SetPKMode(static_cast<BYTE>(requested));Finish();
    }
    ch.m_pGuild=0;expectedGuildId=0;
    const BYTE thresholds[]={0,30,255},levels[]={0,1,29,30,31,254,255},gms[]={0,1,5,255};
    for(unsigned t=0;t<3;++t)for(unsigned l=0;l<7;++l)for(unsigned m=0;m<6;++m)for(unsigned g=0;g<4;++g)for(unsigned s=0;s<4;++s){
        PK_PROTECT_LEVEL=thresholds[t];test_server=servers[s];BYTE old=previous[m],level=levels[l];ch.m_bPKMode=old;ch.m_pointsInstant.gm_level=gms[g];
        bool protect=level<thresholds[t]||servers[s]!=0||gms[g]!=0;
        BYTE final=protect?3:(old==3?0:old);bool changed=final!=old;
        Start(ch,own,other,level,final,changed);ch.SetLevel(level);Finish();
    }
    // Descriptor absence suppresses all PK policy even on test_server/low level.
    ch.BindDesc(0);test_server=1;PK_PROTECT_LEVEL=255;
    for(unsigned n=0;n<6;++n){ch.m_bPKMode=previous[n];Start(ch,own,other,0,previous[n],false);ch.SetLevel(0);Finish();}
    // Direct PK mutation still fans out to viewer without a source descriptor.
    ch.m_bPKMode=0;Start(ch,own,other,0,2,true);ch.SetPKMode(2);Finish();
    ch.BindDesc(&self);
    // Explicit no-op after mode fallback and explicit redundant transition:
    ch.m_bPKMode=4;Start(ch,own,other,0,4,false);ch.SetPKMode(4);Finish();
    ch.m_bPKMode=2;Start(ch,own,other,0,2,true);ch.SetPKMode(4);Finish();
    thecore_memcpy=std::memcpy;PK_PROTECT_LEVEL=oldThreshold;test_server=oldServer;ch.m_map_view.clear();ch.SetSectree(0);ch.BindDesc(0);viewer.BindDesc(0);self.Attach(0,-1,0);recipient.Attach(0,-1,0);buffer_delete(own);buffer_delete(other);fdwatch_delete(watch);
    std::puts("level/PK passed: raw levels and GM override, mutable threshold, pre-fallback equality, real update fanout before log");
}
