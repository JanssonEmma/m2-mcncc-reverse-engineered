#include "desc_fixture.h"
#include "character_fixture.h"
#include "../../game/src/desc_client.h"
#include "../../game/src/input.h"
#include "../../game/src/desc_manager.h"
#include "../../game/src/event.h"
#include "../../game/src/entity.h"
// Test-only access to construct the full old sector containers.
#define private public
#include "../../game/src/sectree.h"
#include "../../game/src/char_manager.h"
#include "../../game/src/questmanager.h"
#undef private
#include "../../game/src/config.h"
#include "../../libthecore/include/main.h"
#include "../../game/src/packet.h"
#include "../../game/src/locale_service.h"
#include "../../game/src/buffer_manager.h"
#include "../../common/length.h"
#include <string>
#include <cassert>
#include <cstring>
extern const BYTE* GetKey_20050304Myevan();
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
extern "C" void desc_test_set_millis(unsigned long);
extern "C" int desc_test_sync_error_line(void);
extern "C" void desc_test_expect_close(int);
extern "C" int desc_test_close_calls(void);
extern "C" int desc_test_entity_error_line(void);
extern "C" int desc_test_sector_error_calls(void);
extern "C" int desc_test_sector_error_line(void);
extern "C" int desc_test_chat_command_logs(void);
extern void (*desc_test_character_disconnect)(CHARACTER*);
extern void (*desc_test_cancel_event)(LPEVENT*);
class Probe : public DescOutputProbe {
public:
    void PrepareCleanup(LPFDWATCH watcher, int socket, CLoginKey* key, CHARACTER* character,
                        LPEVENT ping, LPEVENT disconnect, bool login, bool password) {
        m_lpFdw=watcher; m_sock=socket; m_pkLoginKey=key; m_lpCharacter=character;
        m_lpInputBuffer=buffer_new(0,32); m_lpOutputBuffer=buffer_new(0,32);
        m_pkPingEvent=ping; m_pkDisconnectEvent=disconnect;
        std::memset(&m_accountTable,0,sizeof m_accountTable);
        m_accountTable.id=1; m_dwHandle=0x12345678UL;
        if(login) std::strcpy(m_accountTable.login,"logout");
        if(password) std::strcpy(m_accountTable.passwd,"secret");
        m_pLogFile=std::tmpfile(); assert(m_pLogFile);
    }
    void CheckCleanup() { assert(m_sock==-1 && !m_pLogFile && !m_lpCharacter && !m_pkPingEvent && !m_pkDisconnectEvent); }
    void Empire(BYTE value) { m_accountTable.bEmpire=value; }
    void Retry(int value) { m_iHandshakeRetry=value; }
    bool Encrypted() const { return m_bEncrypted; }
    int Retry() const { return m_iHandshakeRetry; }
    DWORD Sent() const { return m_dwHandshakeSentTime; }
    void Sent(DWORD value) { m_dwHandshakeSentTime=value; }
    BYTE CubeIndex() const { return m_bCRCMagicCubeIdx; }
    DWORD ProcessCRC() const { return m_dwProcCRC; }
    DWORD FileCRC() const { return m_dwFileCRC; }
};
class HandshakeProbe : public CInputHandshake {
public:
    void CheckTables() {
        assert(m_pMainPacketInfo!=m_pPacketInfo);
        assert(m_pMainPacketInfo->GetElement(254)->bSequencePacket);
        assert(m_pMainPacketInfo->GetElement(254)->iSize==2);
        assert(!m_pPacketInfo->GetElement(254)->bSequencePacket);
        assert(m_pPacketInfo->GetElement(254)->iSize==1);
    }
};
class ClientProbe : public CLIENT_DESC {
public:
    void Output(LPBUFFER buffer) { m_lpOutputBuffer=buffer; }
    int SuccessPhase() const { return m_iPhaseWhenSucceed; }
    bool RetryFlag() const { return m_bRetryWhenClosed; }
};
extern int client_test_destroy_calls;
class PacketEntity : public CEntity {
public:
    ~PacketEntity() { Destroy(); }
    void Add(CEntity* entity) { m_map_view[entity]=0; }
    void Observer(bool value) { m_bIsObserver=value; }
    virtual void EncodeInsertPacket(LPENTITY) { std::abort(); }
    virtual void EncodeRemovePacket(LPENTITY) { std::abort(); }
};
// Explicit protocol-encoding substitutes on a full CHARACTER object.
class SpatialCharacter : public CharacterProbe {
public:
    void Kind(BYTE kind) { m_bCharType=kind; }
    DWORD NextPulse() const { return m_dwNextStatePulse; }
    virtual void EncodeInsertPacket(LPENTITY) {}
    virtual void EncodeRemovePacket(LPENTITY) {}
};
static int cleanup_step;
static CHARACTER* cleanup_character;
static void OnDisconnect(CHARACTER* character) { assert(character==cleanup_character && cleanup_step++==0); }
static void OnCancel(LPEVENT* event) { assert(cleanup_step==1 || cleanup_step==2); ++cleanup_step; *event=0; }
static int ReadChat(LPBUFFER output,int offset,BYTE type,BYTE empire,const char* text) {
    int expected=static_cast<int>(std::strlen(text))+10;
    assert(buffer_size(output)>=offset+expected);
    const BYTE* bytes=static_cast<const BYTE*>(buffer_read_peek(output))+offset;
    TPacketGCChat packet; std::memcpy(&packet,bytes,sizeof packet);
    assert(packet.header==4 && packet.size==expected && packet.type==type && packet.id==0 && packet.bEmpire==empire);
    assert(!std::memcmp(bytes+sizeof packet,text,std::strlen(text)+1));
    return offset+expected;
}
static int ReadMonsterChat(LPBUFFER output,int offset,DWORD id,const char* text) {
    int bytes=static_cast<int>(std::strlen(text))+10;
    assert(buffer_size(output)>=offset+bytes);
    const BYTE* encoded=static_cast<const BYTE*>(buffer_read_peek(output))+offset;
    TPacketGCChat header; std::memcpy(&header,encoded,sizeof header);
    assert(header.header==4 && header.size==bytes && header.type==0 && header.id==id && header.bEmpire==0);
    assert(!std::memcmp(encoded+sizeof header,text,std::strlen(text)+1));
    return offset+bytes;
}
static int position_cancel_count;
static LPEVENT position_dead,position_stun;
static void OnPositionCancel(LPEVENT* event) {
    assert(*event==(position_cancel_count==0?position_dead:position_stun));
    ++position_cancel_count; *event=0;
}
int main() {
    quest::CQuestManager quests;
    Probe d; LPBUFFER output=buffer_new(0,256); LPFDWATCH watcher=fdwatch_new(16);
    d.Attach(watcher,5,output); d.SetPacketState(1); d.Retry(9); desc_test_allow_clock();
    d.Handshake(0x11223344UL);
    assert(d.GetHandshake()==0x11223344UL && d.Retry()==0 && buffer_size(output)==13);
    TPacketGCHandshake packet; std::memcpy(&packet,buffer_read_peek(output),sizeof packet);
    assert(packet.bHeader==255 && packet.dwHandshake==d.GetHandshake() && packet.dwTime==d.Sent() && packet.lDelta==0);
    buffer_reset(output);
    d.TrySyncTime(0x55667788UL,-17);
    std::memcpy(&packet,buffer_read_peek(output),sizeof packet);
    assert(packet.dwTime==0x55667788UL && packet.lDelta==-17 && d.Sent()==packet.dwTime);
    buffer_reset(output);
    sockaddr_in address; std::memset(&address,0x5a,sizeof address);
    address.sin_len=16; address.sin_family=2; address.sin_port=0x3412; address.sin_addr.s_addr=0x04030201UL;
    d.UDPGrant(address); assert(!std::memcmp(&d.GetUDPAddr(),&address,sizeof address));
    TPacketGCBindUDP udp; assert(buffer_size(output)==7);
    std::memcpy(&udp,buffer_read_peek(output),sizeof udp);
    assert(udp.header==254 && udp.addr==0x04030201UL && udp.port==0x3412);
    buffer_reset(output);
    d.SetPacketState(0); d.Retry(19); const DWORD before=d.Sent(); d.Handshake(77);
    assert(!buffer_size(output) && d.GetHandshake()==77 && d.Retry()==0 && d.Sent()!=before);
    // Literal known vectors computed independently from the target's TEA round formula.
    const DWORD key[]={0x11223344UL,0xaabbccddUL,0x01020304UL,0xdeadbeefUL};
    const DWORD normal[]={0xd18c286cUL,0x11ba9c22UL,0x159b735eUL,0xfd14def1UL};
    const DWORD localized[]={0x658bc415UL,0x1e5704d0UL,0x109077f9UL,0xaaece3c4UL};
    g_iUseLocale=0; g_eLocalType=LC_JAPAN; d.SetSecurityKey(key);
    assert(!std::memcmp(d.GetDecryptionKey(),key,16) && !std::memcmp(d.GetEncryptionKey(),normal,16));
    g_iUseLocale=1; g_eLocalType=LC_KOREA; d.SetSecurityKey(key);
    assert(!std::memcmp(d.GetEncryptionKey(),normal,16));
    g_eLocalType=LC_JAPAN; d.SetSecurityKey(key);
    assert(!std::memcmp(d.GetEncryptionKey(),localized,16) && !std::memcmp(d.GetDecryptionKey(),key,16));
    const BYTE* generated=GetKey_20050304Myevan(); assert(generated==GetKey_20050304Myevan());
    for(size_t i=38*4;i<1938*4;++i) assert(generated[i]==0); // Dynamic generation stops at38 words.
    d.AssembleCRCMagicCube(1,2); assert(d.CubeIndex()==8 && d.ProcessCRC()==0x67 && d.FileCRC()==0x1c);
    d.AssembleCRCMagicCube(3,4); assert(d.CubeIndex()==16 && d.ProcessCRC()==0xbf67 && d.FileCRC()==0x281c);
    d.AssembleCRCMagicCube(5,6); assert(d.CubeIndex()==24 && d.ProcessCRC()==0x22bf67 && d.FileCRC()==0xcf281c);
    d.AssembleCRCMagicCube(7,8); assert(!d.CubeIndex() && !d.ProcessCRC() && !d.FileCRC());
    g_iUseLocale=0; g_eLocalType=LC_NOSET;
    // Full synchronization state machine with the real name getter and output path.
    buffer_reset(output); d.SetPacketState(1); d.Retry(0); d.Sent(900); desc_test_set_millis(1000);
    assert(d.SyncTime(950,0,false) && d.GetClientTime()==1000 && !buffer_size(output));
    assert(!d.SyncTime(949,0,false) && d.Retry()==1 && buffer_size(output)==13);
    std::memcpy(&packet,buffer_read_peek(output),sizeof packet); assert(packet.lDelta==25);
    buffer_reset(output);
    assert(!d.SyncTime(1000,-1,false) && desc_test_sync_error_line()==565 && !buffer_size(output));
    d.Sent(900); assert(!d.SyncTime(2000,0,false));
    std::memcpy(&packet,buffer_read_peek(output),sizeof packet); assert(packet.lDelta==50);
    buffer_reset(output);
    CharacterProbe character; character.Name("Named player"); d.BindCharacter(&character);
    d.Retry(9); assert(!d.SyncTime(0,0,true) && d.Retry()==9);
    buffer_reset(output);
    assert(d.SyncTime(1000,0,true) && buffer_size(output)==1);
    assert(*static_cast<const BYTE*>(buffer_read_peek(output))==252);
    buffer_reset(output);
    //33rd finite failure closes; the previous32 retries can emit packets.
    d.Retry(0); buffer_adjust_size(output,1024);
    for(int attempt=1;attempt<=33;++attempt) {
        assert(!d.SyncTime(0,0,false) && d.Retry()==attempt);
        if(attempt<33) assert(d.PacketPhase()==1);
    }
    assert(d.PacketPhase()==0 && desc_test_sync_error_line()==603 && buffer_size(output)==32*13);
    d.BindCharacter(0);
    HandshakeProbe handshake; handshake.CheckTables();
    buffer_reset(output); d.SetPacketState(1,false);
    d.SetPhase(2);
    assert(d.Encrypted() && d.GetInputProcessor()->GetType()==2 && buffer_size(output)==2);
    const BYTE* phase_bytes=static_cast<const BYTE*>(buffer_read_peek(output));
    assert(phase_bytes[0]==253 && phase_bytes[1]==2); // Sent before encryption enables.
    buffer_reset(output); d.SetPhase(1);
    assert(d.Encrypted() && d.GetInputProcessor()->GetType()==1 && buffer_size(output)==8);
    buffer_reset(output); d.SetPhase(0);
    assert(d.Encrypted() && d.GetInputProcessor()->GetType()==0 && !buffer_size(output));
    Probe other; other.SetPhase(0); assert(other.GetInputProcessor()==d.GetInputProcessor());
    const int phases[]={1,2,3,4,5,6,10,9};
    const BYTE types[]={1,2,2,2,3,3,8,8};
    for(unsigned i=0;i<sizeof phases/sizeof *phases;++i) {
        buffer_reset(output); d.SetPhase(phases[i]);
        assert(d.PacketPhase()==phases[i] && d.GetInputProcessor()->GetType()==types[i]);
    }

    buffer_reset(output);
    int client_destroyed=client_test_destroy_calls;
    {
        ClientProbe client;
        assert(client.GetType()==1 && client.SuccessPhase()==0 && !client.RetryFlag());
        client.SetRetryWhenClosed(true); assert(client.RetryFlag());
        client.SetRetryWhenClosed(false); assert(!client.RetryFlag());
        client.Output(output);
        client.DBPacket(0xab,0x89abcdefUL,"abc",3);
        const BYTE expected[]={0xab,0xef,0xcd,0xab,0x89,3,0,0,0,'a','b','c'};
        assert(buffer_size(output)==12 && !std::memcmp(buffer_read_peek(output),expected,12));
        buffer_reset(output);
        client.DBPacket(0x11,0x80000001UL,0,0xfedcba98UL);
        const BYTE null_expected[]={0x11,1,0,0,0x80,0x98,0xba,0xdc,0xfe};
        assert(buffer_size(output)==9 && !std::memcmp(buffer_read_peek(output),null_expected,9));
        buffer_reset(output);
        client.DBPacket(0xff,0,"",0);
        client.DBPacketHeader(0x77,0xffffffffUL,0);
        assert(buffer_size(output)==18);
        const BYTE* data=static_cast<const BYTE*>(buffer_read_peek(output));
        assert(data[0]==255 && data[9]==0x77 && data[10]==255 && data[13]==255 && data[17]==0);
        client.Output(0);
    }
    assert(client_test_destroy_calls==client_destroyed+1);
    {
        Probe chat_desc; LPBUFFER chat_output=buffer_new(0,256);
        chat_desc.Attach(watcher,9,chat_output); chat_desc.SetPacketState(1); chat_desc.Empire(3);
        CharacterProbe speaker; speaker.Name("Chat speaker");
        speaker.ChatPacket(1,static_cast<const char*>(0)); assert(!buffer_size(chat_output));
        speaker.BindDesc(&chat_desc);
        speaker.ChatPacket(1,"Value %d/%s",-7,"ok"); int offset=ReadChat(chat_output,0,1,3,"Value -7/ok");
        speaker.ChatPacket(2,""); offset=ReadChat(chat_output,offset,2,3,"");
        std::string maximum(CHAT_MAX_LEN-1,'x'); speaker.ChatPacket(3,"%s",maximum.c_str());
        offset=ReadChat(chat_output,offset,3,3,maximum.c_str()); assert(buffer_size(chat_output)==offset);
        buffer_reset(chat_output); int logs=desc_test_chat_command_logs();
        test_server=0; speaker.ChatPacket(5,"command %d",1); assert(desc_test_chat_command_logs()==logs);
        test_server=1; speaker.ChatPacket(5,"command %d",2); assert(desc_test_chat_command_logs()==logs+1); test_server=0;
        offset=ReadChat(chat_output,0,5,3,"command 1"); offset=ReadChat(chat_output,offset,5,3,"command 2");
        assert(buffer_size(chat_output)==offset); speaker.BindDesc(0); buffer_delete(chat_output);
    }
    {
        CHARACTER_MANAGER manager;
        CharacterProbe original,replacement;
        assert(original.GetVID()==VID()); original.Identity(42,0x11111111UL);
        manager.m_map_pkChrByVID[42]=&original;
        assert(manager.Find(VID(42,0x11111111UL))==&original);
        assert(!manager.Find(VID(42,0x22222222UL)) && !manager.Find(VID(43,0x11111111UL)));
        VID stale=original.GetVID(); replacement.Identity(42,0x22222222UL);
        manager.m_map_pkChrByVID[42]=&replacement;
        assert(manager.Find(42)==&replacement && !manager.Find(stale) && manager.Find(replacement.GetVID())==&replacement);
        original.Identity(43,0x11111111UL); manager.m_map_pkChrByVID[42]=&original;
        assert(manager.Find(42)==&original && !manager.Find(VID(42,0x11111111UL)));
        manager.m_map_pkChrByVID.clear();
    }
    {
        CHARACTER_MANAGER manager;
        Probe nearby_desc,distant_desc;
        LPBUFFER nearby_output=buffer_new(0,128),distant_output=buffer_new(0,128);
        nearby_desc.Attach(watcher,11,nearby_output); nearby_desc.SetPacketState(1);
        distant_desc.Attach(watcher,12,distant_output); distant_desc.SetPacketState(1);
        CharacterProbe monster,nearby,distant,unbound;
        monster.Identity(0xffffffffUL,17); monster.SetXYZ(0,0,0);
        nearby.SetXYZ(6244,0,0); distant.SetXYZ(6245,0,0); unbound.SetXYZ(0,0,0);
        nearby.BindDesc(&nearby_desc); distant.BindDesc(&distant_desc);
        nearby.SetMapIndex(99); monster.SetMapIndex(1); //The old diagnostic path ignores map membership.
        manager.RegisterForMonsterLog(&nearby); manager.RegisterForMonsterLog(&distant); manager.RegisterForMonsterLog(&unbound);
        test_server=0; monster.MonsterLog(static_cast<const char*>(0));
        assert(!buffer_size(nearby_output) && !buffer_size(distant_output));
        test_server=1; monster.BindDesc(&nearby_desc); monster.MonsterLog(static_cast<const char*>(0)); monster.BindDesc(0);
        assert(!buffer_size(nearby_output));
        monster.MonsterLog("awake %d",7);
        int log_offset=ReadMonsterChat(nearby_output,0,0xffffffffUL,"4294967295)awake 7");
        assert(buffer_size(nearby_output)==log_offset && !buffer_size(distant_output));
        monster.BeginStateEmpty(); log_offset=ReadMonsterChat(nearby_output,log_offset,0xffffffffUL,"4294967295)!");
        assert(buffer_size(nearby_output)==log_offset);
        buffer_reset(nearby_output);
        std::string safe_text(74,'m'); monster.MonsterLog("%s",safe_text.c_str());
        std::string expected="4294967295)"+safe_text;
        log_offset=ReadMonsterChat(nearby_output,0,0xffffffffUL,expected.c_str()); assert(buffer_size(nearby_output)==log_offset);
        buffer_reset(nearby_output);
        manager.PacketMonsterLog(0,"B",1);
        assert(buffer_size(nearby_output)==1 && buffer_size(distant_output)==1);
        assert(*static_cast<const char*>(buffer_read_peek(distant_output))=='B');
        buffer_reset(nearby_output); buffer_reset(distant_output);
        monster.SetXYZ(6245,0,0); manager.PacketMonsterLog(&monster,"C",1);
        assert(buffer_size(nearby_output)==1 && buffer_size(distant_output)==1);
        buffer_reset(nearby_output); buffer_reset(distant_output); monster.SetXYZ(0,0,0);
        //Standing cancellation runs in dead/stun order, then queues idle.
        int dead_token=1,stun_token=2;
        position_dead=reinterpret_cast<LPEVENT>(&dead_token); position_stun=reinterpret_cast<LPEVENT>(&stun_token);
        monster.PreparePosition(0,0xff,-1,position_dead,position_stun);
        position_cancel_count=0; desc_test_cancel_event=OnPositionCancel;
        monster.SetPosition(7); desc_test_cancel_event=0;
        assert(position_cancel_count==2 && monster.Position()==7 && monster.AddedState()==0xfe);
        assert(static_cast<unsigned long>(monster.InstantFlags())==0xfffffff7UL && monster.PendingIdle());
        std::string idle="4294967295)[IDLE] \xbd\xac\xb4\xc2 \xbb\xf3\xc5\xc2";
        log_offset=ReadMonsterChat(nearby_output,0,0xffffffffUL,idle.c_str());
        monster.ForceIdle(); monster.SetPosition(0);
        assert(monster.Position()==0 && monster.AddedState()==0xff && monster.PendingIdle());
        assert(buffer_size(nearby_output)==log_offset); //No log when already in the requested current state.
        monster.SetPosition(5); assert(monster.Position()==5 && monster.PendingBattle());
        std::string battle="4294967295)[BATTLE] \xbd\xce\xbf\xec\xb4\xc2 \xbb\xf3\xc5\xc2";
        log_offset=ReadMonsterChat(nearby_output,log_offset,0xffffffffUL,battle.c_str());
        monster.ForceBattle(); monster.SetPosition(5); assert(buffer_size(nearby_output)==log_offset && monster.PendingBattle());
        monster.ForceIdle(); monster.PreparePosition(2,0,0,0,0); monster.SetPosition(5);
        assert(monster.Position()==5 && !monster.HasPendingState() && buffer_size(nearby_output)==log_offset);
        test_server=0; manager.UnregisterForMonsterLog(&nearby); manager.UnregisterForMonsterLog(&distant); manager.UnregisterForMonsterLog(&unbound);
        nearby.BindDesc(0); distant.BindDesc(0); buffer_delete(nearby_output); buffer_delete(distant_output);
    }
    // Entity fanout through real descriptor buffers; sector pointer is only an opaque membership gate.
    {
        Probe owner,peer; LPBUFFER owner_buffer=buffer_new(0,64),peer_buffer=buffer_new(0,64);
        owner.Attach(watcher,7,owner_buffer); peer.Attach(watcher,8,peer_buffer);
        owner.SetPacketState(1); peer.SetPacketState(1);
        PacketEntity source,neighbor,unbound;
        source.Add(&neighbor); source.Add(&unbound);
        source.BindDesc(&owner); neighbor.BindDesc(&peer); neighbor.Observer(true);
        source.PacketView("xy",2); assert(!buffer_size(owner_buffer) && !buffer_size(peer_buffer));
        int sector_token=0; source.SetSectree(reinterpret_cast<LPSECTREE>(&sector_token));
        source.PacketAround("xy",2); assert(buffer_size(owner_buffer)==2 && buffer_size(peer_buffer)==2);
        buffer_reset(owner_buffer); buffer_reset(peer_buffer);
        source.PacketView("a",1,&source); assert(!buffer_size(owner_buffer) && buffer_size(peer_buffer)==1);
        buffer_reset(peer_buffer); source.PacketView("b",1,&neighbor);
        assert(buffer_size(owner_buffer)==1 && !buffer_size(peer_buffer));
        buffer_reset(owner_buffer); source.Observer(true); source.PacketView("c",1);
        assert(buffer_size(owner_buffer)==1 && !buffer_size(peer_buffer));
        buffer_reset(owner_buffer); source.PacketView("d",1,&source);
        assert(!buffer_size(owner_buffer) && !buffer_size(peer_buffer));
        source.ViewCleanup();
        buffer_delete(owner_buffer); buffer_delete(peer_buffer);
    }
    {
        CHARACTER_MANAGER manager;
        SECTREE sector;
        SpatialCharacter me,npc,warp,goto_entity,player;
        SpatialCharacter* all[]={&me,&npc,&warp,&goto_entity,&player};
        for(unsigned i=0;i<5;++i) { all[i]->SetType(0); all[i]->Kind(0); all[i]->SetXYZ(0,0,0); sector.m_set_entity.insert(all[i]); }
        me.Name("Spatial player"); me.BindDesc(&d); player.BindDesc(&d); warp.Kind(3); goto_entity.Kind(9);
        me.SetSectree(&sector); sector.m_neighbor_list.push_back(&sector);
        d.SetPacketState(1); d.Empire(2); buffer_reset(output); thecore_heart->pulse=100;
        me.UpdateSectree();
        assert(me.GetViewAge()==1 && manager.m_set_pkChrState.size()==1 && manager.m_set_pkChrState.count(&npc) && npc.NextPulse()==101);
        assert(!warp.NextPulse() && !goto_entity.NextPulse() && !player.NextPulse());
        thecore_heart->pulse=200; me.SetObserverMode(true);
        int chat_offset=ReadChat(output,0,5,2,"ObserverMode 1");
        assert(me.IsObserverMode() && me.GetViewAge()==2 && manager.m_set_pkChrState.size()==1 && npc.NextPulse()==101);
        me.SetObserverMode(true); assert(manager.m_set_pkChrState.size()==1 && buffer_size(output)==chat_offset);
        me.SetObserverMode(false);
        assert(!me.IsObserverMode() && me.GetViewAge()==3 && npc.NextPulse()==101);
        chat_offset=ReadChat(output,chat_offset,5,2,"ObserverMode 0");
        npc.StopStateMachine(); assert(manager.m_set_pkChrState.empty());
        thecore_heart->pulse=250; npc.StartStateMachine(5); assert(npc.NextPulse()==255);
        npc.StartStateMachine(-1); assert(npc.NextPulse()==255); npc.StopStateMachine(); npc.StopStateMachine();
        thecore_heart->pulse=2147483647; npc.StartStateMachine(1); assert(npc.NextPulse()==0x80000000UL); npc.StopStateMachine();
        thecore_heart->pulse=0; npc.StartStateMachine(-1); assert(npc.NextPulse()==0xffffffffUL); npc.StopStateMachine();
        me.SetSectree(0); me.SetObserverMode(true);
        assert(me.GetViewAge()==3 && desc_test_entity_error_line()==120);
        chat_offset=ReadChat(output,chat_offset,5,2,"ObserverMode 1"); assert(buffer_size(output)==chat_offset);
        me.ViewCleanup(); assert(manager.m_set_pkChrState.empty());
    }
    {
        CHARACTER_MANAGER manager;
        SECTREE a,b,empty;
        a.m_neighbor_list.push_back(&a); a.m_neighbor_list.push_back(&b);
        b.m_neighbor_list.push_back(&a); b.m_neighbor_list.push_back(&b);
        SpatialCharacter player,npc,fresh,warp,goto_entity,orphan;
        SpatialCharacter* all[]={&player,&npc,&fresh,&warp,&goto_entity,&orphan};
        for(unsigned i=0;i<6;++i) { all[i]->SetType(0); all[i]->Kind(0); }
        player.BindDesc(&d); warp.Kind(3); goto_entity.Kind(9); thecore_heart->pulse=300;
        assert(a.InsertEntity(&npc) && !a.InsertEntity(&npc) && manager.m_set_pkChrState.empty());
        assert(a.InsertEntity(&player) && a.m_iPCCount==1 && b.m_iPCCount==1 && manager.m_set_pkChrState.empty());
        assert(a.InsertEntity(&fresh) && manager.m_set_pkChrState.size()==1 && manager.m_set_pkChrState.count(&fresh) && fresh.NextPulse()==301);
        assert(a.InsertEntity(&warp) && a.InsertEntity(&goto_entity) && manager.m_set_pkChrState.size()==1);
        assert(b.InsertEntity(&player) && player.GetSectree()==&b && !a.m_set_entity.count(&player) && b.m_set_entity.count(&player));
        assert(a.m_iPCCount==1 && b.m_iPCCount==1 && manager.m_set_pkChrState.count(&fresh));
        assert(a.InsertEntity(&player) && manager.m_set_pkChrState.count(&fresh));
        npc.StartStateMachine(); warp.StartStateMachine(); goto_entity.StartStateMachine();
        assert(manager.m_set_pkChrState.size()==4);
        a.RemoveEntity(&player);
        assert(!player.GetSectree() && !a.m_iPCCount && !b.m_iPCCount && manager.m_set_pkChrState.empty());
        int errors=desc_test_sector_error_calls(); a.RemoveEntity(&player);
        assert(desc_test_sector_error_calls()==errors+2 && desc_test_sector_error_line()==103 && manager.m_set_pkChrState.empty());
        a.m_set_entity.insert(&orphan); errors=desc_test_sector_error_calls();
        assert(a.InsertEntity(&orphan) && orphan.GetSectree()==&a && desc_test_sector_error_calls()==errors+1 && desc_test_sector_error_line()==139);
        a.m_set_entity.erase(&npc); assert(!a.InsertEntity(&npc) && !a.m_set_entity.count(&npc)); a.m_set_entity.insert(&npc);
        assert(empty.InsertEntity(&player) && !empty.m_iPCCount); empty.RemoveEntity(&player);
        for(unsigned i=1;i<6;++i) a.RemoveEntity(all[i]);
        assert(a.m_set_entity.empty() && b.m_set_entity.empty() && manager.m_set_pkChrState.empty());
        thecore_heart->pulse=0;
    }
    // Real DESC teardown with registry and DB writer, scripted character/events/close.
    buffer_reset(output);
    {
        DESC_MANAGER manager;
        ClientProbe database; database.Output(output); db_clientdesc=&database;
        CLoginKey login_key(71,&d);
        EVENT ping,disconnect; std::memset(&ping,0,sizeof ping); std::memset(&disconnect,0,sizeof disconnect);
        cleanup_character=&character; cleanup_step=0;
        desc_test_character_disconnect=OnDisconnect; desc_test_cancel_event=OnCancel;
        manager.ConnectAccount("logout",&d);
        d.PrepareCleanup(watcher,5,&login_key,&character,&ping,&disconnect,true,true);
        desc_test_expect_close(5); g_bAuthServer=false;
        d.Destroy(); d.CheckCleanup();
        assert(cleanup_step==3 && desc_test_close_calls()==1 && !manager.FindByLoginName("logout"));
        assert(login_key.m_dwExpireTime==1000 && !login_key.m_pkDesc);
        assert(buffer_size(output)==57);
        const BYTE* logout=static_cast<const BYTE*>(buffer_read_peek(output));
        const BYTE logout_header[]={2,0x78,0x56,0x34,0x12,48,0,0,0};
        assert(!std::memcmp(logout,logout_header,9));
        TLogoutPacket payload; std::memcpy(&payload,logout+9,sizeof payload);
        assert(!std::strcmp(payload.login,"logout") && !std::strcmp(payload.passwd,"secret"));
        assert(payload.login[30]==0 && payload.passwd[16]==0);
        desc_test_character_disconnect=0; desc_test_cancel_event=0;
        for(int branch=0;branch<3;++branch) {
            buffer_reset(output); g_bAuthServer=branch==0;
            d.PrepareCleanup(watcher,-1,0,0,0,0,branch!=1,branch!=2);
            d.Destroy(); d.CheckCleanup(); assert(!buffer_size(output));
        }
        g_bAuthServer=false; db_clientdesc=0; database.Output(0);
    }
    buffer_delete(output); fdwatch_delete(watcher);
    std::puts("descriptor controls passed: handshake fields, UDP byte order, both key branches and CRC reset");
}
