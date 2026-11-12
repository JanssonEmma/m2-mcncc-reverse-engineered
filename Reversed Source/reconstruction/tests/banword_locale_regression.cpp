// Complete real class/SendLog/str_lower/name validators; full shared graph.
// Existing full lifecycle fixtures only. External packet memory and logging observed.
#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/banword.h"
#include "../../game/src/char.h"
#include "../../game/src/desc.h"
#include "../../game/src/desc_manager.h"
#include "../../game/src/mob_manager.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/config.h"
#include "../../game/src/locale_service.h"
#include "../../game/src/utils.h"
#include "../../game/src/packet.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <climits>
#include <clocale>
extern void SendLog(const char*);
#include "banword_locale_api_checks.h"
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
static bool diagExpected;
static const char* diagText;
static unsigned diagCount;
extern "C" void banword_locale_observe_name_diag(int level,int index,const char* text,
    unsigned first,unsigned second,unsigned code) {
    assert(diagExpected && level==0 && index==1 && text==diagText);
    assert(first==0x80 && second==0x80 && code==0x8080);++diagCount;
}
static struct PacketContext {
    bool active;LPBUFFER outputs[4];std::vector<int> order;
    PacketContext():active(false) { for(int i=0;i<4;++i) outputs[i]=0; }
} packet;
static void* ObserveCopy(void* to,const void* from,size_t size) {
    if(packet.active) for(int i=0;i<4;++i)
        if(to==packet.outputs[i]->write_point) packet.order.push_back(i);
    return std::memcpy(to,from,size);
}
struct Fixture {
    quest::CQuestManager quests;
    DESC_MANAGER descriptors;
    CMobManager mobs;
    CBanwordManager words;
    CMob reservedMob;
    CHARACTER characters[3];
    DescOutputProbe desc[4];
    LPBUFFER output[4];LPFDWATCH watch;
    Fixture():watch(fdwatch_new(16)) {
        assert(words.m_hashmap_words.bucket_count()==193);
        for(int i=0;i<4;++i) {
            output[i]=buffer_new(0,8192);packet.outputs[i]=output[i];
            desc[i].Attach(watch,5+i,output[i]);desc[i].SetPacketState(5);
            desc[i].m_accountTable.bEmpire=static_cast<BYTE>(i+1);
            descriptors.m_set_pkDesc.insert(&desc[i]);
            if(i<3) {
                characters[i].BindDesc(&desc[i]);desc[i].BindCharacter(&characters[i]);
                characters[i].m_stName="banword-character";
                characters[i].m_pointsInstant.gm_level=0;
                assert(desc[i].GetCharacter()==&characters[i]);
            } else assert(desc[i].GetCharacter()==0);
        }
        mobs.m_map_pkMobByName["reservedname"]=&reservedMob;
        thecore_memcpy=ObserveCopy;
    }
    ~Fixture() {
        packet.active=false;thecore_memcpy=std::memcpy;
        descriptors.m_set_pkDesc.clear();mobs.m_map_pkMobByName.clear();
        for(int i=0;i<3;++i) characters[i].BindDesc(0);
        for(int i=0;i<4;++i) {
            desc[i].BindCharacter(0);desc[i].Attach(0,-1,0);
            buffer_delete(output[i]);packet.outputs[i]=0;
        }
        fdwatch_delete(watch);
    }
    void Begin() {
        packet.active=false;packet.order.clear();
        for(int i=0;i<4;++i) buffer_reset(output[i]);
        packet.active=true;
    }
    void Finish(const char* text) {
        packet.active=false;
        std::vector<DESC*> recipients;
        for(int i=0;i<3;++i)
            if(test_server || characters[i].m_pointsInstant.gm_level) recipients.push_back(&desc[i]);
        std::sort(recipients.begin(),recipients.end(),std::less<DESC*>());
        assert(packet.order.size()==recipients.size());
        for(std::size_t i=0;i<recipients.size();++i) assert(&desc[packet.order[i]]==recipients[i]);
        const std::size_t n=std::strlen(text);
        for(int i=0;i<4;++i) {
            const bool expected=i<3 && (test_server || characters[i].m_pointsInstant.gm_level);
            assert(buffer_size(output[i])==(expected?n+10:0));
            if(expected) {
                const unsigned char* p=reinterpret_cast<const unsigned char*>(buffer_read_peek(output[i]));
                assert(p[0]==4 && (unsigned(p[1])|(unsigned(p[2])<<8))==n+10 && p[3]==2);
                assert(p[4]==0 && p[5]==0 && p[6]==0 && p[7]==0 && p[8]==i+1);
                assert(!std::memcmp(p+9,text,n+1));
            }
        }
    }
    void Initialize(TBanwordTable* table,WORD count,unsigned unique) {
        Begin();assert(words.Initialize(table,count));
        char expected[128];std::sprintf(expected,"Banword reloaded! (total %d banwords)",static_cast<int>(unique));
        Finish(expected);
    }
};
static void HashAndLower() {
    CBanwordManager::stringhash hash;
    assert(hash("")==0 && hash("ab")==1627429073UL && hash("bc")==1644206629UL);
    assert(hash("abc")==2230358368UL);
    std::string binary;for(int i=0;i<256;++i) binary+=static_cast<char>(i);
    assert(hash(binary)==4117514240UL); // Includes embeddedNUL, unsigned bytes and target32 wrap.
    const int limits[]={INT_MIN,-2,-1,0,1,2,24,256,INT_MAX};
    for(unsigned byte=0;byte<256;++byte)
        for(unsigned k=0;k<sizeof(limits)/sizeof(limits[0]);++k) {
            char src[3]={static_cast<char>(byte),'Z',0},before[3];std::memcpy(before,src,3);
            char dest[260];std::memset(dest,0x55,sizeof dest);
            str_lower(src,dest,limits[k]);
            int copied=0;
            if(byte && limits[k]>0) {
                const unsigned expected=byte>='A' && byte<='Z'?byte+32:byte;
                assert(static_cast<unsigned char>(dest[copied++])==expected);
                if(limits[k]>1) assert(dest[copied++]=='z');
            }
            assert(dest[copied]==0 && static_cast<unsigned char>(dest[copied+1])==0x55);
            assert(!std::memcmp(src,before,3));
        }
    char inplace[]="ABC123xyz";str_lower(inplace,inplace,INT_MAX);assert(!std::strcmp(inplace,"abc123xyz"));
    char overlap[8]="ABC";str_lower(overlap,overlap+1,3);assert(!std::strcmp(overlap,"Aaaa"));
}
static void SendAndInitialize(Fixture& f) {
    const char* text="literal %s remains text";
    for(unsigned gm=0;gm<256;++gm) {
        test_server=0;f.characters[0].m_pointsInstant.gm_level=static_cast<BYTE>(gm);
        f.Begin();SendLog(text);f.Finish(text);
    }
    f.characters[0].m_pointsInstant.gm_level=0;
    f.characters[1].m_pointsInstant.gm_level=1;f.characters[2].m_pointsInstant.gm_level=255;
    f.Begin();SendLog(text);f.Finish(text);
    test_server=1;f.Begin();SendLog(text);f.Finish(text);test_server=0;
    TBanwordTable table[4];std::memset(table,0,sizeof table);
    std::strcpy(table[0].szWord,"bad");std::strcpy(table[1].szWord,"evil");
    std::strcpy(table[2].szWord,"bad");std::memset(table[3].szWord,'X',24);
    TBanwordTable before[4];std::memcpy(before,table,sizeof table);
    f.Initialize(table,4,3);
    assert(!std::memcmp(table,before,sizeof table));
    assert(f.words.m_hashmap_words.size()==3 && f.words.Find("bad") && f.words.Find("evil"));
    assert(!f.words.Find("BAD") && !f.words.Find("xxbad"));
    std::string maximum(24,'X');assert(f.words.Find(maximum.c_str()));
    for(CBanwordManager::TBanwordHashmap::iterator it=f.words.m_hashmap_words.begin();it!=f.words.m_hashmap_words.end();++it)
        assert(it->second);
    f.Initialize(0,0,0);assert(f.words.m_hashmap_words.empty());
    assert(!f.words.CheckString(0));f.words.ConvertString(0); // Empty container never dereferences input/callback.
    is_twobyte=is_twobyte_euckr;
}
static void Banwords(Fixture& f) {
    f.words.m_hashmap_words.clear();f.words.m_hashmap_words["bad"]=false;
    assert(f.words.Find("bad") && f.words.CheckString("xxbadyy") && !f.words.CheckString("BAD"));
    const char unchanged[]="xxbadyy";assert(f.words.CheckString(unchanged) && !std::strcmp(unchanged,"xxbadyy"));
    f.words.m_hashmap_words["evil"]=true;
    char sentence[]="bad evil bad";f.words.ConvertString(sentence);assert(!std::strcmp(sentence,"*** **** ***"));
    f.words.m_hashmap_words.clear();f.words.m_hashmap_words["B"]=true;
    char pair[]={static_cast<char>(0xb0),'B','X',0};
    is_twobyte=is_twobyte_euckr;assert(!f.words.CheckString(pair));
    char copy[sizeof pair];std::memcpy(copy,pair,sizeof pair);f.words.ConvertString(copy);
    assert(!std::memcmp(copy,pair,sizeof pair));
    is_twobyte=is_twobyte_gb2312;assert(f.words.CheckString(pair));
    std::memcpy(copy,pair,sizeof pair);f.words.ConvertString(copy);assert(copy[1]=='*' && copy[0]==pair[0]);
    is_twobyte=is_twobyte_euckr;
    std::string pairword(pair,2);f.words.m_hashmap_words.clear();f.words.m_hashmap_words[pairword]=true;
    std::memcpy(copy,pair,sizeof pair);assert(f.words.CheckString(copy));f.words.ConvertString(copy);
    assert(copy[0]=='*' && copy[1]=='*' && copy[2]=='X' && copy[3]==0);
    f.words.m_hashmap_words.clear();f.words.m_hashmap_words["*x"]=true;
    char starred[]="*x";f.words.ConvertString(starred);assert(!std::strcmp(starred,"*x"));
    f.words.m_hashmap_words.clear();f.words.m_hashmap_words["ab"]=true;f.words.m_hashmap_words["bc"]=true;
    // Original FNV buckets: bc64 before ab191 at193 buckets; ordered-map substitution differs.
    char overlap[]="abc";f.words.ConvertString(overlap);assert(!std::strcmp(overlap,"a**"));
    f.words.m_hashmap_words.clear();f.words.m_hashmap_words[""]=true;
    assert(f.words.CheckString("x") && !f.words.CheckString(""));
    // Original ConvertString(nonempty input) with emptyword does not advance: do not execute.
    char empty[]="";f.words.ConvertString(empty);assert(empty[0]==0);
    f.words.m_hashmap_words.clear();
}
static bool AsciiAlnum(unsigned c) {return (c>='0'&&c<='9')||(c>='A'&&c<='Z')||(c>='a'&&c<='z');}
static void Validators(Fixture& f) {
    f.words.m_hashmap_words.clear();is_twobyte=is_twobyte_euckr;test_server=0;
    assert(check_name_independent("ordinary")==1 && check_name_independent("ReSeRvEdNaMe")==0);
    std::string prefix(256,'a'),longInput=prefix+"DIFFERENT";
    f.mobs.m_map_pkMobByName[prefix]=&f.reservedMob;
    assert(check_name_independent(longInput.c_str())==0); // Original truncates only the mob lookup.
    f.mobs.m_map_pkMobByName.erase(prefix);
    f.words.m_hashmap_words["bad"]=true;
    assert(!check_name_independent("xxbadxx") && check_name_independent("xxBADxx"));
    f.words.m_hashmap_words.clear();
    int(*names[])(const char*)={check_name_gb2312,check_name_big5,check_name_euckr,
        check_name_latin1,check_name_alphabet,check_name_sjis};
    for(unsigned n=0;n<sizeof names/sizeof names[0];++n) {
        assert(names[n]("Aa1")==1 && names[n]("ReSeRvEdNaMe")==0);
        assert(names[n]("a?")==0 && names[n]("a b")==0 && names[n]("a_")==0);
        f.words.m_hashmap_words["bad"]=true;assert(names[n]("bad")==0);f.words.m_hashmap_words.clear();
    }
    assert(check_name_gb2312("A")==1 && check_name_big5("A")==1 && check_name_sjis("A")==1);
    assert(!check_name_euckr("A") && !check_name_latin1("A") && !check_name_alphabet("A"));
    assert(check_name_sjis("")==1); // Old SJIS path has neither NULL nor empty guard.
    assert(!check_name_gb2312(0) && !check_name_big5(0) && !check_name_euckr(0) &&
           !check_name_latin1(0) && !check_name_alphabet(0));
    std::string longName(64,'A');assert(check_name_euckr(longName.c_str())==1);
    char terminalLead[]={'A',static_cast<char>(0xb0),0,0};
    assert(check_name_euckr(terminalLead)==1);
    const unsigned latin[]={145,146,196,214,220,223,228,246,252};
    for(unsigned i=0;i<sizeof latin/sizeof latin[0];++i) {
        char text[]={'A',static_cast<char>(latin[i]),0,0};
        assert(check_name_latin1(text)==1);
    }
    for(unsigned first=0;first<256;++first)
        for(unsigned second=0;second<256;++second) {
            char text[]={static_cast<char>(first),static_cast<char>(second),0,0};
            const bool ascii=AsciiAlnum(first) && (!second || AsciiAlnum(second));
            const bool gbPair=first>=0xb0 && first<=0xf7 && second>=0xa1 && second<=0xfe &&
                              !(first==0xd7 && second>=0xfa);
            const unsigned code=(first<<8)|second;
            const bool bigPair=second!=0 && ((code>=0xa440 && code<=0xc67e)||(code>=0xc940 && code<=0xf9d5));
            assert(check_name_gb2312(text)==static_cast<int>(ascii||gbPair));
            assert(check_name_big5(text)==static_cast<int>(ascii||bigPair));
            const bool sjis=((first>=0x81&&first<=0x9f)||(first>=0xe0&&first<=0xfc)) &&
                             ((second>=0x40&&second<=0x7e)||(second>=0x80&&second<=0xfc));
            assert(is_char_sjis(text,text+2)==static_cast<int>(sjis));
            assert(is_char_sjis(text,text+1)==0 && is_char_sjis(text,text)==0);
            assert(sjis_is_disable_name_char(text)==(first==0x81 && second==0x40));
            if(sjis) assert(check_name_sjis(text)==(first==0x81&&second==0x40?0:1));
        }
    char invalid[]={'A',static_cast<char>(0x80),static_cast<char>(0x80),0};
    diagExpected=true;diagText=invalid;diagCount=0;test_server=1;
    assert(check_name_big5(invalid)==0 && diagCount==1);
    test_server=0;diagExpected=false;is_twobyte=0;check_name=0;
}
int main() {
    assert(std::setlocale(LC_CTYPE,"C"));desc_test_allow_clock();
    BanwordLocaleApiChecks();HashAndLower();
    assert(singleton<CBanwordManager>::instance_ptr()==0);
    { Fixture f;assert(&CBanwordManager::instance()==&f.words);
      SendAndInitialize(f);Banwords(f);Validators(f); }
    assert(singleton<CBanwordManager>::instance_ptr()==0);
    singleton<CBanwordManager>* base=new CBanwordManager;delete base;
    assert(singleton<CBanwordManager>::instance_ptr()==0);
    std::puts("banword/name locale passed: full class/hash/lifecycle, real GM chat fanout, old scan/convert quirks, bounded ASCII lowercase and exhaustive GB/Big5/SJIS pairs");
}
