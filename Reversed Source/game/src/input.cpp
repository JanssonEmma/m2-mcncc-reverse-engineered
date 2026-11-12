#include <cstring>
#include <cstdio>
#include <arpa/inet.h>
// Recovered base dispatch only; application handlers remain unresolved.
#include "input.h"
#include "login_sim.h"
#include <cstdlib>
extern bool Metin2Server_IsInvalid();
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include <cstdint>
#include "packet.h"
#include "config.h"
#include "TrafficProfiler.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"

// 080cb590
bool IsEmptyAdminPage()
{
    for (const auto& address : g_stAdminPageIP) {
        if (!address.empty())
            return false;
    }
    return true;
}

// 080cb5c0
bool IsAdminPage(const char* ip)
{
    for (const auto& address : g_stAdminPageIP) {
        if (address == ip)
            return true;
    }
    return false;
}

CInputProcessor::CInputProcessor() : m_pPacketInfo(0), m_iBufferLeft(0)
{
    static CPacketInfoCG s_PacketInfo;
    if(!m_pPacketInfo) BindPacketInfo(&s_PacketInfo);
}
CInputProcessor::~CInputProcessor() {}
void CInputProcessor::BindPacketInfo(CPacketInfo* info) { m_pPacketInfo=info; }
bool CInputProcessor::Process(DESC* desc, const void* original, int bytes, int& consumed)
{
    BYTE last_header=0;
    int last_length=0;
    if(!m_pPacketInfo) {
        _sys_err("Process",189,"No packet info has been binded to");
        return true;
    }
    if(test_server) sys_log(0,"Packet Analyze : start");
    m_iBufferLeft=bytes;
    const BYTE* data=static_cast<const BYTE*>(original);
    while(bytes>0) {
        const BYTE header=*data;
        int length;
        const char* name=0;
        if(header==0) length=1;
        else if(!m_pPacketInfo->Get(header,&length,&name)) {
            _sys_err("Process",204,"UNKNOWN HEADER: %d, LAST HEADER: %d(%d), REMAIN BYTES: %d, fd: %d",
                static_cast<int>(header),static_cast<int>(last_header),last_length,m_iBufferLeft,desc->GetSocket());
            printdata(static_cast<const unsigned char*>(original),m_iBufferLeft);
            desc->SetPhase(0);
            return true;
        }
        if(m_iBufferLeft<length) return true;
        if(header!=0) {
            if(test_server && header!=7)
                sys_log(0,"Packet Analyze [Header %d][bufferLeft %d] ",static_cast<int>(header),m_iBufferLeft);
            m_pPacketInfo->Start();
            const int extra=Analyze(desc,header,reinterpret_cast<const char*>(data));
            if(extra<0) return true;
            length=static_cast<int>(static_cast<unsigned int>(length)+static_cast<unsigned int>(extra));
            desc->Log("%s %d",name,length);
            m_pPacketInfo->End();
        }
        if(g_bTrafficProfileOn)
            TrafficProfiler::instance().Report(TrafficProfiler::IODIR_INPUT,header,static_cast<DWORD>(length));
        if(m_pPacketInfo->IsSequencePacket(header)) {
            const BYTE expected=desc->GetSequence();
            if(expected!=data[length-1]) {
                _sys_err("Process",241,"SEQUENCE %x mismatch %u != %u header %u",desc,
                    static_cast<unsigned int>(expected),static_cast<unsigned int>(data[length-1]),static_cast<unsigned int>(header));
                desc->SetPhase(0);
                return true;
            }
            desc->SetNextSequence();
        }
        last_length=length;
        m_iBufferLeft=static_cast<int>(static_cast<unsigned int>(m_iBufferLeft)-static_cast<unsigned int>(length));
        data+=length;
        consumed=static_cast<int>(static_cast<unsigned int>(consumed)+static_cast<unsigned int>(length));
        if(GetType()!=desc->GetInputProcessor()->GetType()) return false;
        last_header=header;
        bytes=m_iBufferLeft;
    }
    return true;
}
BYTE CInputClose::GetType() { return 0; }
int CInputClose::Analyze(DESC*, BYTE, const char*) { return m_iBufferLeft; }

void CInputProcessor::Pong(DESC* desc)
{
    desc->SetPong(true);
    if(Metin2Server_IsInvalid()) {
        const int value=rand();
        if(value==(value/50000)*50000) exit(0);
    }
}

void CInputProcessor::Handshake(DESC* desc,const char* data)
{
    const TPacketCGHandshake* packet=reinterpret_cast<const TPacketCGHandshake*>(data);
    if(desc->GetHandshake()!=packet->dwHandshake) {
        _sys_err("Handshake",302,"Invalid Handshake on %d",desc->GetSocket());
        desc->SetPhase(0);
        return;
    }
    if(!desc->IsPhase(1)) {
        desc->SyncTime(packet->dwTime,packet->lDelta,true);
        return;
    }
    if(desc->SyncTime(packet->dwTime,packet->lDelta,false))
        desc->SetPhase(g_bAuthServer?10:2);
}

//080cd6b0/080cd720; private table intentionally survives the empty destructor.
CInputHandshake::CInputHandshake() {
    CPacketInfoCG* info=new CPacketInfoCG;
    TPacketElement* pong=info->GetElement(254);
    pong->bSequencePacket=false;
    --pong->iSize;
    m_pMainPacketInfo=m_pPacketInfo;
    BindPacketInfo(info);
}
CInputHandshake::~CInputHandshake() {}
BYTE CInputHandshake::GetType() { return 1; }

void CInputHandshake::GuildMarkLogin(DESC* desc, const char*)
{
    if (!guild_mark_server) {
        _sys_err("Analyze", 0x343, "Guild Mark login requested but i'm not a mark server!");
        desc->SetPhase(0);
        return;
    }

    sys_log(0, "MARK_SERVER Try Login");
    sys_log(0, "MARK_SERVER Login OK");
    desc->SetPhase(2);
}

// 080cd790
int CInputHandshake::Analyze(DESC* desc, BYTE header, const char* data)
{
    switch (header) {
        case 0x0a:
            return 0;
        case 0x64:
            GuildMarkLogin(desc, data);
            return 0;
        case 0xfe:
            Pong(desc);
            return 0;
        case 0xff:
            Handshake(desc, data);
            return 0;
        case 0x40:
            break;
        default:
            _sys_err("Analyze", 0x360, "Handshake phase does not handle packet %d (fd %d)",
                     static_cast<int>(header), desc->GetSocket());
            return 0;
    }

    if (m_iBufferLeft <= 1)
        return -1;
    const char* text = data + 1;
    const auto* newline = static_cast<const char*>(std::memchr(text, '\n', m_iBufferLeft - 1));
    if (!newline)
        return -1;
    const char* end = newline;
    if (end != text && end[-1] == '\r')
        --end;
    const std::string command(text, end);
    std::string result = "UNKNOWN";

    if (command == "IS_SERVER_UP") {
        result = g_bNoMoreClient ? "NO" : "YES";
    } else if (command == "IS_PASSPOD_UP") {
        result = g_bNoPasspod ? "NO" : "YES";
    } else if (command == "USER_COUNT") {
        const char* ip = inet_ntoa(desc->GetAddr().sin_addr);
        if (IsEmptyAdminPage() || IsAdminPage(ip)) {
            int total, local;
            int* byEmpire;
            DESC_MANAGER::instance().GetUserCount(total, &byEmpire, local);
            char count[64];
            std::snprintf(count, sizeof(count), "%d %d %d %d %d",
                          total, byEmpire[1], byEmpire[2], byEmpire[3], local);
            result = count;
        } else {
            result = " WEBADMIN : Wrong Connector : ";
            result += ip;
            result += ' ';
        }
    } else if (command == "PACKET_INFO") {
        m_pMainPacketInfo->Log("packet_info.txt");
        result = "OK";
    }

    sys_log(1, "TEXT %s RESULT %s", command.c_str(), result.c_str());
    result += '\n';
    desc->Packet(result.data(), result.size());
    return static_cast<int>(newline - text + 1);
}

// 080cd580
void CInputProcessor::Version(CHARACTER* character, const char* data)
{
    if (!character)
        return;

    char filename[34] = {};
    char timestamp[34] = {};
    std::memcpy(filename, data + 1, 33);
    std::memcpy(timestamp, data + 0x22, 33);
    sys_log(0, "VERSION: %s %s %s", character->GetName(), timestamp, filename);
    if (DESC* desc = character->GetDesc())
        desc->SetClientVersion(timestamp);
}

// 080cd5f0
void CInputProcessor::CRCReport(CHARACTER* character, const char* data)
{
    if (!character)
        return;

    std::uint32_t crc[3];
    std::memcpy(crc, data + 2, sizeof(crc));
    sys_log(0, "CRC_REPORT: %s %u %lu %lu %lu", character->GetName(),
            static_cast<unsigned>(static_cast<BYTE>(data[1])),
            static_cast<unsigned long>(crc[0]), static_cast<unsigned long>(crc[1]),
            static_cast<unsigned long>(crc[2]));
}

// 080cd650
void LoginFailure(DESC* desc, const char* status)
{
    if (!desc)
        return;
    TPacketGCLoginFailure packet;
    packet.header = 7;
    strncpy(packet.szStatus, status, 8);
    packet.szStatus[8] = '\0';
    desc->Packet(&packet, sizeof(packet));
}

std::map<DWORD, CLoginSim*> g_sim;
std::vector<TPlayerTable> g_vec_save;
