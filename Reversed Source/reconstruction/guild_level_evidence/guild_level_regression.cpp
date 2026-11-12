// Full-class integration: actual guild/member/CHARACTER and CLIENT_DESC/DESC paths.
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/guild.h"
#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/desc_client.h"
#include "../../game/src/packet.h"
#include "../../game/src/config.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <climits>
#include <new>
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
static void* ObserveCopy(void*,const void*,size_t);
static const DWORD Key=0x12345678UL,StoredPid=0x87654321UL,Offer=0xfedcba98UL;
class ClientProbe:public CLIENT_DESC {public:void Output(LPBUFFER b){m_lpOutputBuffer=b;} void TestPhase(int phase){m_iPhase=phase;}};
struct Fixture {
    quest::CQuestManager quests;
    CGuild guild;
    CHARACTER source,a,b,offline;
    ClientProbe database;
    DescOutputProbe first,second;
    LPBUFFER dbOut,firstOut,secondOut;
    LPFDWATCH watch;
    Fixture():guild(0x89abcdefUL) {
        dbOut=buffer_new(0,8192);firstOut=buffer_new(0,8192);secondOut=buffer_new(0,8192);
        watch=fdwatch_new(16);database.Output(dbOut);
        first.Attach(watch,5,firstOut);first.SetPacketState(5);
        second.Attach(watch,6,secondOut);second.SetPacketState(5);
        a.BindDesc(&first);b.BindDesc(&second);
        a.m_dwPlayerID=42;b.m_dwPlayerID=84;offline.m_dwPlayerID=126;
        source.m_dwPlayerID=0xdeadbeefUL;source.m_points.job=6;
        source.m_points.level=255;source.m_vid=VID(111,222);source.m_stName="guild-source";
        char name[]="stored-name";
        TGuildMember member(StoredPid,225,167,242,11,Offer,name);
        member._dummy=0xa5;guild.m_member.insert(std::make_pair(Key,member));
        Roster(true);
        thecore_memcpy=ObserveCopy;
    }
    DescOutputProbe& Desc(unsigned i){return i==0?first:second;}
    LPBUFFER Output(unsigned i){return i==0?firstOut:secondOut;}
    int Index(CHARACTER* ch){if(ch==&a)return 0;if(ch==&b)return 1;assert(!"unexpected online descriptor");return -1;}
    void Roster(bool online) {
        guild.m_memberOnline.clear();
        if(online){guild.m_memberOnline.insert(&b);guild.m_memberOnline.insert(&offline);guild.m_memberOnline.insert(&a);}
    }
    ~Fixture();
};
struct Context {
    bool active,mutate,mutated,present;
    unsigned dbParts,logs,headerOrder,wireOrder;
    Fixture* f;TGuildMember* member;BYTE requested,oldGrade;
    DWORD oldOffer;
    std::vector<BYTE> db,memberPrefix,wire[2],pending[2];
    std::vector<int> recipients,openRecipients;
    bool headerSeen[2],prefixSeen[2],wireSeen[2],isOnline[2];
    Context():active(false),mutate(false),mutated(false),present(false),dbParts(0),logs(0),
        headerOrder(0),wireOrder(0),f(0),member(0),requested(0),oldGrade(0),oldOffer(0) {
        for(unsigned i=0;i<2;++i)headerSeen[i]=prefixSeen[i]=wireSeen[i]=isOnline[i]=false;
    }
} static obs;
static void Put4(std::vector<BYTE>& b,unsigned at,DWORD v){for(unsigned i=0;i<4;++i)b[at+i]=static_cast<BYTE>(v>>(8*i));}
static std::vector<BYTE> DbFrame(DWORD guild,DWORD key,DWORD offer,BYTE level,BYTE grade) {
    std::vector<BYTE> b(23,0);b[0]=24;Put4(b,5,14);Put4(b,9,guild);Put4(b,13,key);
    Put4(b,17,offer);b[21]=level;b[22]=grade;return b;
}
static std::vector<BYTE> Prefix(const TGuildMember& m,BYTE level,bool mutation) {
    std::vector<BYTE> b(13,0);Put4(b,0,m.pid);b[4]=mutation?7:m.grade;
    b[5]=m.is_general;b[6]=m.job;b[7]=level;Put4(b,8,mutation?0x01020304UL:m.offer_exp);b[12]=0;return b;
}
static std::vector<BYTE> GuildHeader(){const BYTE bytes[]={75,17,0,2};return std::vector<BYTE>(bytes,bytes+4);}
static std::vector<BYTE> Bytes(LPBUFFER b) {
    if(!b)return std::vector<BYTE>();
    const BYTE* begin=static_cast<const BYTE*>(buffer_read_peek(b));return std::vector<BYTE>(begin,begin+buffer_size(b));
}
extern "C" void guild_level_observe_db_log(int level,const char* format,unsigned header,unsigned long handle,unsigned long size,unsigned long buffered) {
    assert(obs.active&&obs.present&&obs.member&&obs.logs==0&&obs.dbParts==0);
    assert(level==1&&!std::strcmp(format,"DB_PACKET: header %d handle %d size %d buffer_size %d"));
    assert(header==24&&handle==0&&size==14&&buffered==0);
    assert(obs.member->level==obs.requested&&obs.member->_dummy==0xa5);
    ++obs.logs;
}
static void* ObserveCopy(void* out,const void* in,size_t bytes) {
    if(!obs.active)return std::memcpy(out,in,bytes);
    Fixture& f=*obs.f;
    if(out==f.dbOut->write_point) {
        const unsigned offsets[]={0,1,5,9},lengths[]={1,4,4,14};
        assert(obs.present&&obs.logs==1&&obs.dbParts<4);
        assert(bytes==lengths[obs.dbParts]&&!std::memcmp(in,&obs.db[offsets[obs.dbParts]],bytes));
        assert(obs.member->level==obs.requested&&obs.member->_dummy==0xa5);
        ++obs.dbParts;
    } else for(unsigned i=0;i<2;++i) {
        LPBUFFER buffered=f.Desc(i).BufferedOutput();
        if(buffered&&out==buffered->write_point) {
            assert(obs.present&&obs.dbParts==4&&obs.member->_dummy==0);
            if(bytes==4) {
                assert(!obs.headerSeen[i]&&obs.headerOrder<obs.recipients.size());
                assert(obs.recipients[obs.headerOrder]==static_cast<int>(i));
                const std::vector<BYTE> header=GuildHeader();assert(!std::memcmp(in,&header[0],4));
                obs.headerSeen[i]=true;++obs.headerOrder;
                // Mutate LIVE mapped fields only; never erase maps/sets/current iterators.
                if(obs.mutate&&!obs.mutated){obs.member->grade=7;obs.member->offer_exp=0x01020304UL;obs.mutated=true;}
            } else {
                assert(bytes==13&&obs.headerSeen[i]&&!obs.prefixSeen[i]);
                assert(in==obs.member&&!std::memcmp(in,&obs.memberPrefix[0],13));
                obs.prefixSeen[i]=true;
            }
        }
        if(out==f.Output(i)->write_point) {
            assert(obs.present&&obs.prefixSeen[i]&&!obs.wireSeen[i]);
            assert(obs.wireOrder<obs.openRecipients.size()&&obs.openRecipients[obs.wireOrder]==static_cast<int>(i));
            assert(bytes==obs.wire[i].size()&&!std::memcmp(in,&obs.wire[i][0],bytes));
            obs.wireSeen[i]=true;++obs.wireOrder;
        }
    }
    return std::memcpy(out,in,bytes);
}
Fixture::~Fixture() {
    obs.active=false;thecore_memcpy=std::memcpy;db_clientdesc=0;
    guild.m_memberOnline.clear();a.BindDesc(0);b.BindDesc(0);
    first.ReleaseBuffered();second.ReleaseBuffered();first.Attach(0,-1,0);second.Attach(0,-1,0);
    database.Output(0);buffer_delete(dbOut);buffer_delete(firstOut);buffer_delete(secondOut);fdwatch_delete(watch);
}
static void EqualBytes(LPBUFFER b,const std::vector<BYTE>& expected) {
    assert(buffer_size(b)==expected.size());
    if(!expected.empty())assert(!std::memcmp(buffer_read_peek(b),&expected[0],expected.size()));
}
static void Run(Fixture& f,DWORD key,BYTE level,bool mutation=false) {
    obs.active=false;obs=Context();obs.f=&f;obs.requested=level;obs.mutate=mutation;
    buffer_reset(f.dbOut);buffer_reset(f.firstOut);buffer_reset(f.secondOut);
    const size_t count=f.guild.m_member.size();const std::set<CHARACTER*> roster=f.guild.m_memberOnline;
    obs.member=f.guild.GetMember(key);obs.present=obs.member!=0;
    if(obs.present) {
        obs.member->_dummy=0xa5;obs.oldGrade=obs.member->grade;obs.oldOffer=obs.member->offer_exp;
        obs.db=DbFrame(f.guild.GetID(),key,obs.oldOffer,level,obs.oldGrade);
        obs.memberPrefix=Prefix(*obs.member,level,mutation);
        for(std::set<CHARACTER*>::iterator it=f.guild.m_memberOnline.begin();it!=f.guild.m_memberOnline.end();++it) {
            if(!(*it)->GetDesc())continue;
            const int i=f.Index(*it);obs.recipients.push_back(i);obs.isOnline[i]=true;
            if(f.Desc(i).PacketPhase()!=0)obs.openRecipients.push_back(i);
        }
    }
    for(unsigned i=0;i<2;++i) {
        obs.pending[i]=Bytes(f.Desc(i).BufferedOutput());obs.wire[i]=obs.pending[i];
        std::vector<BYTE> header=GuildHeader();obs.wire[i].insert(obs.wire[i].end(),header.begin(),header.end());
        obs.wire[i].insert(obs.wire[i].end(),obs.memberPrefix.begin(),obs.memberPrefix.end());
    }
    db_clientdesc=obs.present?&f.database:0;obs.active=true;
    f.guild.LevelChange(key,level);obs.active=false;
    assert(f.guild.m_member.size()==count&&f.guild.m_memberOnline==roster);
    if(obs.present) {
        assert(obs.logs==1&&obs.dbParts==4&&obs.member->level==level&&obs.member->_dummy==0);
        assert(obs.member->grade==(mutation?7:obs.oldGrade));
        assert(obs.member->offer_exp==(mutation?0x01020304UL:obs.oldOffer));
        assert(!mutation||obs.recipients.empty()||obs.mutated);
        EqualBytes(f.dbOut,obs.db);
    } else {assert(!obs.logs&&!obs.dbParts);EqualBytes(f.dbOut,std::vector<BYTE>());}
    assert(obs.headerOrder==obs.recipients.size()&&obs.wireOrder==obs.openRecipients.size());
    for(unsigned i=0;i<2;++i) {
        assert(obs.headerSeen[i]==obs.isOnline[i]);
        if(obs.isOnline[i]&&f.Desc(i).PacketPhase()!=0) {
            assert(obs.prefixSeen[i]&&obs.wireSeen[i]&&!f.Desc(i).BufferedOutput());
            EqualBytes(f.Output(i),obs.wire[i]);
        } else {
            assert(!obs.prefixSeen[i]&&!obs.wireSeen[i]);EqualBytes(f.Output(i),std::vector<BYTE>());
            std::vector<BYTE> pending=obs.pending[i];
            if(obs.isOnline[i]){std::vector<BYTE> h=GuildHeader();pending.insert(pending.end(),h.begin(),h.end());}
            assert(Bytes(f.Desc(i).BufferedOutput())==pending);
        }
        f.Desc(i).ReleaseBuffered();
    }
}
static void CheckConstructors(Fixture& f) {
    union Storage {long double alignment;BYTE raw[sizeof(SGuildMember)];} storage;
    std::memset(storage.raw,0xa5,sizeof(storage.raw));
    SGuildMember* first=new(storage.raw) SGuildMember(&f.source,225,Offer);
    assert(first->pid==0xdeadbeefUL&&first->grade==225&&first->is_general==0);
    assert(first->job==2&&first->level==255&&first->offer_exp==Offer&&first->name=="guild-source");
    // Inspect object representation, not the indeterminate _dummy scalar.
    assert(storage.raw[12]==0xa5);
    f.source.m_stName="changed-source";assert(first->name=="guild-source");
    first->~SGuildMember();
    std::memset(storage.raw,0x5a,sizeof(storage.raw));
    char name[]="copied-name";
    SGuildMember* second=new(storage.raw) SGuildMember(StoredPid,225,167,242,255,Offer,name);
    assert(second->pid==StoredPid&&second->grade==225&&second->is_general==167&&second->job==242);
    assert(second->level==255&&second->offer_exp==Offer&&second->name=="copied-name"&&storage.raw[12]==0x5a);
    name[0]='X';assert(second->name=="copied-name");second->~SGuildMember();
    assert(f.source.GetLevel()==255&&f.source.GetPlayerID()==0xdeadbeefUL);
}
int main() {
    assert(sizeof(DWORD)==4&&sizeof(BYTE)==1&&sizeof(WORD)==2);
    assert(sizeof(TPacketGuildChangeMemberData)==14&&sizeof(TPacketGCGuild)==4);
    int (CHARACTER::*levelSignature)()=&CHARACTER::GetLevel;
    DWORD (CHARACTER::*pidSignature)()=&CHARACTER::GetPlayerID;
    DWORD (CGuild::*idSignature)()=&CGuild::GetID;
    (void)levelSignature;(void)pidSignature;(void)idSignature;
    desc_test_allow_clock();test_server=0;Fixture f;CheckConstructors(f);
    TGuildMember* member=f.guild.GetMember(Key);assert(member&&member==&f.guild.m_member.find(Key)->second);
    const size_t count=f.guild.m_member.size();assert(!f.guild.GetMember(0xffffffffUL)&&f.guild.m_member.size()==count);
    member->level=202;member->_dummy=0xa5;const std::string name=member->name;
    Run(f,0xffffffffUL,77);assert(member->level==202&&member->_dummy==0xa5&&member->name==name);

    // A member with no online recipients still persists and clears _dummy after DB.
    f.Roster(false);Run(f,Key,255);
    const BYTE goldenDb[]={0x18,0,0,0,0,0x0e,0,0,0,0xef,0xcd,0xab,0x89,
        0x78,0x56,0x34,0x12,0x98,0xba,0xdc,0xfe,0xff,0xe1};
    assert(buffer_size(f.dbOut)==sizeof(goldenDb)&&!std::memcmp(buffer_read_peek(f.dbOut),goldenDb,sizeof(goldenDb)));
    f.Roster(true);Run(f,Key,255);
    const BYTE goldenClient[]={0x4b,0x11,0,2,0x21,0x43,0x65,0x87,0xe1,0xa7,0xf2,0xff,0x98,0xba,0xdc,0xfe,0};
    assert(buffer_size(f.firstOut)==sizeof(goldenClient)&&!std::memcmp(buffer_read_peek(f.firstOut),goldenClient,sizeof(goldenClient)));
    for(unsigned level=0;level<256;++level){Run(f,Key,static_cast<BYTE>(level));Run(f,Key,static_cast<BYTE>(level));}
    assert(member->pid==StoredPid&&member->is_general==167&&member->job==242&&member->name==name);
    // Mutate grade/offer at first real BufferedPacket header copy, before live prefix13 read.
    Run(f,Key,37,true);assert(member->grade==7&&member->offer_exp==0x01020304UL);
    member->grade=225;member->offer_exp=Offer;
    const BYTE pending[]={0x99,0xaa};f.first.BufferedPacket(pending,2);Run(f,Key,44);
    assert(buffer_size(f.firstOut)==19&&static_cast<const BYTE*>(buffer_read_peek(f.firstOut))[0]==0x99);

    // A nonnull but closed DESC buffers header4; real Packet returns without prefix13/output.
    f.guild.m_memberOnline.clear();f.guild.m_memberOnline.insert(&f.a);
    f.first.SetPacketState(0);Run(f,Key,55);f.first.SetPacketState(5);
    f.guild.m_memberOnline.clear();f.guild.m_memberOnline.insert(&f.offline);Run(f,Key,66);
    // DBPacket itself is a direct-buffer API, independent of normal DESC phase.
    f.database.TestPhase(0);f.Roster(false);Run(f,Key,66);
    f.guild.m_memberOnline.clear();db_clientdesc=0;
    std::puts("guild level passed: real constructors/map lookup, DB23-before-dummy, live member prefix/client17, all levels and same-level resend");
}
