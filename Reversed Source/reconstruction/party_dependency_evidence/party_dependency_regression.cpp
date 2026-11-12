#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/party.h"
#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
#include "../../game/src/sectree.h"
#undef protected
#undef private
#include "../../game/src/packet.h"
#include "../../common/packet.h"
#include "../../game/src/desc_client.h"
#include "../../game/src/locale.hpp"
#include "../../game/src/utils.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError() { std::abort(); }
extern "C" { void desc_test_allow_clock(void);extern int party_test_clock_reads; }
struct Observer {
    bool active;unsigned sent;int buildLogs;
    std::vector<std::string> steps;
    std::vector<DWORD> recipients;
    std::vector<LPBUFFER> outputs;
    std::vector<BYTE> expected;
} static obs;
static DWORD Read4(const BYTE* b) { return static_cast<DWORD>(b[0])|static_cast<DWORD>(b[1])<<8|static_cast<DWORD>(b[2])<<16|static_cast<DWORD>(b[3])<<24; }
static void Put4(std::vector<BYTE>& b,unsigned at,DWORD value) { for(unsigned i=0;i<4;++i)b[at+i]=static_cast<BYTE>(value>>(8*i)); }
static void Put2(std::vector<BYTE>& b,unsigned at,short value) { unsigned short raw=static_cast<unsigned short>(value);b[at]=static_cast<BYTE>(raw);b[at+1]=static_cast<BYTE>(raw>>8); }
extern "C" void party_test_log(int level,const char* format,const char* text) {
    if(!obs.active)return;
    if(std::strcmp(format,"PARTY %s role is %d")==0) { assert(level==1&&obs.steps.empty());++obs.buildLogs;obs.steps.push_back("build"); }
    else if(std::strcmp(format,"PARTY send info %s[%d] to %s[%d]")==0) {
        assert(level==2&&obs.sent<obs.recipients.size());char expected[128];
        std::snprintf(expected,sizeof(expected),"PARTY send info source[1000] to %s[%lu]",obs.recipients[obs.sent]==1000?"source":"member",obs.recipients[obs.sent]);
        assert(text==std::string(expected));obs.steps.push_back("log");
    }
}
static void* Copy(void* out,const void* in,size_t bytes) {
    if(obs.active&&bytes==21&&static_cast<const BYTE*>(in)[0]==79) {
        assert(obs.sent<obs.outputs.size());assert(out==obs.outputs[obs.sent]->write_point);
        assert(!obs.steps.empty()&&obs.steps.back()=="log");
        assert(obs.expected.size()==21&&std::memcmp(in,&obs.expected[0],21)==0);
        obs.steps.push_back("packet");++obs.sent;
    }
    return std::memcpy(out,in,bytes);
}
class ClientProbe : public CLIENT_DESC { public:void Output(LPBUFFER b){m_lpOutputBuffer=b;} };
static void Bind(CHARACTER& ch,DescOutputProbe& d,LPFDWATCH watch,int fd,LPBUFFER output,DWORD vid) {
    d.Attach(watch,fd,output);d.SetPacketState(5);ch.BindDesc(&d);ch.m_vid=VID(vid,55);ch.m_stName="member";
}
static void Clear(CParty& p) {
    p.m_memberMap.clear();p.m_map_iFlag.clear();p.m_pkChrLeader=0;p.m_pkChrExpCentralize=0;p.m_pkDungeon=0;p.m_bPCParty=false;assert(!p.m_eventUpdate);
}
static void Member(CParty& p,DWORD pid,CHARACTER* ch,BYTE role) {
    CParty::TMember member;member.pCharacter=ch;member.bNear=false;member.bRole=role;member.bLevel=1;member.strName="fixture";p.m_memberMap[pid]=member;
}
static void ResetObserver(LPBUFFER a,LPBUFFER b,const std::vector<BYTE>& packet,bool active=true) {
    buffer_reset(a);buffer_reset(b);obs.steps.clear();obs.recipients.clear();obs.outputs.clear();obs.expected=packet;obs.sent=0;obs.buildLogs=0;obs.active=active;
}
static void FinishObserver(int builds) {
    obs.active=false;assert(obs.buildLogs==builds&&obs.sent==obs.recipients.size());
    std::vector<std::string> expected;if(builds)expected.push_back("build");
    for(unsigned i=0;i<obs.sent;++i){expected.push_back("log");expected.push_back("packet");assert(buffer_size(obs.outputs[i])==21&&std::memcmp(buffer_read_peek(obs.outputs[i]),&obs.expected[0],21)==0);}
    assert(obs.steps==expected);
}
static std::vector<BYTE> Expected(BYTE hp,BYTE role,bool affects,int exp) {
    std::vector<BYTE> result(21,0);result[0]=79;Put4(result,1,1234);result[5]=role;result[6]=hp;
    const int values[]={-32769,32768,-1,65535,12345,-12345};
    if(affects){Put2(result,7,static_cast<short>(exp));for(int i=0;i<6;++i)Put2(result,9+2*i,static_cast<short>(values[i]));}
    return result;
}
int main() {
    assert(sizeof(DWORD)==4&&sizeof(long)==4&&sizeof(TPacketGCPartyUpdate)==21&&sizeof(TPacketPartySetMemberLevel)==9);
    desc_test_allow_clock();test_server=0;g_iUseLocale=0;
    // Real ctor/Initialize, including two independent clock reads and untouched maps.
    {
        int before=party_test_clock_reads;CParty party;assert(party_test_clock_reads==before+2);
        assert(party.m_memberMap.empty()&&party.m_map_iFlag.empty()&&party.m_dwLeaderPID==0&&!party.m_eventUpdate);
        assert(party.m_dwPartyHealTime==party.m_dwPartyStartTime+1);
        for(int i=0;i<8;++i){assert(party.m_anRoleCount[i]==0);assert(party.m_anMaxRole[i]==(i==0?32:(i==1?1:0)));}
        Member(party,12,0,231);party.m_map_iFlag["retained"]=42;party.m_iLeadership=77;party.m_iCountNearPartyMember=8;
        before=party_test_clock_reads;party.Initialize();assert(party_test_clock_reads==before+2);
        assert(party.m_memberMap.size()==1&&party.m_map_iFlag["retained"]==42&&party.m_itNextOwner==party.m_memberMap.begin());
        assert(party.m_iLeadership==0&&party.m_iCountNearPartyMember==0&&party.m_dwPartyHealTime==party.m_dwPartyStartTime+1);
        assert(party.m_iExpDistributionMode==0&&!party.m_pkChrExpCentralize&&!party.m_pkChrLeader&&!party.m_pkDungeon&&!party.m_bPCParty);
        assert(!party.m_bPartyHealReady&&!party.m_bCanUsePartyHeal&&party.m_iLongTimeExpBonus==0&&party.m_iExpBonus==0&&party.m_iAttBonus==0&&party.m_iDefBonus==0);
        assert(party.GetRole(12)==231&&party.GetRole(999)==0&&party.m_memberMap.size()==1);
        // Exact operator[] insertion; do not replace the helper with find/null.
        party.m_dwLeaderPID=888;(void)party.GetLeaderCharacter();assert(party.m_memberMap.count(888)==1);
        Clear(party);
    }
    // Request has no membership/PC-party guard and uses actual DB wire framing.
    {
        CParty party;party.m_dwLeaderPID=0xfedcba98UL;ClientProbe client;LPBUFFER output=buffer_new(0,8192);client.Output(output);db_clientdesc=&client;
        const DWORD ids[]={0UL,1234UL,0xffffffffUL};const BYTE levels[]={0,1,255};
        for(unsigned i=0;i<3;++i){buffer_reset(output);party.RequestSetMemberLevel(ids[i],levels[i]);assert(buffer_size(output)==18);
            const BYTE* b=static_cast<const BYTE*>(buffer_read_peek(output));assert(b[0]==56&&Read4(b+1)==0&&Read4(b+5)==9);
            assert(Read4(b+9)==0xfedcba98UL&&Read4(b+13)==ids[i]&&b[17]==levels[i]);assert(party.m_memberMap.empty());}
        db_clientdesc=0;client.Output(0);buffer_delete(output);Clear(party);
    }
    quest::CQuestManager quests;
    CHARACTER source,other,offline,leader;
    source.m_dwPlayerID=1234;source.m_stName="source";source.m_vid=VID(1000,55);
    leader.m_pos.x=0;leader.m_pos.y=0;
    LPFDWATCH watch=fdwatch_new(16);LPBUFFER first=buffer_new(0,8192),second=buffer_new(0,8192);
    DescOutputProbe sourceDesc,otherDesc;
    Bind(source,sourceDesc,watch,5,first,1000);source.m_stName="source";
    Bind(other,otherDesc,watch,6,second,2000);
    const int points[]={91,92,103,104,110,111};const int values[]={-32769,32768,-1,65535,12345,-12345};
    for(int i=0;i<6;++i)source.m_pointsInstant.points[points[i]]=values[i];
    CParty party;source.m_pkParty=&party;party.m_dwLeaderPID=99;
    Member(party,1234,&source,231);Member(party,20,&offline,1);Member(party,30,0,2);Member(party,2000,&other,3);Member(party,99,&leader,1);
    thecore_memcpy=Copy;
    struct Row {int x,hp,maxHP,locale,nearCount,exp,longBonus;BYTE hpByte;bool affects;} rows[]={
        {5203,50,100,0,2,30,7,50,true}, // DISTANCE_APPROX=4999
        {5204,50,100,0,2,30,7,50,false},// DISTANCE_APPROX=5000
        {0,-10,100,0,2,30,7,0,true},
        {0,101,100,0,2,30,7,100,true},
        {0,50,100,0,1,30,7,50,true},
        {0,50,100,0,-1,30,7,50,true}, // unsigned negative near count is NOT <2
        {0,50,100,1,1,30,7,50,true},
        {0,50,100,2,2,32768,9,50,true},
        {0,2147483647,100,1,2,30,7,0,true} // target32 HP*100 wraps to-100
    };
    assert(DISTANCE_APPROX(5203,0)==4999&&DISTANCE_APPROX(5204,0)==5000);
    for(unsigned n=0;n<sizeof(rows)/sizeof(*rows);++n){
        Row& r=rows[n];source.m_pos.x=r.x;source.m_pos.y=0;source.m_points.hp=r.hp;source.m_pointsInstant.iMaxHP=r.maxHP;
        party.m_iCountNearPartyMember=r.nearCount;party.m_iExpBonus=r.exp;party.m_iLongTimeExpBonus=r.longBonus;g_iUseLocale=r.locale;
        int exp=r.locale?r.exp:(static_cast<unsigned int>(r.nearCount)<2U?0:r.exp+r.longBonus);
        std::vector<BYTE> expected=Expected(r.hpByte,231,r.affects,exp);
        // Actual builder bytes, then actual full map-order broadcast including self.
        TPacketGCPartyUpdate packet;std::memset(&packet,0xcc,sizeof(packet));assert(source.BuildUpdatePartyPacket(packet));assert(!std::memcmp(&packet,&expected[0],21));
        ResetObserver(first,second,expected);obs.recipients.push_back(1000);obs.recipients.push_back(2000);obs.outputs.push_back(first);obs.outputs.push_back(second);
        party.SendPartyInfoOneToAll(&source);FinishObserver(1);
    }
    // Source disconnected: no build, no recipient output, even with online members.
    ResetObserver(first,second,Expected(0,0,false,0));source.BindDesc(0);party.SendPartyInfoOneToAll(&source);FinishObserver(0);assert(!buffer_size(first)&&!buffer_size(second));source.BindDesc(&sourceDesc);
    // No-party builder returns false and leaves caller-initialized output unchanged.
    source.m_pkParty=0;TPacketGCPartyUpdate untouched;std::memset(&untouched,0xa5,sizeof(untouched));assert(!source.BuildUpdatePartyPacket(untouched));
    const BYTE* raw=reinterpret_cast<const BYTE*>(&untouched);for(unsigned i=0;i<sizeof(untouched);++i)assert(raw[i]==0xa5);
    // Do not invoke broadcast on that false-builder path: the original sends
    // its own indeterminate stack packet, which cannot be safely asserted.
    source.m_pkParty=&party;
    // Empty recipient map still invokes the real builder. Keep its party valid
    // and leader missing insertion controlled by adding the leader before call.
    CParty empty;source.m_pkParty=&party;ResetObserver(first,second,Expected(0,0,false,0));empty.SendPartyInfoOneToAll(&source);FinishObserver(1);Clear(empty);
    thecore_memcpy=std::memcpy;source.m_pkParty=0;Clear(party);g_iUseLocale=0;
    source.BindDesc(0);other.BindDesc(0);sourceDesc.Attach(0,-1,0);otherDesc.Attach(0,-1,0);buffer_delete(first);buffer_delete(second);fdwatch_delete(watch);event_destroy();
    std::puts("party dependency passed: real ctor/init, DB56 framing, packet79 builder/broadcast, raw roles, distance and locale branches");
}
