// 2009 UDP packet registry, server-state reply, and datagram dispatch.
#include "packet_info.h"
#include "input.h"
#include "config.h"
#include "desc.h"
#include "desc_manager.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include <arpa/inet.h>
#include <cstddef>
#include <cstdint>
#include <cstring>

// Registration sizes/IDs are literal target constants, not newer sizeof values.
CPacketInfoUDP::CPacketInfoUDP()
{
    Set(1, 9, "ServerStateRequest", false);
}
CPacketInfoUDP::~CPacketInfoUDP()
{
    Log("udp_packet_info.txt");
}

#pragma pack(push, 1)
struct ServerStateChecker_RequestPacket {
    BYTE header;
    std::uint32_t key;
    std::uint32_t index;
};
struct ServerStateChecker_ResponsePacket {
    BYTE header;
    std::uint32_t key;
    std::uint32_t index;
    BYTE state;
};
#pragma pack(pop)
static_assert(sizeof(ServerStateChecker_RequestPacket) == 9, "2009 UDP state request");
static_assert(sizeof(ServerStateChecker_ResponsePacket) == 10, "2009 UDP state reply");

extern socket_t udp_socket;

CInputUDP::CInputUDP()
{
    static CPacketInfoUDP packetInfo;
    BindPacketInfo(&packetInfo);
}

CInputUDP::~CInputUDP() {}

BYTE CInputUDP::GetType()
{
    return 6;
}

void CInputUDP::Handshake(DESC* desc, const char* data)
{
    std::uint32_t handshake;
    std::memcpy(&handshake, data + 1, sizeof(handshake));
    if (desc->GetHandshake() != handshake) {
        sys_log(0, "UDP: Handshake differs %s", desc->GetHostName());
        return;
    }
    sys_log(0, "UDP: Grant %s:%d", inet_ntoa(m_SockAddr.sin_addr), static_cast<int>(m_SockAddr.sin_port));
    desc->UDPGrant(m_SockAddr);
}

void CInputUDP::StateChecker(const char* data)
{
    ServerStateChecker_RequestPacket request;
    std::memcpy(&request, data, sizeof(request));
    int total, local;
    int* byEmpire;
    DESC_MANAGER::instance().GetUserCount(total, &byEmpire, local);
    ServerStateChecker_ResponsePacket response;
    response.header = 1;
    response.key = request.key;
    response.index = request.index;
    if (g_bNoMoreClient)
        response.state = 0;
    else if (total > g_iFullUserCount)
        response.state = 3;
    else
        response.state = total <= g_iBusyUserCount ? 1 : 2;

    if (sendto(udp_socket, &response, sizeof(response), 0,
               reinterpret_cast<sockaddr*>(&m_SockAddr), 16) < 0) {
        const char* addressNumber = inet_ntoa(m_SockAddr.sin_addr);
        const char* address = inet_ntoa(m_SockAddr.sin_addr);
        _sys_err("StateChecker", 0x5f, "cannot sendto datagram socket : %s , %d",
                 address, static_cast<int>(reinterpret_cast<std::size_t>(addressNumber)));
    }
}

int CInputUDP::Analyze(DESC*, BYTE header, const char* data)
{
    if (header == 1)
        StateChecker(data);
    else
        _sys_err("Analyze", 0x73, "unknown UDP header %u", static_cast<unsigned int>(header));
    return 0;
}

bool CInputUDP::Process(DESC* desc, const void* original, int bytes, int& consumed)
{
    if (!m_pPacketInfo) {
        _sys_err("Process", 0x84, "No packet info has been binded to");
        return true;
    }

    m_iBufferLeft = bytes;
    const unsigned char* data = static_cast<const unsigned char*>(original);
    BYTE lastHeader = 0;
    int lastLength = 0;
    while (m_iBufferLeft > 4) {
        const BYTE header = *data;
        int length;
        const char* name;
        if (!m_pPacketInfo->Get(header, &length, &name)) {
            _sys_err("Process", 0x93, "UNKNOWN HEADER: %d, LAST HEADER: %d(%d), REMAIN BYTES: %d",
                     static_cast<int>(header), static_cast<int>(lastHeader), lastLength, m_iBufferLeft);
            printdata(static_cast<const unsigned char*>(original), bytes);
            return true;
        }
        if (m_iBufferLeft < length)
            return true;
        const int extra = Analyze(desc, header, reinterpret_cast<const char*>(data));
        if (extra < 0)
            return true;
        lastLength = length + extra;
        m_iBufferLeft -= lastLength;
        consumed += lastLength;
        data += lastLength;
        lastHeader = header;
    }
    return true;
}
