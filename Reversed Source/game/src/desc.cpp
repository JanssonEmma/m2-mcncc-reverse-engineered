#include <cstdint>
#include "locale.hpp"
// Partial DESC recovery; see reconstruction/desc-lifecycle-ghidra.json.
// Remaining DESC methods intentionally have no production substitute bodies.
#include "desc.h"
#include "input.h"
#include "char.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "guild.h"
#include "guild_manager.h"
#include "event.h"
#include "locale_service.h"
#include "../../libthecore/include/memcpy.h"
#include "buffer_manager.h"
#include "sequence.h"
#include "config.h"
#include "packet.h"
#include "protocol.h"
#include "TrafficProfiler.h"
#include "../../libthecore/include/crypt.h"
#include <cassert>
#include <cstring>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdarg>
#include "utils.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/socket.h"
#include "../../libthecore/include/utils.h"
extern int max_bytes_written;
extern int current_bytes_written;
extern int total_bytes_written;

DESC::DESC() { Initialize(); }
DESC::~DESC() { Destroy(); }
BYTE DESC::GetType() { return 0; }

// Initialize at 0x08139200 deliberately preserves the other members.
void DESC::Initialize()
{
    std::memset(&m_UDPSockAddr, 0, sizeof m_UDPSockAddr);
    m_pInputProcessor = 0;
    m_lpFdw = 0;
    m_sock = -1;
    m_iPhase = 0;
    m_dwHandle = 0;
    m_iHandshakeRetry = 0;
    m_wPort = 0;
    m_LastTryTime = 0;
    m_lpInputBuffer = 0;
    m_iMinInputBufferLen = 0;
    m_dwHandshake = 0;
    m_lpOutputBuffer = 0;
    m_lpCharacter = 0;
    m_lpBufferedOutputBuffer = 0;
    std::memset(&m_accountTable, 0, sizeof m_accountTable);
    m_iCurrentSequence = 0;
    m_dwMatrixCols = 0;
    m_dwMatrixRows = 0;
    m_pkLoginKey = 0;
    m_pLogFile = 0;
    m_dwProcCRC = 0;
    m_pkPingEvent = 0;
    m_bEncrypted = false;
    m_pkDisconnectEvent = 0;
    m_wP2PPort = 0;
    m_bAdminMode = false;
    m_bPong = true;
    m_bMatrixTryCount = 0;
    m_bPasspodPending = false;
    m_bCRCMagicCubeIdx = 0;
    m_dwFileCRC = 0;
    m_bHackCRCQuery = false;
    m_dwBillingExpireSecond = 0;
    m_dwLoginKey = 0;
}

int DESC::ProcessOutput()
{
    if (buffer_size(m_lpOutputBuffer) == 0)
        return 0;
    int buffer_left = fdwatch_get_buffer_size(m_lpFdw, m_sock);
    if (buffer_left <= 0)
        return 0;
    // Target calls the recovered signed-int MIN function at081b9260.
    size_t bytes_to_write = MIN(buffer_left, static_cast<int>(buffer_size(m_lpOutputBuffer)));
    if (bytes_to_write == 0)
        return 0;
    const char* data = static_cast<const char*>(buffer_read_peek(m_lpOutputBuffer));
    int result = socket_write(m_sock, data, bytes_to_write);
    if (result == 0) {
        // The original globals are int; target operations retain32-bit wrapping.
        max_bytes_written = MAX(static_cast<int>(bytes_to_write), max_bytes_written);
        total_bytes_written = static_cast<int>(static_cast<unsigned int>(total_bytes_written)
            + static_cast<unsigned int>(bytes_to_write));
        current_bytes_written = static_cast<int>(static_cast<unsigned int>(current_bytes_written)
            + static_cast<unsigned int>(bytes_to_write));
        buffer_read_proceed(m_lpOutputBuffer, static_cast<int>(bytes_to_write));
        if (buffer_size(m_lpOutputBuffer) != 0)
            fdwatch_add_fd(m_lpFdw, m_sock, this, FDW_WRITE, 1);
    }
    return result;
}

// Addressed descriptor helpers; evidence in desc-helpers-ghidra.json.
void DESC::Log(const char* format, ...)
{
    if (m_pLogFile) {
        time_t now = get_global_time();
        struct tm stamp = *std::localtime(&now);
        std::fprintf(m_pLogFile, "%02d %02d %02d:%02d:%02d | ",
            stamp.tm_mon + 1, stamp.tm_mday, stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
        va_list args;
        va_start(args, format);
        std::vfprintf(m_pLogFile, format, args);
        va_end(args);
        std::fputc('\n', m_pLogFile);
        std::fflush(m_pLogFile);
    }
}
DWORD DESC::GetClientTime() { return m_dwClientTime; }
BYTE DESC::GetEmpire() { return m_accountTable.bEmpire; }
void DESC::SetRelay(const char* name) { m_stRelayName = name; }
void DESC::BindCharacter(CHARACTER* ch) { m_lpCharacter = ch; }
void DESC::SetAdminMode() { m_bAdminMode = true; }
bool DESC::IsAdminMode() { return m_bAdminMode; }
bool DESC::IsPong() { return m_bPong; }
void DESC::SetPong(bool pong) { m_bPong = pong; }
void DESC::SetMatrixCardRowsAndColumns(unsigned long rows, unsigned long cols)
{ m_dwMatrixRows = rows; m_dwMatrixCols = cols; }
unsigned long DESC::GetMatrixRows() { return m_dwMatrixRows; }
unsigned long DESC::GetMatrixCols() { return m_dwMatrixCols; }
bool DESC::CheckMatrixTryCount()
{
    m_bMatrixTryCount = static_cast<BYTE>(m_bMatrixTryCount + 1);
    return m_bMatrixTryCount < 3;
}
void DESC::SetLoginKey(DWORD key) { m_dwLoginKey = key; }
void DESC::SetBillingExpireSecond(DWORD seconds) { m_dwBillingExpireSecond = seconds; }
DWORD DESC::GetBillingExpireSecond() { return m_dwBillingExpireSecond; }

void DESC::SetLoginKey(CLoginKey* key)
{
    m_pkLoginKey = key;
    sys_log(0, "SetLoginKey %lu", key->m_dwKey);
}
DWORD DESC::GetLoginKey() { return m_pkLoginKey ? m_pkLoginKey->m_dwKey : m_dwLoginKey; }

// Packet writing at 0x08139cb0, 0x08139d10, 0x0813a1d0.
void DESC::BufferedPacket(const void* data, int size)
{
    if (!m_lpBufferedOutputBuffer)
        m_lpBufferedOutputBuffer = buffer_new(0, 1024);
    buffer_write(m_lpBufferedOutputBuffer, data, size);
}

void DESC::Packet(const void* data, int size)
{
    assert(size > 0);
    if (m_iPhase == 0)
        return;
    if (!m_stRelayName.empty()) {
        TPacketGGRelay relay;
        relay.bHeader = 3;
        std::strncpy(relay.szName, m_stRelayName.c_str(), 24);
        relay.szName[24] = 0;
        relay.lSize = size;
        if (!packet_encode(m_lpOutputBuffer, &relay, sizeof relay)) {
            m_iPhase = 0;
            return;
        }
        m_stRelayName = "";
        if (!packet_encode(m_lpOutputBuffer, data, size)) {
            m_iPhase = 0;
            return;
        }
        fdwatch_add_fd(m_lpFdw, m_sock, this, FDW_WRITE, 1);
        return;
    }
    if (m_lpBufferedOutputBuffer) {
        buffer_write(m_lpBufferedOutputBuffer, data, size);
        data = buffer_read_peek(m_lpBufferedOutputBuffer);
        size = static_cast<int>(buffer_size(m_lpBufferedOutputBuffer));
    }
    if (g_bTrafficProfileOn)
        TrafficProfiler::instance().Report(TrafficProfiler::IODIR_OUTPUT,
            *static_cast<const BYTE*>(data), static_cast<DWORD>(size));
    if (!m_bEncrypted) {
        if (!packet_encode(m_lpOutputBuffer, data, size))
            m_iPhase = 0;
    } else if (buffer_has_space(m_lpOutputBuffer) <
               static_cast<int>(static_cast<unsigned int>(size) + 8U)) {
        sys_log(0, "buffer mem_size  = %d", m_lpOutputBuffer->mem_size);
        sys_log(0, "buffer write_pos = %d", m_lpOutputBuffer->write_point_pos);
        sys_log(0, "iSize = %d", size);
        m_iPhase = 0;
    } else {
        DWORD* destination = static_cast<DWORD*>(buffer_write_peek(m_lpOutputBuffer));
        if (packet_encode(m_lpOutputBuffer, data, size)) {
            const int encoded_size = TEA_Encrypt(destination, destination, m_adwEncryptionKey, size);
            if (size < encoded_size)
                buffer_write_proceed(m_lpOutputBuffer, encoded_size - size);
        }
    }
    if (m_lpBufferedOutputBuffer) {
        buffer_delete(m_lpBufferedOutputBuffer);
        m_lpBufferedOutputBuffer = 0;
    }
    // Ordinary failures still register writable interest after closing the phase.
    fdwatch_add_fd(m_lpFdw, m_sock, this, FDW_WRITE, 1);
}

void DESC::LargePacket(const void* data, int size)
{
    sys_log(0, "LargePacket Size %d  Buffer Size %d", size, buffer_size(m_lpOutputBuffer));
    buffer_adjust_size(m_lpOutputBuffer, size);
    Packet(data, size);
}

BYTE DESC::GetSequence() { return gc_abSequence[m_iCurrentSequence]; }
void DESC::SetNextSequence()
{
    m_iCurrentSequence=static_cast<int>(static_cast<unsigned int>(m_iCurrentSequence)+1U);
    if(m_iCurrentSequence==65536) m_iCurrentSequence=0;
}

int DESC::ProcessInput()
{
    if(!m_lpInputBuffer) {
        _sys_err("ProcessInput",216,"DESC::ProcessInput : nil input buffer");
        return -1;
    }
    buffer_adjust_size(m_lpInputBuffer,m_iMinInputBufferLen);
    const size_t space=buffer_has_space(m_lpInputBuffer);
    char* destination=static_cast<char*>(buffer_write_peek(m_lpInputBuffer));
    const int received=socket_read(m_sock,destination,space);
    if(received<0) return -1;
    if(received==0) return 0;
    buffer_write_proceed(m_lpInputBuffer,received);
    if(!m_pInputProcessor) {
        _sys_err("ProcessInput",231,"no input processor");
        return received;
    }
    if(!m_bEncrypted) {
        for(;;) {
            int consumed=0;
            const bool done=m_pInputProcessor->Process(this,buffer_read_peek(m_lpInputBuffer),
                static_cast<int>(buffer_size(m_lpInputBuffer)),consumed);
            buffer_read_proceed(m_lpInputBuffer,consumed);
            if(done) break;
        }
    } else {
        TEMP_BUFFER temporary;
        buffer_adjust_size(temporary.getptr(),static_cast<int>(buffer_size(m_lpInputBuffer)+8UL));
        DWORD remaining=buffer_size(m_lpInputBuffer);
        const DWORD* source=static_cast<const DWORD*>(buffer_read_peek(m_lpInputBuffer));
        DWORD* clear=static_cast<DWORD*>(buffer_write_peek(temporary.getptr()));
        const int decrypted=TEA_Decrypt(clear,source,m_adwDecryptionKey,static_cast<int>(remaining));
        buffer_write_proceed(temporary.getptr(),decrypted);
        int consumed;
        for(;;) {
            consumed=0;
            if(m_pInputProcessor->Process(this,buffer_read_peek(temporary.getptr()),
               static_cast<int>(buffer_size(temporary.getptr())),consumed)) break;
            if(static_cast<int>(remaining)<consumed) {
                buffer_read_proceed(m_lpInputBuffer,static_cast<int>(remaining));
                consumed=0;
                break;
            }
            buffer_read_proceed(m_lpInputBuffer,consumed);
            remaining-=consumed;
            buffer_read_proceed(temporary.getptr(),consumed);
        }
        buffer_read_proceed(m_lpInputBuffer,consumed);
    }
    return received;
}

void DESC::FlushOutput()
{
    if(!buffer_size(m_lpOutputBuffer)) return;
    bool triggered=false;
    struct timeval start,now,timeout;
    gettimeofday(&start,0);
    socket_block(m_sock);
    sys_log(0,"FLUSH START %d",buffer_size(m_lpOutputBuffer));
    do {
        if(!buffer_size(m_lpOutputBuffer)) break;
        gettimeofday(&now,0);
        const int elapsed=static_cast<int>(static_cast<unsigned int>(now.tv_sec)-static_cast<unsigned int>(start.tv_sec));
        if(elapsed>10 && (!triggered || elapsed>20)) {
            SetPhase(0);
            break;
        }
        timeout.tv_sec=0; timeout.tv_usec=10000;
        const int events=fdwatch(m_lpFdw,&timeout);
        if(events<0) {
            _sys_err("FlushOutput",667,"num_events < 0 : %d",events);
            break;
        }
        for(int index=0;index<events;++index) {
            if(fdwatch_get_client_data(m_lpFdw,static_cast<unsigned int>(index))!=this) continue;
            const int event=fdwatch_check_event(m_lpFdw,m_sock,static_cast<unsigned int>(index));
            if(event==FDW_WRITE) {
                triggered=true;
                if(ProcessOutput()<0) {
                    _sys_err("FlushOutput",687,"Cannot flush output buffer");
                    SetPhase(0);
                }
            } else if(event==FDW_EOF) SetPhase(0);
        }
    } while(m_iPhase!=0);
    sys_log(0,buffer_size(m_lpOutputBuffer)?"FLUSH FAIL":"FLUSH SUCCESS");
    usleep(250000);
}

void DESC::Handshake(DWORD handshake)
{
    m_dwHandshake=handshake;
    TrySyncTime(get_dword_time(),0);
    m_iHandshakeRetry=0;
}
void DESC::TrySyncTime(DWORD now,long delta)
{
    TPacketGCHandshake packet;
    packet.bHeader=255; packet.dwHandshake=m_dwHandshake;
    packet.dwTime=now; packet.lDelta=delta;
    Packet(&packet,sizeof packet);
    m_dwHandshakeSentTime=now;
}
void DESC::UDPGrant(const sockaddr_in& address)
{
    m_UDPSockAddr=address;
    TPacketGCBindUDP packet;
    packet.header=254; packet.addr=m_UDPSockAddr.sin_addr.s_addr;
    packet.port=m_UDPSockAddr.sin_port;
    Packet(&packet,sizeof packet);
}
const BYTE* GetKey_20050304Myevan()
{
    static bool generated=false;
    static DWORD key[1938];
    if(!generated) {
        generated=true;
        DWORD seed=0x58edadb9UL;
        unsigned int index=0;
        do {
            seed=(seed^0x80164043UL)+0xd61f75bdUL;
            key[index++]=seed;
        } while(index<static_cast<BYTE>(seed));
    }
    return reinterpret_cast<const BYTE*>(key);
}
void DESC::SetSecurityKey(const DWORD* key)
{
    const BYTE* mask=reinterpret_cast<const BYTE*>("JyTxtHljHJlVJHorRM301vf@4fvj10-v");
    if(g_iUseLocale && !LC_IsKorea()) mask=GetKey_20050304Myevan()+37;
    thecore_memcpy(m_adwDecryptionKey,key,16);
    TEA_Encrypt(m_adwEncryptionKey,m_adwDecryptionKey,reinterpret_cast<const DWORD*>(mask),16);
    sys_log(0,"SetSecurityKey decrypt %lu %lu %lu %lu encrypt %lu %lu %lu %lu",
        m_adwDecryptionKey[0],m_adwDecryptionKey[1],m_adwDecryptionKey[2],m_adwDecryptionKey[3],
        m_adwEncryptionKey[0],m_adwEncryptionKey[1],m_adwEncryptionKey[2],m_adwEncryptionKey[3]);
}
void DESC::AssembleCRCMagicCube(BYTE process_piece,BYTE file_piece)
{
    static BYTE xor_table[32]={102,30,0,0,0,0,0,0,188,44,0,0,0,0,0,0,
                               39,201,0,0,0,0,0,0,43,5,0,0,0,0,0,0};
    const BYTE index=m_bCRCMagicCubeIdx;
    m_bCRCMagicCubeIdx=static_cast<BYTE>(index+8);
    m_dwProcCRC|=static_cast<DWORD>(xor_table[index]^process_piece)<<(index&31);
    m_dwFileCRC|=static_cast<DWORD>(xor_table[index+1]^file_piece)<<(index&31);
    if(((index+8)&31)==0) {
        m_bCRCMagicCubeIdx=0; m_dwProcCRC=0; m_dwFileCRC=0;
    }
}

bool DESC::SyncTime(DWORD time,long delta,bool infinite_retry)
{
    const std::uint32_t now=static_cast<std::uint32_t>(get_dword_time());
    const std::uint32_t client_time=static_cast<std::uint32_t>(time);
    if(delta<0) {
        _sys_err("SyncTime",565,"Desc::SyncTime : value error (lDelta %d, ip %s)",delta,m_stHost.c_str());
        return false;
    }
    const std::uint32_t bias=now-(client_time+static_cast<std::uint32_t>(delta));
    if(bias<51) {
        if(infinite_retry) {
            const BYTE header=252;
            Packet(&header,1);
        }
        m_dwClientTime=now;
        if(m_lpCharacter) sys_log(0,"SyncTime: bias %d name: %s",bias,m_lpCharacter->GetName());
        else sys_log(0,"SyncTime: bias %d",bias);
        return true;
    }
    long next_delta=static_cast<std::int32_t>(now-client_time)/2;
    if(next_delta<0) {
        sys_log(0,"SyncTime: lower than zero %d",next_delta);
        next_delta=static_cast<long>((now-static_cast<std::uint32_t>(m_dwHandshakeSentTime))>>1);
    }
    sys_log(1,"SyncTime: ServerTime %u dwTime %u lDelta %d SentTime %u lNewDelta %d",
        now,time,delta,m_dwHandshakeSentTime,next_delta);
    bool retry=infinite_retry;
    if(!retry) {
        m_iHandshakeRetry=static_cast<int>(static_cast<unsigned int>(m_iHandshakeRetry)+1U);
        retry=m_iHandshakeRetry<33;
    }
    if(retry) TrySyncTime(now,next_delta);
    else {
        const char* name=m_lpCharacter?m_lpCharacter->GetName():"!NO CHARACTER!";
        _sys_err("SyncTime",603,"handshake retry limit reached! (limit %d character %s)",32,name);
        SetPhase(0);
    }
    return false;
}

//0813a240. All six function-static processors initialize before phase selection.
namespace {
struct PingEventInfo {
    DESC* desc;
};
}

EVENTFUNC(ping_event)
{
    DESC* desc = static_cast<PingEventInfo*>(event->info)->desc;
    if (!desc->IsAdminMode()) {
        if (desc->IsPong()) {
            const BYTE ping = 0x2c;
            desc->Packet(&ping, sizeof(ping));
            desc->SetPong(false);
        } else {
            sys_log(0, "PING_EVENT: no pong %s", desc->GetHostName());
            desc->SetPhase(0);
        }
        desc->TrySyncTime(get_dword_time(), 0);
    }
    return ping_event_second_cycle;
}

// 08139660
bool DESC::Setup(LPFDWATCH watcher, socket_t socket, const sockaddr_in& peer, DWORD handle, DWORD handshake)
{
    m_lpFdw = watcher;
    m_stHost.assign(inet_ntoa(peer.sin_addr));
    m_dwHandle = handle;
    m_lpOutputBuffer = buffer_new(nullptr, 0x10000);
    if (!m_lpOutputBuffer)
        return false;
    m_iMinInputBufferLen = 0x400;
    m_lpInputBuffer = buffer_new(nullptr, 0x800);
    if (!m_lpInputBuffer)
        return false;

    m_sock = socket;
    m_SockAddr = peer;
    fdwatch_add_fd(m_lpFdw, m_sock, this, FDW_READ, 0);

    PingEventInfo* info;
    const DWORD id = __AllocEventInfo(sizeof(PingEventInfo), reinterpret_cast<void**>(&info));
    info->desc = this;
    assert(!m_pkPingEvent);
    m_pkPingEvent = event_create_ex(ping_event, id, ping_event_second_cycle,
                                    const_cast<char*>("desc.cpp"), 0xba);

    const char* key = LC_IsEurope() || LC_IsGermany() ? "1234abcd5678efgh" : "testtesttesttest";
    std::memcpy(m_adwEncryptionKey, key, 16);
    std::memcpy(m_adwDecryptionKey, key, 16);
    SetPhase(1);
    Handshake(handshake);
    sys_log(0, "SYSTEM: new connection from [%s] fd: %d handshake %u output input_len %d, ptr %p",
            m_stHost.c_str(), m_sock, static_cast<unsigned>(m_dwHandshake),
            buffer_size(m_lpInputBuffer), static_cast<void*>(this));
    Log("SYSTEM: new connection from [%s] fd: %d handshake %u ptr %p", m_stHost.c_str(),
        m_sock, static_cast<unsigned>(m_dwHandshake), static_cast<void*>(this));
    return true;
}

void DESC::SetPhase(int phase) {
    static CInputClose s_inputClose;
    static CInputHandshake s_inputHandshake;
    static CInputLogin s_inputLogin;
    static CInputMain s_inputMain;
    static CInputDead s_inputDead;
    static CInputAuth s_inputAuth;
    m_iPhase=phase;
    TPacketGCPhase packet;
    packet.header=253;
    packet.phase=static_cast<BYTE>(phase);
    Packet(&packet,sizeof packet);
    switch(m_iPhase) {
    case 0: m_pInputProcessor=&s_inputClose; break;
    case 1: m_pInputProcessor=&s_inputHandshake; break;
    case 2: case 3: case 4:
        m_bEncrypted=true; m_pInputProcessor=&s_inputLogin; break;
    case 5: case 6:
        m_bEncrypted=true; m_pInputProcessor=&s_inputMain; break;
    case 10:
        m_bEncrypted=true; m_pInputProcessor=&s_inputAuth;
        sys_log(0,"AUTH_PHASE %p",this); break;
    }
}

//08139340; original ownership/order and non-idempotent buffer fields preserved.
void DESC::Destroy() {
    if(m_pkLoginKey) m_pkLoginKey->Expire();
    if(m_accountTable.id) DESC_MANAGER::instance().DisconnectAccount(m_accountTable.login);
    if(m_pLogFile) { std::fclose(m_pLogFile); m_pLogFile=0; }
    if(m_lpCharacter) { m_lpCharacter->Disconnect(); m_lpCharacter=0; }
    buffer_delete(m_lpOutputBuffer);
    buffer_delete(m_lpInputBuffer);
    event_cancel(&m_pkPingEvent);
    event_cancel(&m_pkDisconnectEvent);
    if(!g_bAuthServer && m_accountTable.login[0] && m_accountTable.passwd[0]) {
        TLogoutPacket packet;
        std::strncpy(packet.login,m_accountTable.login,30); packet.login[30]=0;
        std::strncpy(packet.passwd,m_accountTable.passwd,16); packet.passwd[16]=0;
        db_clientdesc->DBPacket(2,m_dwHandle,&packet,sizeof packet);
    }
    if(m_sock!=-1) {
        sys_log(0,"SYSTEM: closing socket. DESC #%d",m_sock);
        Log("SYSTEM: closing socket. DESC #%d",m_sock);
        fdwatch_del_fd(m_lpFdw,m_sock);
        socket_close(m_sock); m_sock=-1;
    }
}

namespace {
struct DisconnectEventInfo { DESC* desc; };
}

long disconnect_event(LPEVENT event)
{
    DESC* desc=static_cast<DisconnectEventInfo*>(event->info)->desc;
    desc->m_pkDisconnectEvent=0;
    desc->SetPhase(0);
    return 0;
}

bool DESC::DelayedDisconnect(int seconds)
{
    if (m_pkDisconnectEvent) return false;
    void* rawInfo = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(DisconnectEventInfo), &rawInfo);
    static_cast<DisconnectEventInfo*>(rawInfo)->desc = this;
    unsigned int raw=static_cast<unsigned int>(seconds)*static_cast<unsigned int>(passes_per_sec);
    int when;
    std::memcpy(&when,&raw,sizeof(when));
    m_pkDisconnectEvent=event_create_ex(disconnect_event,id,when,const_cast<char*>("desc.cpp"),0x2d8);
    return true;
}

void DESC::DisconnectOfSameLogin()
{
    if (!m_lpCharacter) SetPhase(0);
    else if (!m_pkDisconnectEvent) {
        m_lpCharacter->ChatPacket(1,locale_find("\264\331\270\245\040\304\304\307\273\305\315\277\241\274\255\040\267\316\261\327\300\316\040\307\317\277\251\040\301\242\274\323\300\273\040\301\276\267\341\040\307\325\264\317\264\331\056",g_iUseLocale));
        DelayedDisconnect(5);
    }
}

void DESC::BindAccountTable(TAccountTable* table)
{
    assert(table);
    thecore_memcpy(&m_accountTable, table, sizeof(m_accountTable));
    DESC_MANAGER::instance().ConnectAccount(m_accountTable.login, this);
}

// 0813b160
void DESC::SendRunupMatrixizPacket(const char* quiz)
{
    TPacketGCRunupMatrixQuiz packet{};
    packet.bHeader = 201;
    const std::size_t capacity = sizeof(packet.szQuiz) - 1;
    std::size_t length = std::strlen(quiz);
    if (length > capacity) {
        _sys_err("SendRunupMatrixizPacket", 0x3c1, "runup matrix quiz len is too long (%d > max(%d))",
                 static_cast<int>(length), static_cast<int>(capacity));
        length = capacity;
    }
    std::memcpy(packet.szQuiz, quiz, length);
    packet.szQuiz[length] = '\0';
    Packet(&packet, sizeof(packet));
}

void DESC::SendLoginSuccessPacket()
{
    TPacketGCLoginSuccess packet = {};
    packet.bHeader = 0x20;
    packet.handle = m_dwHandle;
    packet.random_key = DESC_MANAGER::instance().MakeRandomKey(m_dwHandle);
    thecore_memcpy(packet.players, m_accountTable.players, sizeof(packet.players));

    for (int i = 0; i < 4; ++i) {
        CGuild* guild = CGuildManager::instance().GetLinkedGuild(m_accountTable.players[i].dwID);
        if (guild) {
            packet.guild_id[i] = guild->GetID();
            std::strncpy(packet.guild_name[i], guild->GetName(), sizeof(packet.guild_name[i]) - 1);
        }
    }
    Packet(&packet, sizeof(packet));
}
